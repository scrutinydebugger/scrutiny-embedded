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

namespace scrutiny
{
    void MainHandler::init(Config* config)
    {
        m_processing_request = false;
        m_disconnect_pending = false;
        m_config = *config;

        m_comm_handler.init(
            m_config.m_rx_buffer, m_config.m_rx_buffer_size,
            m_config.m_tx_buffer, m_config.m_tx_buffer_size,
            &m_timebase, m_config.prng_seed
            );

        check_config();
        if (!m_enabled)
        {
            m_comm_handler.disable();
        }
    }

    void MainHandler::check_config()
    {
        m_enabled = true;
        if (m_config.m_rx_buffer == nullptr || m_config.m_rx_buffer_size < protocol::MINIMUM_RX_BUFFER_SIZE)
        {
            m_enabled = false;
        }

        if (m_config.m_tx_buffer == nullptr || m_config.m_tx_buffer_size < protocol::MINIMUM_TX_BUFFER_SIZE)
        {
            m_enabled = false;
        }
    }

    void MainHandler::process(const uint32_t timestep_us)
    {
        if (!m_enabled)
        {
            m_processing_request = false;
            m_disconnect_pending = false;
            m_comm_handler.reset();
            return;
        }
        m_timebase.step(timestep_us);
        m_comm_handler.process();

        if (m_comm_handler.request_received() && !m_processing_request)
        {
            m_processing_request = true;
            protocol::Response* response = m_comm_handler.prepare_response();
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

    bool MainHandler::get_rpv(uint16_t id, RuntimePublishedValue* rpv)
    {
        const uint16_t rpv_count = m_config.get_rpv_count();
        bool found = false;
        for (uint16_t i=0; i<rpv_count; i++)    // if unset this count will be 0
        {
            if(m_config.get_rpvs_array()[i].id == id)
            {
                found = true;
                *rpv = m_config.get_rpvs_array()[i];
                break;
            }
        }

        return found;
    }

    VariableType MainHandler::get_rpv_type(uint16_t id)
    {
        RuntimePublishedValue rpv;
        const bool found = get_rpv(id, &rpv);
        return (found) ? rpv.type : VariableType::unknown;
    }


    void MainHandler::process_request(const protocol::Request* request, protocol::Response* response)
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
    protocol::ResponseCode MainHandler::process_get_info(const protocol::Request* request, protocol::Response* response)
    {
        union
        {
            protocol::ResponseData::GetInfo::GetProtocolVersion get_protocol_version;
            protocol::ResponseData::GetInfo::GetSpecialMemoryRegionCount get_special_memory_region_count;
            protocol::ResponseData::GetInfo::GetSpecialMemoryRegionLocation get_special_memory_region_location;
            protocol::ResponseData::GetInfo::GetSupportedFeatures get_supproted_features;
            protocol::ResponseData::GetInfo::GetRPVCount get_rpv_count;
        } response_data;

        union
        {
            protocol::RequestData::GetInfo::GetSpecialMemoryRegionLocation get_special_memory_region_location;
            protocol::RequestData::GetInfo::GetRPVDefinition get_rpv_definition;
        } request_data;

        union 
        {
            struct 
            {
                const RuntimePublishedValue* rpvs;
                protocol::GetRPVDefinitionResponseEncoder* get_rpv_definition_encoder;
            } getprv;
            
        } stack;
        

        protocol::ResponseCode code = protocol::ResponseCode::FailureToProceed;

        switch (static_cast<protocol::GetInfo::Subfunction>(request->subfunction_id))
        {
                // =========== [GetprotocolVersion] ==========
            case protocol::GetInfo::Subfunction::GetprotocolVersion:
            {
                response_data.get_protocol_version.major = SCRUTINY_PROTOCOL_VERSION_MAJOR(SCRUTINY_ACTUAL_PROTOCOL_VERSION);
                response_data.get_protocol_version.minor = SCRUTINY_PROTOCOL_VERSION_MINOR(SCRUTINY_ACTUAL_PROTOCOL_VERSION);
                code = m_codec.encode_response_protocol_version(&response_data.get_protocol_version, response);
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
                response_data.get_supproted_features.memory_read = true;	
                response_data.get_supproted_features.memory_write = m_config.memory_write_enable;
                response_data.get_supproted_features.datalog_acquire = false;   // todo
                response_data.get_supproted_features.user_command = m_config.is_user_command_callback_set();

                code = m_codec.encode_response_supported_features(&response_data.get_supproted_features, response);
                break;
            }
                // =========== [GetSpecialMemoryRegionCount] ==========
            case protocol::GetInfo::Subfunction::GetSpecialMemoryRegionCount:
            {
                response_data.get_special_memory_region_count.nbr_readonly_region = m_config.readonly_ranges_count();
                response_data.get_special_memory_region_count.nbr_forbidden_region = m_config.forbidden_ranges_count();
                code = m_codec.encode_response_special_memory_region_count(&response_data.get_special_memory_region_count, response);
                break;
            }

                // =========== [GetSpecialMemoryLocation] ==========
            case protocol::GetInfo::Subfunction::GetSpecialMemoryLocation:
            {
                code = m_codec.decode_request_get_special_memory_region_location(request, &request_data.get_special_memory_region_location);
                if (code != protocol::ResponseCode::OK)
                {
                    break;
                }

                if (static_cast<protocol::GetInfo::MemoryRegionType>(request_data.get_special_memory_region_location.region_type) == protocol::GetInfo::MemoryRegionType::ReadOnly)
                {
                    if (!m_config.is_readonly_address_range_set())
                    {
                        code = protocol::ResponseCode::FailureToProceed;
                        break;
                    }

                    if (request_data.get_special_memory_region_location.region_index >= m_config.readonly_ranges_count())
                    {
                        code = protocol::ResponseCode::FailureToProceed;
                        break;
                    }

                    const uint8_t index = request_data.get_special_memory_region_location.region_index;
                    response_data.get_special_memory_region_location.start = reinterpret_cast<uintptr_t>(m_config.readonly_ranges()[index].start);
                    response_data.get_special_memory_region_location.end = reinterpret_cast<uintptr_t>(m_config.readonly_ranges()[index].end);
                }
                else if (static_cast<protocol::GetInfo::MemoryRegionType>(request_data.get_special_memory_region_location.region_type) == protocol::GetInfo::MemoryRegionType::Forbidden)
                {
                    if (!m_config.is_forbidden_address_range_set())
                    {
                        code = protocol::ResponseCode::FailureToProceed;
                        break;
                    }

                    if (request_data.get_special_memory_region_location.region_index >= m_config.forbidden_ranges_count())
                    {
                        code = protocol::ResponseCode::FailureToProceed;
                        break;
                    }

                    const uint8_t index = request_data.get_special_memory_region_location.region_index;
                    response_data.get_special_memory_region_location.start = reinterpret_cast<uintptr_t>(m_config.forbidden_ranges()[index].start);
                    response_data.get_special_memory_region_location.end = reinterpret_cast<uintptr_t>(m_config.forbidden_ranges()[index].end);
                }
                else
                {
                    code = protocol::ResponseCode::InvalidRequest;
                    break;
                }

                response_data.get_special_memory_region_location.region_type = request_data.get_special_memory_region_location.region_type;
                response_data.get_special_memory_region_location.region_index = request_data.get_special_memory_region_location.region_index;

                code = m_codec.encode_response_special_memory_region_location(&response_data.get_special_memory_region_location, response);
                break;
            }
                // =================================

            case protocol::GetInfo::Subfunction::GetRuntimePublishedValuesCount:
            {
                response_data.get_rpv_count.count = m_config.get_rpv_count();
                code = m_codec.encode_response_get_rpv_count(&response_data.get_rpv_count, response);
                break;
            }
                // =================================

            case protocol::GetInfo::Subfunction::GetRuntimePublishedValuesDefinition:
            {
                code = m_codec.decode_request_get_rpv_definition(request, &request_data.get_rpv_definition);
                if (code != protocol::ResponseCode::OK)
                {
                    break;
                }

                stack.getprv.get_rpv_definition_encoder = m_codec.encode_response_get_rpv_definition(response, m_comm_handler.tx_buffer_size());

                if (request_data.get_rpv_definition.start_index >= m_config.get_rpv_count())
                {
                    code = protocol::ResponseCode::FailureToProceed;
                    break;
                }

                if (request_data.get_rpv_definition.start_index + request_data.get_rpv_definition.count > m_config.get_rpv_count())
                {
                    code = protocol::ResponseCode::FailureToProceed;
                    break;
                }
                
                
                stack.getprv.rpvs = m_config.get_rpvs_array();
                for (int32_t i=request_data.get_rpv_definition.start_index; i<request_data.get_rpv_definition.start_index + request_data.get_rpv_definition.count; i++)
                {
                    stack.getprv.get_rpv_definition_encoder->write(&stack.getprv.rpvs[i]);
                    if (stack.getprv.get_rpv_definition_encoder->overflow())	// If it doesn't fit the transmit buffer
                    {
                        code = protocol::ResponseCode::Overflow;
                        break;
                    }
                }

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

    // ============= [CommControl] ============
    protocol::ResponseCode MainHandler::process_comm_control(const protocol::Request* request, protocol::Response* response)
    {
        union
        {
            protocol::ResponseData::CommControl::Connect connect;
            protocol::ResponseData::CommControl::Discover discover;
            protocol::ResponseData::CommControl::GetParams get_params;
            protocol::ResponseData::CommControl::Heartbeat heartbeat;
        }response_data;

        union
        {
            protocol::RequestData::CommControl::Connect connect;
            protocol::RequestData::CommControl::Discover discover;
            protocol::RequestData::CommControl::Disconnect disconnect;
            protocol::RequestData::CommControl::Heartbeat heartbeat;
        }request_data;     

        protocol::ResponseCode code = protocol::ResponseCode::FailureToProceed;

        switch (static_cast<protocol::CommControl::Subfunction>(request->subfunction_id))
        {
                // =========== [Discover] ==========
            case protocol::CommControl::Subfunction::Discover:
            {
                code = m_codec.decode_request_comm_discover(request, &request_data.discover);
                if (code != protocol::ResponseCode::OK)
                    break;

                // Magic validation is done by the codec.
                response_data.discover.display_name = m_config.display_name;
                response_data.discover.display_name_length = strnlen(m_config.display_name, scrutiny::protocol::MAX_DISPLAY_NAME_LENGTH);

                code = m_codec.encode_response_comm_discover(response, &response_data.discover);
                break;
            }

                // =========== [Heartbeat] ==========
            case protocol::CommControl::Subfunction::Heartbeat:
            {
                code = m_codec.decode_request_comm_heartbeat(request, &request_data.heartbeat);
                if (code != protocol::ResponseCode::OK)
                    break;

                if (request_data.heartbeat.session_id != m_comm_handler.get_session_id())
                {
                    code = protocol::ResponseCode::InvalidRequest;
                    break;
                }

                const bool success = m_comm_handler.heartbeat(request_data.heartbeat.challenge);
                if (!success)
                {
                    code = protocol::ResponseCode::InvalidRequest;
                    break;
                }

                response_data.heartbeat.session_id = m_comm_handler.get_session_id();
                response_data.heartbeat.challenge_response = ~request_data.heartbeat.challenge;

                code = m_codec.encode_response_comm_heartbeat(&response_data.heartbeat, response);
                break;
            }

                // =========== [GetParams] ==========
            case protocol::CommControl::Subfunction::GetParams:
            {
                response_data.get_params.data_tx_buffer_size = m_config.m_tx_buffer_size;
                response_data.get_params.data_rx_buffer_size = m_config.m_rx_buffer_size;
                response_data.get_params.max_bitrate = m_config.max_bitrate;
                response_data.get_params.comm_rx_timeout = SCRUTINY_COMM_RX_TIMEOUT_US;
                response_data.get_params.heartbeat_timeout = SCRUTINY_COMM_HEARTBEAT_TMEOUT_US;
                response_data.get_params.address_size = sizeof(void*);
                code = m_codec.encode_response_comm_get_params(&response_data.get_params, response);
                break;
            }
                // =========== [Connect] ==========
            case protocol::CommControl::Subfunction::Connect:
            {
                code = m_codec.decode_request_comm_connect(request, &request_data.connect);
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

                response_data.connect.session_id = m_comm_handler.get_session_id();
                memcpy(response_data.connect.magic, protocol::CommControl::CONNECT_MAGIC, sizeof(protocol::CommControl::CONNECT_MAGIC));
                code = m_codec.encode_response_comm_connect(&response_data.connect, response);
                break;
            }

                // =========== [Diconnect] ==========
            case protocol::CommControl::Subfunction::Disconnect:
            {
                code = m_codec.decode_request_comm_disconnect(request, &request_data.disconnect);
                if (code != protocol::ResponseCode::OK)
                    break;

                if (m_comm_handler.is_connected())
                {
                    if (m_comm_handler.get_session_id() == request_data.disconnect.session_id)
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


    protocol::ResponseCode MainHandler::process_memory_control(const protocol::Request* request, protocol::Response* response)
    {
        protocol::ResponseCode code = protocol::ResponseCode::FailureToProceed;
        
        // Make sure the compiler optimize stack space. Because it may well not (don't trust this guy.)
        union 
        {
            struct 
            {
                protocol::ReadMemoryBlocksRequestParser* readmem_parser;
                protocol::ReadMemoryBlocksResponseEncoder* readmem_encoder;
                protocol::MemoryBlock block;
            } read_mem;

            struct
            {
                protocol::MemoryBlock block;
                protocol::WriteMemoryBlocksRequestParser* writemem_parser;
                protocol::WriteMemoryBlocksResponseEncoder* writemem_encoder;
            } write_mem;

            struct
            {
                protocol::ReadRPVRequestParser* readrpv_parser;
                protocol::ReadRPVResponseEncoder* readrpv_encoder;

                RuntimePublishedValue rpv;
                scrutiny::AnyType v;
                uint16_t id;
                bool success;

            } read_rpv;
            
            struct
            {
                protocol::WriteRPVRequestParser* writerpv_parser;
                protocol::WriteRPVResponseEncoder* writerpv_encoder;
                RuntimePublishedValue rpv;
                scrutiny::AnyType v;
                bool success;
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
                if (! stack.read_mem.readmem_parser->is_valid())
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
                    stack.read_mem.readmem_parser->next(& stack.read_mem.block);

                    if (!stack.read_mem.readmem_parser->is_valid())
                    {
                        code = protocol::ResponseCode::InvalidRequest;
                        break;
                    }

                    if (touches_forbidden_region(& stack.read_mem.block))
                    {
                        code = protocol::ResponseCode::Forbidden;
                        break;
                    }

                    stack.read_mem.readmem_encoder->write(&stack.read_mem.block);
                    // We don't check overflow here as we rely on the request parser to be right on the required buffer size.
                }
                break;
            }

                // =========== [Write] ==========
            case protocol::MemoryControl::Subfunction::Write:			// fall through
            case protocol::MemoryControl::Subfunction::WriteMasked:
            {
                const bool masked = static_cast<protocol::MemoryControl::Subfunction>(request->subfunction_id) == protocol::MemoryControl::Subfunction::WriteMasked;
                code = protocol::ResponseCode::OK;

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
                        for (uint16_t i=0; i<stack.write_mem.block.length; i++)
                        {
                            temp = stack.write_mem.block.start_address[i];
                            temp |= (stack.write_mem.block.source_data[i] & stack.write_mem.block.mask[i]); // Bit to 1
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
                if (!m_config.is_read_published_values_configured())
                {
                    code = protocol::ResponseCode::UnsupportedFeature;
                    break;
                }

                stack.read_rpv.readrpv_parser = m_codec.decode_request_memory_control_read_rpv(request );
                stack.read_rpv.readrpv_encoder = m_codec.encode_response_memory_control_read_rpv(response, m_comm_handler.tx_buffer_size());

                if (!stack.read_rpv.readrpv_parser->is_valid())
                {
                    code = protocol::ResponseCode::InvalidRequest;
                    break;
                }

                stack.read_rpv.success = true;;

                while (!stack.read_rpv.readrpv_parser->finished())
                {
                    const bool ok_to_process = stack.read_rpv.readrpv_parser->next(&stack.read_rpv.id);

                    if (!stack.read_rpv.readrpv_parser->is_valid())
                    {
                         stack.read_rpv.success = false;
                        code = protocol::ResponseCode::InvalidRequest;
                        break;
                    }

                    if (ok_to_process)
                    {
                        const bool rpv_found = get_rpv(stack.read_rpv.id, &stack.read_rpv.rpv);
                        if (!rpv_found)
                        {
                             stack.read_rpv.success = false;
                            code = protocol::ResponseCode::FailureToProceed;
                            break;
                        }

                        const bool callback_success = m_config.get_rpv_read_callback()(stack.read_rpv.rpv, &stack.read_rpv.v);
                        if (!callback_success)
                        {
                             stack.read_rpv.success = false;
                            code = protocol::ResponseCode::FailureToProceed;
                            break;
                        }

                        stack.read_rpv.readrpv_encoder->write(&stack.read_rpv.rpv, stack.read_rpv.v);

                        if (stack.read_rpv.readrpv_encoder->overflow())
                        {
                             stack.read_rpv.success = false;
                            code = protocol::ResponseCode::Overflow;
                            break;
                        }
                    }
                }

                if ( stack.read_rpv.success)
                {
                    code = protocol::ResponseCode::OK;
                }

                break;
            }

            case protocol::MemoryControl::Subfunction::WriteRPV:
            {
                if (!m_config.is_write_published_values_configured())
                {
                    code = protocol::ResponseCode::UnsupportedFeature;
                    break;
                }

                stack.write_rpv.writerpv_parser = m_codec.decode_request_memory_control_write_rpv(request, this );
                stack.write_rpv.writerpv_encoder = m_codec.encode_response_memory_control_write_rpv(response, m_comm_handler.tx_buffer_size());

                if (!stack.write_rpv.writerpv_parser->is_valid())
                {
                    code = protocol::ResponseCode::InvalidRequest;
                    break;
                }

                stack.write_rpv.success = true;
                while (!stack.write_rpv.writerpv_parser->finished())
                {
                    const bool ok_to_process = stack.write_rpv.writerpv_parser->next(&stack.write_rpv.rpv, &stack.write_rpv.v);

                    if (!stack.write_rpv.writerpv_parser->is_valid())
                    {
                        stack.write_rpv.success = false;
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
                        stack.write_rpv.success = false;
                        code = protocol::ResponseCode::FailureToProceed;
                        break;
                    }

                    stack.write_rpv.writerpv_encoder->write(&stack.write_rpv.rpv);

                    if (stack.write_rpv.writerpv_encoder->overflow())
                    {
                        stack.write_rpv.success = false;
                        code = protocol::ResponseCode::Overflow;
                        break;
                    }
                }

                if (stack.write_rpv.success)
                {
                    code = protocol::ResponseCode::OK;
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


    bool MainHandler::touches_forbidden_region(const protocol::MemoryBlock* block)
    {
        if (!m_config.is_forbidden_address_range_set())
        {
            return false;
        }

        const uintptr_t block_start = reinterpret_cast<uintptr_t>(block->start_address);
        const uintptr_t block_end = block_start + block->length;

        for (unsigned int i = 0; i < m_config.forbidden_ranges_count(); i++)
        {
            const AddressRange& range = m_config.forbidden_ranges()[i];

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

    bool MainHandler::touches_readonly_region(const protocol::MemoryBlock* block)
    {
        if (!m_config.is_readonly_address_range_set())
        {
            return false;
        }

        const uintptr_t block_start = reinterpret_cast<uintptr_t>(block->start_address);
        const uintptr_t block_end = block_start + block->length;
        for (unsigned int i = 0; i < m_config.readonly_ranges_count(); i++)
        {
            const AddressRange& range = m_config.readonly_ranges()[i];

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

    protocol::ResponseCode MainHandler::process_user_command(const protocol::Request* request, protocol::Response* response)
    {
        protocol::ResponseCode code = protocol::ResponseCode::FailureToProceed;

        if (m_config.is_user_command_callback_set())
        {
            uint16_t response_data_length = 0;
            //caling user callback;
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