//    scrutiny_main_handler.cpp
//        The main scrutiny class to be manipulated by the user.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include <string.h>

#include "scrutiny_main_handler.hpp"
#include "scrutiny_software_id.hpp"
#include "scrutiny_common_codecs.hpp"

namespace scrutiny
{
    void MainHandler::init(const Config *config)
    {
        m_processing_request = false;
        m_disconnect_pending = false;
        m_config = *config;

        m_comm_handler.init(
            m_config.m_rx_buffer, m_config.m_rx_buffer_size,
            m_config.m_tx_buffer, m_config.m_tx_buffer_size,
            &m_timebase, m_config.prng_seed);

        check_config();
        if (!m_enabled)
        {
            m_comm_handler.disable();
        }

        // If there's an init error witht he comm handler, we disable as well.
        if (!m_comm_handler.is_enabled())
        {
            m_enabled = false;
        }

#if SCRUTINY_ENABLE_DATALOGGING
        m_datalogger.init(this, &m_timebase, m_config.m_datalogger_buffer, m_config.m_datalogger_buffer_size);
#endif
    }

    void MainHandler::check_config()
    {
        m_enabled = true;
        if (m_config.m_rx_buffer == nullptr || m_config.m_rx_buffer_size < protocol::MINIMUM_RX_BUFFER_SIZE)
        {
            m_enabled = false;
        }

        if (m_config.m_rx_buffer == nullptr || m_config.m_rx_buffer_size > protocol::MAXIMUM_RX_BUFFER_SIZE)
        {
            m_enabled = false;
        }

        if (m_config.m_tx_buffer == nullptr || m_config.m_tx_buffer_size < protocol::MINIMUM_TX_BUFFER_SIZE)
        {
            m_enabled = false;
        }

        if (m_config.m_tx_buffer == nullptr || m_config.m_tx_buffer_size > protocol::MAXIMUM_TX_BUFFER_SIZE)
        {
            m_enabled = false;
        }
    }

#if SCRUTINY_ENABLE_DATALOGGING

    /// @brief Copy memory from src to dst (makes a memcpy) but respect the forbodden region constraints given by the user.
    /// @param dst Destination buffer
    /// @param src Source buffer
    /// @param size Number of bytes to copy
    /// @return true on success. False on failure
    bool MainHandler::read_memory(void *dst, const void *src, const uint32_t size) const
    {
        if (touches_forbidden_region(src, size))
        {
            return false;
        }

        memcpy(dst, src, size);
        return true;
    }

    /// @brief Reads a variable from the memory
    /// @param addr Location of the variable in memory
    /// @param variable_type Type of variable to read
    /// @param val The output structure that supports all types.
    /// @return true on success. False on failure
    bool MainHandler::fetch_variable(const void *addr, const VariableType variable_type, AnyType *val) const
    {
        uint8_t typesize = tools::get_type_size(variable_type);
        if (typesize == 0 || typesize > sizeof(AnyType))
        {
            return false;
        }

        if (read_memory(val, addr, typesize) == false)
        {
            memset(val, 0, sizeof(AnyType));
            return false;
        }
        return true;
    }

    /// @brief Reads a bitfield variable from the memory
    /// @param addr Location of the variable in memory
    /// @param var_tt Variable Type Type (no size in type). (uint, sint, float, etc.)
    /// @param bitoffset Offset of the start of the data
    /// @param bitsize Size of the data to read
    /// @param val The output structure that supports all types.
    /// @param output_type The type of the data read. The size is adjusted to be as small as possible.
    /// @return true on success. False on failure
    bool MainHandler::fetch_variable_bitfield(
        const void *addr,
        const VariableTypeType var_tt,
        const uint_fast8_t bitoffset,
        const uint_fast8_t bitsize,
        AnyType *val,
        VariableType *output_type) const
    {
        bool success = true;
        const uint_fast8_t fetch_required_size = ((bitoffset + bitsize - 1) >> 3) + 1;
        const uint_fast8_t output_required_size = ((bitsize - 1) >> 3) + 1;
        const VariableTypeSize fetch_type_size = tools::get_required_type_size(fetch_required_size);
        const VariableTypeSize output_type_size = tools::get_required_type_size(output_required_size);
        const VariableType fetch_variable_type = tools::make_type(VariableTypeType::_uint, fetch_type_size);
        const VariableType output_variable_type = tools::make_type(var_tt, output_type_size);

        if (touches_forbidden_region(addr, tools::get_type_size(fetch_type_size)))
        {
            success = false;
        }
        else if (bitsize == 0)
        {
            success = false;
        }
        else if (var_tt == VariableTypeType::_sint || var_tt == VariableTypeType::_uint || var_tt == VariableTypeType::_boolean)
        {
            success = fetch_variable(addr, fetch_variable_type, val);
            if (success)
            {
                if (fetch_type_size == VariableTypeSize::_8)
                {
                    val->uint8 >>= bitoffset;
                }
                else if (fetch_type_size == VariableTypeSize::_16)
                {
                    val->uint16 >>= bitoffset;
                }
                else if (fetch_type_size == VariableTypeSize::_32)
                {
                    val->uint32 >>= bitoffset;
                }
#if SCRUTINY_SUPPORT_64BITS
                else if (fetch_type_size == VariableTypeSize::_64)
                {
                    val->uint64 >>= bitoffset;
                }
#endif
                else // Unsupported
                {
                    success = false;
                }

                if (success)
                {
                    AnyTypeFast mask;
                    uint_fast8_t i;
                    if (output_type_size == VariableTypeSize::_8)
                    {
                        mask.uint8 = 1;
                        for (i = 1; i < bitsize; i++)
                        {
                            mask.uint8 |= (static_cast<uint_fast8_t>(1) << i);
                        }
                        val->uint8 &= mask.uint8;
                        if (var_tt == VariableTypeType::_sint)
                        {
                            if (val->uint8 >> (bitsize - 1))
                            {
                                val->uint8 |= (~mask.uint8);
                            }
                        }
                    }
                    else if (output_type_size == VariableTypeSize::_16)
                    {
                        mask.uint16 = 0x1FF;
                        for (i = 9; i < bitsize; i++)
                        {
                            mask.uint16 |= (static_cast<uint_fast16_t>(1) << i);
                        }
                        val->uint16 &= mask.uint16;
                        if (var_tt == VariableTypeType::_sint)
                        {
                            if (val->uint16 >> (bitsize - 1))
                            {
                                val->uint16 |= (~mask.uint16);
                            }
                        }
                    }
                    else if (output_type_size == VariableTypeSize::_32)
                    {
                        mask.uint32 = 0x1FFFF;
                        for (i = 17; i < bitsize; i++)
                        {
                            mask.uint32 |= (static_cast<uint_fast32_t>(1) << i);
                        }
                        val->uint32 &= mask.uint32;
                        if (var_tt == VariableTypeType::_sint)
                        {
                            if (val->uint32 >> (bitsize - 1))
                            {
                                val->uint32 |= (~mask.uint32);
                            }
                        }
                    }
#if SCRUTINY_SUPPORT_64BITS
                    else if (output_type_size == VariableTypeSize::_64)
                    {
                        mask.uint64 = 0x1FFFFFFFF;
                        for (i = 33; i < bitsize; i++)
                        {
                            mask.uint64 |= (static_cast<uint_fast64_t>(1) << i);
                        }
                        val->uint64 &= mask.uint64;
                        if (var_tt == VariableTypeType::_sint)
                        {
                            if (val->uint64 >> (bitsize - 1))
                            {
                                val->uint64 |= (~mask.uint64);
                            }
                        }
                    }
#endif
                    else // Unsupported
                    {
                        success = false;
                    }
                }
            }
        }

        if (!success)
        {
            *output_type = VariableType::unknown;
            memset(val, 0, sizeof(AnyType));
        }
        else
        {
            *output_type = output_variable_type;
        }

        return success;
    }

    void MainHandler::process_datalogging(void)
    {
    }

#endif
    void MainHandler::process(const uint32_t timestep_us)
    {
        if (!m_enabled)
        {
            m_processing_request = false;
            m_disconnect_pending = false;
            m_comm_handler.reset();
#if SCRUTINY_ENABLE_DATALOGGING
            m_datalogger.reset();
#endif
            return;
        }
        m_timebase.step(timestep_us);
        m_comm_handler.process();
#if SCRUTINY_ENABLE_DATALOGGING
        process_datalogging();
#endif

        if (m_comm_handler.request_received() && !m_processing_request)
        {
            m_processing_request = true;
            protocol::Response *response = m_comm_handler.prepare_response();
            process_request(m_comm_handler.get_request(), response);

            m_comm_handler.send_response(response);
        }

        if (m_processing_request)
        {
            if (!m_comm_handler.transmitting())
            {
                m_comm_handler.wait_next_request(); // Allow reception of next request
                m_processing_request = false;

                if (m_disconnect_pending)
                {
                    m_comm_handler.disconnect();
                    m_disconnect_pending = false;
                }
            }
        }
    }

    bool MainHandler::get_rpv(const uint16_t id, RuntimePublishedValue *rpv) const
    {
        const uint16_t rpv_count = m_config.get_rpv_count();
        bool found = false;
        for (uint16_t i = 0; i < rpv_count; i++) // if unset this count will be 0
        {
            if (m_config.get_rpvs_array()[i].id == id)
            {
                found = true;
                *rpv = m_config.get_rpvs_array()[i];
                break;
            }
        }

        return found;
    }

    VariableType MainHandler::get_rpv_type(const uint16_t id) const
    {
        RuntimePublishedValue rpv;
        const bool found = get_rpv(id, &rpv);
        return (found) ? rpv.type : VariableType::unknown;
    }

    void MainHandler::process_request(const protocol::Request *const request, protocol::Response *const response)
    {
        protocol::ResponseCode code = protocol::ResponseCode::FailureToProceed;
        response->reset();

        response->command_id = request->command_id;
        response->subfunction_id = request->subfunction_id;

        switch (static_cast<protocol::CommandId>(request->command_id))
        {
            // ============= [GetInfo] ============
        case protocol::CommandId::GetInfo:
            code = process_get_info(request, response);
            break;

            // ============= [CommControl] ============
        case protocol::CommandId::CommControl:
            code = process_comm_control(request, response);
            break;

            // ============= [MemoryControl] ============
        case protocol::CommandId::MemoryControl:
            code = process_memory_control(request, response);
            break;

            // ============= [DataLogControl] ===========
        case protocol::CommandId::DataLogControl:
            break;

            // ============= [UserCommand] ===========
        case protocol::CommandId::UserCommand:
            code = process_user_command(request, response);
            break;

            // ============================================
        default:
            code = protocol::ResponseCode::UnsupportedFeature;
            break;
        }

        response->response_code = static_cast<uint8_t>(code);
        if (code != protocol::ResponseCode::OK)
        {
            response->data_length = 0;
        }
    }

    // ============= [GetInfo] ============
    protocol::ResponseCode MainHandler::process_get_info(const protocol::Request *const request, protocol::Response *const response)
    {
        union
        {
            struct
            {
                protocol::ResponseData::GetInfo::GetProtocolVersion response_data;
            } get_protocol_version;

            struct
            {
                protocol::ResponseData::GetInfo::GetSpecialMemoryRegionCount response_data;
            } get_special_memory_region_count;

            struct
            {
                protocol::RequestData::GetInfo::GetSpecialMemoryRegionLocation request_data;
                protocol::ResponseData::GetInfo::GetSpecialMemoryRegionLocation response_data;
            } get_special_memory_region_location;

            struct
            {
                protocol::ResponseData::GetInfo::GetSupportedFeatures response_data;
            } get_supproted_features;

            struct
            {
                protocol::ResponseData::GetInfo::GetRPVCount response_data;
            } get_rpv_count;

            struct
            {
                const RuntimePublishedValue *rpvs;
                protocol::RequestData::GetInfo::GetRPVDefinition request_data;
                protocol::GetRPVDefinitionResponseEncoder *response_encoder;
            } get_prv_def;

        } stack;

        protocol::ResponseCode code = protocol::ResponseCode::FailureToProceed;

        switch (static_cast<protocol::GetInfo::Subfunction>(request->subfunction_id))
        {
            // =========== [GetprotocolVersion] ==========
        case protocol::GetInfo::Subfunction::GetprotocolVersion:
        {
            stack.get_protocol_version.response_data.major = SCRUTINY_PROTOCOL_VERSION_MAJOR(SCRUTINY_ACTUAL_PROTOCOL_VERSION);
            stack.get_protocol_version.response_data.minor = SCRUTINY_PROTOCOL_VERSION_MINOR(SCRUTINY_ACTUAL_PROTOCOL_VERSION);
            code = m_codec.encode_response_protocol_version(&stack.get_protocol_version.response_data, response);
            break;
        }

            // =========== [GetSoftwareID] ==========
        case protocol::GetInfo::Subfunction::GetSoftwareId:
        {
            code = m_codec.encode_response_software_id(response);
            break;
        }
            // =========== [GetSupportedFeatures] ==========
        case protocol::GetInfo::Subfunction::GetSupportedFeatures:
        {
            stack.get_supproted_features.response_data.memory_read = true;
            stack.get_supproted_features.response_data.memory_write = m_config.memory_write_enable;
#if SCRUTINY_ENABLE_DATALOGGING
            stack.get_supproted_features.response_data.datalogging = m_config.is_datalogging_configured();
#else
            stack.get_supproted_features.response_data.datalogging = false;
#endif
            stack.get_supproted_features.response_data.user_command = m_config.is_user_command_callback_set();
#if SCRUTINY_SUPPORT_64BITS
            stack.get_supproted_features.response_data._64bits = true;
#else
            stack.get_supproted_features.response_data._64bits = false;
#endif

            code = m_codec.encode_response_supported_features(&stack.get_supproted_features.response_data, response);
            break;
        }
            // =========== [GetSpecialMemoryRegionCount] ==========
        case protocol::GetInfo::Subfunction::GetSpecialMemoryRegionCount:
        {
            stack.get_special_memory_region_count.response_data.nbr_readonly_region = m_config.readonly_ranges_count();
            stack.get_special_memory_region_count.response_data.nbr_forbidden_region = m_config.forbidden_ranges_count();
            code = m_codec.encode_response_special_memory_region_count(&stack.get_special_memory_region_count.response_data, response);
            break;
        }

            // =========== [GetSpecialMemoryLocation] ==========
        case protocol::GetInfo::Subfunction::GetSpecialMemoryLocation:
        {
            code = m_codec.decode_request_get_special_memory_region_location(request, &stack.get_special_memory_region_location.request_data);
            if (code != protocol::ResponseCode::OK)
            {
                break;
            }

            if (static_cast<protocol::GetInfo::MemoryRegionType>(stack.get_special_memory_region_location.request_data.region_type) == protocol::GetInfo::MemoryRegionType::ReadOnly)
            {
                if (!m_config.is_readonly_address_range_set())
                {
                    code = protocol::ResponseCode::FailureToProceed;
                    break;
                }

                if (stack.get_special_memory_region_location.request_data.region_index >= m_config.readonly_ranges_count())
                {
                    code = protocol::ResponseCode::FailureToProceed;
                    break;
                }

                const uint8_t index = stack.get_special_memory_region_location.request_data.region_index;
                stack.get_special_memory_region_location.response_data.start = reinterpret_cast<uintptr_t>(m_config.readonly_ranges()[index].start);
                stack.get_special_memory_region_location.response_data.end = reinterpret_cast<uintptr_t>(m_config.readonly_ranges()[index].end);
            }
            else if (static_cast<protocol::GetInfo::MemoryRegionType>(stack.get_special_memory_region_location.request_data.region_type) == protocol::GetInfo::MemoryRegionType::Forbidden)
            {
                if (!m_config.is_forbidden_address_range_set())
                {
                    code = protocol::ResponseCode::FailureToProceed;
                    break;
                }

                if (stack.get_special_memory_region_location.request_data.region_index >= m_config.forbidden_ranges_count())
                {
                    code = protocol::ResponseCode::FailureToProceed;
                    break;
                }

                const uint8_t index = stack.get_special_memory_region_location.request_data.region_index;
                stack.get_special_memory_region_location.response_data.start = reinterpret_cast<uintptr_t>(m_config.forbidden_ranges()[index].start);
                stack.get_special_memory_region_location.response_data.end = reinterpret_cast<uintptr_t>(m_config.forbidden_ranges()[index].end);
            }
            else
            {
                code = protocol::ResponseCode::InvalidRequest;
                break;
            }

            stack.get_special_memory_region_location.response_data.region_type = stack.get_special_memory_region_location.request_data.region_type;
            stack.get_special_memory_region_location.response_data.region_index = stack.get_special_memory_region_location.request_data.region_index;

            code = m_codec.encode_response_special_memory_region_location(&stack.get_special_memory_region_location.response_data, response);
            break;
        }
            // =================================

        case protocol::GetInfo::Subfunction::GetRuntimePublishedValuesCount:
        {
            stack.get_rpv_count.response_data.count = m_config.get_rpv_count();
            code = m_codec.encode_response_get_rpv_count(&stack.get_rpv_count.response_data, response);
            break;
        }
            // =================================

        case protocol::GetInfo::Subfunction::GetRuntimePublishedValuesDefinition:
        {
            code = m_codec.decode_request_get_rpv_definition(request, &stack.get_prv_def.request_data);
            if (code != protocol::ResponseCode::OK)
            {
                break;
            }

            stack.get_prv_def.response_encoder = m_codec.encode_response_get_rpv_definition(response, m_comm_handler.tx_buffer_size());

            if (stack.get_prv_def.request_data.start_index >= m_config.get_rpv_count())
            {
                code = protocol::ResponseCode::FailureToProceed;
                break;
            }

            if (stack.get_prv_def.request_data.start_index + stack.get_prv_def.request_data.count > m_config.get_rpv_count())
            {
                code = protocol::ResponseCode::FailureToProceed;
                break;
            }

            stack.get_prv_def.rpvs = m_config.get_rpvs_array();
            for (int32_t i = stack.get_prv_def.request_data.start_index; i < stack.get_prv_def.request_data.start_index + stack.get_prv_def.request_data.count; i++)
            {
                stack.get_prv_def.response_encoder->write(&stack.get_prv_def.rpvs[i]);
                if (stack.get_prv_def.response_encoder->overflow()) // If it doesn't fit the transmit buffer
                {
                    code = protocol::ResponseCode::Overflow;
                    break;
                }
            }

            break;
        }
            // =================================
        default:
        {
            code = protocol::ResponseCode::UnsupportedFeature;
            break;
        }
        }

        return code;
    }

    // ============= [CommControl] ============
    protocol::ResponseCode MainHandler::process_comm_control(const protocol::Request *const request, protocol::Response *const response)
    {
        union
        {
            struct
            {
                protocol::RequestData::CommControl::Discover request_data;
                protocol::ResponseData::CommControl::Discover response_data;
            } discover;

            struct
            {
                protocol::RequestData::CommControl::Connect request_data;
                protocol::ResponseData::CommControl::Connect response_data;
            } connect;

            struct
            {
                protocol::ResponseData::CommControl::GetParams response_data;
            } get_params;

            struct
            {
                protocol::RequestData::CommControl::Heartbeat request_data;
                protocol::ResponseData::CommControl::Heartbeat response_data;
            } heartbeat;

            struct
            {
                protocol::RequestData::CommControl::Disconnect request_data;
            } disconnect;
        } stack;

        protocol::ResponseCode code = protocol::ResponseCode::FailureToProceed;

        switch (static_cast<protocol::CommControl::Subfunction>(request->subfunction_id))
        {
            // =========== [Discover] ==========
        case protocol::CommControl::Subfunction::Discover:
        {
            code = m_codec.decode_request_comm_discover(request, &stack.discover.request_data);
            if (code != protocol::ResponseCode::OK)
                break;

            // Magic validation is done by the codec.
            stack.discover.response_data.display_name = m_config.display_name;
            stack.discover.response_data.display_name_length = static_cast<uint8_t>(strnlen(m_config.display_name, scrutiny::protocol::MAX_DISPLAY_NAME_LENGTH));

            code = m_codec.encode_response_comm_discover(response, &stack.discover.response_data);
            break;
        }

            // =========== [Heartbeat] ==========
        case protocol::CommControl::Subfunction::Heartbeat:
        {
            code = m_codec.decode_request_comm_heartbeat(request, &stack.heartbeat.request_data);
            if (code != protocol::ResponseCode::OK)
                break;

            if (stack.heartbeat.request_data.session_id != m_comm_handler.get_session_id())
            {
                code = protocol::ResponseCode::InvalidRequest;
                break;
            }

            const bool success = m_comm_handler.heartbeat(stack.heartbeat.request_data.challenge);
            if (!success)
            {
                code = protocol::ResponseCode::InvalidRequest;
                break;
            }

            stack.heartbeat.response_data.session_id = m_comm_handler.get_session_id();
            stack.heartbeat.response_data.challenge_response = ~stack.heartbeat.request_data.challenge;

            code = m_codec.encode_response_comm_heartbeat(&stack.heartbeat.response_data, response);
            break;
        }

            // =========== [GetParams] ==========
        case protocol::CommControl::Subfunction::GetParams:
        {
            stack.get_params.response_data.data_tx_buffer_size = m_config.m_tx_buffer_size;
            stack.get_params.response_data.data_rx_buffer_size = m_config.m_rx_buffer_size;
            stack.get_params.response_data.max_bitrate = m_config.max_bitrate;
            stack.get_params.response_data.comm_rx_timeout = SCRUTINY_COMM_RX_TIMEOUT_US;
            stack.get_params.response_data.heartbeat_timeout = SCRUTINY_COMM_HEARTBEAT_TMEOUT_US;
            stack.get_params.response_data.address_size = sizeof(void *);
            code = m_codec.encode_response_comm_get_params(&stack.get_params.response_data, response);
            break;
        }
            // =========== [Connect] ==========
        case protocol::CommControl::Subfunction::Connect:
        {
            code = m_codec.decode_request_comm_connect(request, &stack.connect.request_data);
            if (code != protocol::ResponseCode::OK)
            {
                break;
            }

            if (m_comm_handler.is_connected())
            {
                code = protocol::ResponseCode::Busy;
                break;
            }

            if (m_comm_handler.connect() == false)
            {
                code = protocol::ResponseCode::FailureToProceed;
                break;
            }

            stack.connect.response_data.session_id = m_comm_handler.get_session_id();
            memcpy(stack.connect.response_data.magic, protocol::CommControl::CONNECT_MAGIC, sizeof(protocol::CommControl::CONNECT_MAGIC));
            code = m_codec.encode_response_comm_connect(&stack.connect.response_data, response);
            break;
        }

            // =========== [Diconnect] ==========
        case protocol::CommControl::Subfunction::Disconnect:
        {
            code = m_codec.decode_request_comm_disconnect(request, &stack.disconnect.request_data);
            if (code != protocol::ResponseCode::OK)
                break;

            if (m_comm_handler.is_connected())
            {
                if (m_comm_handler.get_session_id() == stack.disconnect.request_data.session_id)
                {
                    m_disconnect_pending = true;
                }
                else
                {
                    code = protocol::ResponseCode::InvalidRequest;
                    break;
                }
            }

            // empty data
            code = protocol::ResponseCode::OK;
            break;
        }

            // =================================
        default:
        {
            code = protocol::ResponseCode::UnsupportedFeature;
            break;
        }
        }

        return code;
    }

    protocol::ResponseCode MainHandler::process_memory_control(const protocol::Request *const request, protocol::Response *const response)
    {
        protocol::ResponseCode code = protocol::ResponseCode::FailureToProceed;

        // Make sure the compiler optimize stack space. Because it may well not (don't trust this guy.)
        union
        {
            struct
            {
                protocol::ReadMemoryBlocksRequestParser *readmem_parser;
                protocol::ReadMemoryBlocksResponseEncoder *readmem_encoder;
                MemoryBlock block;
            } read_mem;

            struct
            {
                MemoryBlock block;
                protocol::WriteMemoryBlocksRequestParser *writemem_parser;
                protocol::WriteMemoryBlocksResponseEncoder *writemem_encoder;
            } write_mem;

            struct
            {
                protocol::ReadRPVRequestParser *readrpv_parser;
                protocol::ReadRPVResponseEncoder *readrpv_encoder;

                RuntimePublishedValue rpv;
                scrutiny::AnyType v;
                uint16_t id;
            } read_rpv;

            struct
            {
                protocol::WriteRPVRequestParser *writerpv_parser;
                protocol::WriteRPVResponseEncoder *writerpv_encoder;
                RuntimePublishedValue rpv;
                scrutiny::AnyType v;
            } write_rpv;

        } stack;

        switch (static_cast<protocol::MemoryControl::Subfunction>(request->subfunction_id))
        {
            // =========== [Read] ==========
        case protocol::MemoryControl::Subfunction::Read:
        {
            code = protocol::ResponseCode::OK;

            stack.read_mem.readmem_parser = m_codec.decode_request_memory_control_read(request);
            stack.read_mem.readmem_encoder = m_codec.encode_response_memory_control_read(response, m_comm_handler.tx_buffer_size());

            // We avoid playing in memory unless we are 100% sure the request is good.
            if (!stack.read_mem.readmem_parser->is_valid())
            {
                code = protocol::ResponseCode::InvalidRequest;
                break;
            }

            if (stack.read_mem.readmem_parser->required_tx_buffer_size() > m_comm_handler.tx_buffer_size())
            {
                code = protocol::ResponseCode::Overflow;
                break;
            }

            while (!stack.read_mem.readmem_parser->finished())
            {
                stack.read_mem.readmem_parser->next(&stack.read_mem.block);

                if (!stack.read_mem.readmem_parser->is_valid())
                {
                    code = protocol::ResponseCode::InvalidRequest;
                    break;
                }

                if (touches_forbidden_region(&stack.read_mem.block))
                {
                    code = protocol::ResponseCode::Forbidden;
                    break;
                }

                stack.read_mem.readmem_encoder->write(&stack.read_mem.block);

                if (stack.read_mem.readmem_encoder->overflow())
                {
                    code = protocol::ResponseCode::Overflow;
                    break;
                }
            }
            break;
        }

            // =========== [Write] ==========
        case protocol::MemoryControl::Subfunction::Write: // fall through
        case protocol::MemoryControl::Subfunction::WriteMasked:
        {
            const bool masked = static_cast<protocol::MemoryControl::Subfunction>(request->subfunction_id) == protocol::MemoryControl::Subfunction::WriteMasked;
            code = protocol::ResponseCode::OK;

            if (m_config.memory_write_enable == false)
            {
                code = protocol::ResponseCode::Forbidden;
                break;
            }

            stack.write_mem.writemem_parser = m_codec.decode_request_memory_control_write(request, masked);
            stack.write_mem.writemem_encoder = m_codec.encode_response_memory_control_write(response, m_comm_handler.tx_buffer_size());
            if (!stack.write_mem.writemem_parser->is_valid())
            {
                code = protocol::ResponseCode::InvalidRequest;
                break;
            }

            if (stack.write_mem.writemem_parser->required_tx_buffer_size() > m_comm_handler.tx_buffer_size())
            {
                code = protocol::ResponseCode::Overflow;
                break;
            }

            while (!stack.write_mem.writemem_parser->finished())
            {
                stack.write_mem.writemem_parser->next(&stack.write_mem.block);

                if (!stack.write_mem.writemem_parser->is_valid())
                {
                    code = protocol::ResponseCode::InvalidRequest;
                    break;
                }

                if (touches_forbidden_region(&stack.write_mem.block))
                {
                    code = protocol::ResponseCode::Forbidden;
                    break;
                }

                if (touches_readonly_region(&stack.write_mem.block))
                {
                    code = protocol::ResponseCode::Forbidden;
                    break;
                }

                stack.write_mem.writemem_encoder->write(&stack.write_mem.block);
                // We don't check overflow here as we rely on the request parser to be right on the required buffer size.

                if (!masked)
                {
                    memcpy(stack.write_mem.block.start_address, stack.write_mem.block.source_data, stack.write_mem.block.length);
                }
                else
                {
                    uint8_t temp;
                    for (uint16_t i = 0; i < stack.write_mem.block.length; i++)
                    {
                        temp = stack.write_mem.block.start_address[i];
                        temp |= (stack.write_mem.block.source_data[i] & stack.write_mem.block.mask[i]);    // Bit to 1
                        temp &= (stack.write_mem.block.source_data[i] | (~stack.write_mem.block.mask[i])); // Bit to 0
                        stack.write_mem.block.start_address[i] = temp;
                    }
                }
            }
            break;
        }

        // =========== [Read RPV] ==========
        case protocol::MemoryControl::Subfunction::ReadRPV:
        {
            code = protocol::ResponseCode::OK;
            if (!m_config.is_read_published_values_configured())
            {
                code = protocol::ResponseCode::UnsupportedFeature;
                break;
            }

            stack.read_rpv.readrpv_parser = m_codec.decode_request_memory_control_read_rpv(request);
            stack.read_rpv.readrpv_encoder = m_codec.encode_response_memory_control_read_rpv(response, m_comm_handler.tx_buffer_size());

            if (!stack.read_rpv.readrpv_parser->is_valid())
            {
                code = protocol::ResponseCode::InvalidRequest;
                break;
            }

            while (!stack.read_rpv.readrpv_parser->finished())
            {
                const bool ok_to_process = stack.read_rpv.readrpv_parser->next(&stack.read_rpv.id);

                if (!stack.read_rpv.readrpv_parser->is_valid())
                {
                    code = protocol::ResponseCode::InvalidRequest;
                    break;
                }

                if (ok_to_process)
                {
                    const bool rpv_found = get_rpv(stack.read_rpv.id, &stack.read_rpv.rpv);
                    if (!rpv_found)
                    {
                        code = protocol::ResponseCode::FailureToProceed;
                        break;
                    }

                    const bool callback_success = m_config.get_rpv_read_callback()(stack.read_rpv.rpv, &stack.read_rpv.v);
                    if (!callback_success)
                    {
                        code = protocol::ResponseCode::FailureToProceed;
                        break;
                    }

                    stack.read_rpv.readrpv_encoder->write(&stack.read_rpv.rpv, stack.read_rpv.v);

                    if (stack.read_rpv.readrpv_encoder->overflow())
                    {
                        code = protocol::ResponseCode::Overflow;
                        break;
                    }
                }
            }
            break;
        }

        case protocol::MemoryControl::Subfunction::WriteRPV:
        {
            code = protocol::ResponseCode::OK;
            if (!m_config.is_write_published_values_configured())
            {
                code = protocol::ResponseCode::UnsupportedFeature;
                break;
            }

            stack.write_rpv.writerpv_parser = m_codec.decode_request_memory_control_write_rpv(request, this);
            stack.write_rpv.writerpv_encoder = m_codec.encode_response_memory_control_write_rpv(response, m_comm_handler.tx_buffer_size());

            if (!stack.write_rpv.writerpv_parser->is_valid())
            {
                code = protocol::ResponseCode::InvalidRequest;
                break;
            }

            while (!stack.write_rpv.writerpv_parser->finished())
            {
                const bool ok_to_process = stack.write_rpv.writerpv_parser->next(&stack.write_rpv.rpv, &stack.write_rpv.v);

                if (!stack.write_rpv.writerpv_parser->is_valid())
                {
                    code = protocol::ResponseCode::InvalidRequest;
                    break;
                }

                if (!ok_to_process)
                {
                    continue;
                }

                const bool write_success = m_config.get_rpv_write_callback()(stack.write_rpv.rpv, &stack.write_rpv.v);
                if (!write_success)
                {
                    code = protocol::ResponseCode::FailureToProceed;
                    break;
                }

                stack.write_rpv.writerpv_encoder->write(&stack.write_rpv.rpv);

                if (stack.write_rpv.writerpv_encoder->overflow())
                {
                    code = protocol::ResponseCode::Overflow;
                    break;
                }
            }

            break;
        }

            // =================================
        default:
        {
            code = protocol::ResponseCode::UnsupportedFeature;
            break;
        }
        }

        return code;
    }

    bool MainHandler::touches_forbidden_region(const MemoryBlock *block) const
    {
        return touches_forbidden_region(block->start_address, block->length);
    }

    bool MainHandler::touches_forbidden_region(const void *addr_start, const size_t length) const
    {
        if (!m_config.is_forbidden_address_range_set())
        {
            return false;
        }

        const uintptr_t block_start = reinterpret_cast<uintptr_t>(addr_start);
        const uintptr_t block_end = block_start + length;

        for (unsigned int i = 0; i < m_config.forbidden_ranges_count(); i++)
        {
            const AddressRange &range = m_config.forbidden_ranges()[i];

            if (block_start >= reinterpret_cast<uintptr_t>(range.start) && block_start <= reinterpret_cast<uintptr_t>(range.end))
            {
                return true;
            }

            if (block_end >= reinterpret_cast<uintptr_t>(range.start) && block_end <= reinterpret_cast<uintptr_t>(range.end))
            {
                return true;
            }
        }
        return false;
    }

    bool MainHandler::touches_readonly_region(const MemoryBlock *block) const
    {
        return touches_readonly_region(block->start_address, block->length);
    }

    bool MainHandler::touches_readonly_region(const void *addr_start, const size_t length) const
    {
        if (!m_config.is_readonly_address_range_set())
        {
            return false;
        }

        const uintptr_t block_start = reinterpret_cast<uintptr_t>(addr_start);
        const uintptr_t block_end = block_start + length;
        for (unsigned int i = 0; i < m_config.readonly_ranges_count(); i++)
        {
            const AddressRange &range = m_config.readonly_ranges()[i];

            if (block_start >= reinterpret_cast<uintptr_t>(range.start) && block_start <= reinterpret_cast<uintptr_t>(range.end))
            {
                return true;
            }

            if (block_end >= reinterpret_cast<uintptr_t>(range.start) && block_end <= reinterpret_cast<uintptr_t>(range.end))
            {
                return true;
            }
        }
        return false;
    }

    protocol::ResponseCode MainHandler::process_user_command(const protocol::Request *const request, protocol::Response *const response)
    {
        protocol::ResponseCode code = protocol::ResponseCode::FailureToProceed;

        if (m_config.is_user_command_callback_set())
        {
            uint16_t response_data_length = 0;
            // caling user callback;
            m_config.user_command_callback(request->subfunction_id, request->data, request->data_length, response->data, &response_data_length, m_comm_handler.tx_buffer_size());
            if (response_data_length > m_comm_handler.tx_buffer_size())
            {
                code = protocol::ResponseCode::Overflow;
            }
            else
            {
                response->data_length = response_data_length;
                code = protocol::ResponseCode::OK;
            }
        }
        else
        {
            code = protocol::ResponseCode::UnsupportedFeature;
        }

        return code;
    }
}