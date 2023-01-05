//    scrutiny_codec_v1_0.cpp
//        Encode/Decode functions for the Scrutiny Protocol V1.0
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include <string.h>
#include <stdint.h>

#include "scrutiny_setup.hpp"
#include "scrutiny_software_id.hpp"
#include "scrutiny_tools.hpp"
#include "scrutiny_types.hpp"
#include "scrutiny_main_handler.hpp"
#include "protocol/scrutiny_codec_v1_0.hpp"
#include "scrutiny_common_codecs.hpp"

#if defined(_MSC_VER)
#pragma warning(disable : 4127) // Get rid of constexpr always true condition warning.
#endif

namespace scrutiny
{
    namespace protocol
    {
        //==============================================================
        void ReadMemoryBlocksRequestParser::validate()
        {
            constexpr unsigned int addr_size = sizeof(void *);
            uint32_t cursor = 0;
            uint16_t length;

            while (true)
            {
                if (addr_size + 2 > static_cast<uint16_t>(m_request_datasize - cursor))
                {
                    m_invalid = true;
                    return;
                }

                cursor += addr_size;
                length = codecs::decode_16_bits_big_endian(&m_buffer[cursor]);
                cursor += 2;

                m_required_tx_buffer_size += addr_size + 2 + length;

                if (cursor == m_request_datasize)
                {
                    break;
                }
            }
        }

        void ReadMemoryBlocksRequestParser::init(const Request *request)
        {
            m_buffer = request->data;
            m_request_datasize = request->data_length;
            reset();
            validate();
        }

        void ReadMemoryBlocksRequestParser::next(MemoryBlock *memblock)
        {
            constexpr unsigned int addr_size = sizeof(void *);
            uint16_t length;
            uintptr_t addr;
            if (m_finished || m_invalid)
            {
                return;
            }

            if (addr_size + 2 > static_cast<uint16_t>(m_request_datasize - m_bytes_read))
            {
                m_finished = true;
                m_invalid = true;
                return;
            }

            codecs::decode_address_big_endian(&m_buffer[m_bytes_read], &addr);
            m_bytes_read += addr_size;
            length = codecs::decode_16_bits_big_endian(&m_buffer[m_bytes_read]);
            m_bytes_read += 2;

            memblock->start_address = reinterpret_cast<uint8_t *>(addr);
            memblock->length = length;

            if (m_bytes_read == m_request_datasize)
            {
                m_finished = true;
            }
        }

        void ReadMemoryBlocksRequestParser::reset()
        {
            m_bytes_read = 0;
            m_invalid = false;
            m_finished = false;
            m_required_tx_buffer_size = 0;
        }

        //==============================================================

        void WriteMemoryBlocksRequestParser::init(const Request *request, bool masked_write)
        {
            m_buffer = request->data;
            m_size_limit = request->data_length;
            m_masked_write = masked_write;
            reset();
            validate();
        }

        void WriteMemoryBlocksRequestParser::validate()
        {
            constexpr unsigned int addr_size = sizeof(void *);
            uint16_t cursor = 0;
            uint16_t length;

            while (true)
            {
                if (addr_size + 2 > static_cast<uint16_t>(m_size_limit - cursor))
                {
                    m_invalid = true;
                    return;
                }

                cursor += addr_size;
                length = codecs::decode_16_bits_big_endian(&m_buffer[cursor]);
                cursor += 2;
                cursor += length;
                if (m_masked_write)
                {
                    cursor += length; // With masked write. There is a mask as long as the data it self
                }

                if (cursor > m_size_limit) // Sum of block length is bigger than request length
                {
                    m_invalid = true;
                    return;
                }

                m_required_tx_buffer_size += addr_size + 2;

                if (cursor == m_size_limit) // That's the length in the request
                {
                    break;
                }
            }
        }

        void WriteMemoryBlocksRequestParser::next(MemoryBlock *memblock)
        {
            constexpr unsigned int addr_size = sizeof(void *);
            uint16_t length;
            uintptr_t addr;
            if (m_finished || m_invalid)
            {
                return;
            }

            if (addr_size + 2 > static_cast<uint16_t>(m_size_limit - m_bytes_read))
            {
                m_finished = true;
                m_invalid = true;
                return;
            }

            codecs::decode_address_big_endian(&m_buffer[m_bytes_read], &addr);
            m_bytes_read += addr_size;
            length = codecs::decode_16_bits_big_endian(&m_buffer[m_bytes_read]);
            m_bytes_read += 2;

            if ((length > static_cast<uint16_t>(m_size_limit - m_bytes_read)) ||
                (m_masked_write && (length << 1) > static_cast<uint16_t>(m_size_limit - m_bytes_read)))
            {
                m_invalid = true;
                m_finished = true;
                return;
            }

            memblock->start_address = reinterpret_cast<uint8_t *>(addr);
            memblock->source_data = reinterpret_cast<uint8_t *>(&m_buffer[m_bytes_read]);
            m_bytes_read += length;
            if (m_masked_write)
            {
                memblock->mask = reinterpret_cast<uint8_t *>(&m_buffer[m_bytes_read]);
                m_bytes_read += length;
            }
            else
            {
                memblock->mask = nullptr;
            }
            memblock->length = length;

            if (m_bytes_read == m_size_limit)
            {
                m_finished = true;
            }
        }

        void WriteMemoryBlocksRequestParser::reset()
        {
            m_bytes_read = 0;
            m_invalid = false;
            m_finished = false;
            m_required_tx_buffer_size = 0;
        }

        //==============================================================

        void ReadMemoryBlocksResponseEncoder::init(Response *response, const uint16_t max_size)
        {
            m_size_limit = max_size;
            m_buffer = response->data;
            m_response = response;
            reset();
        }

        void ReadMemoryBlocksResponseEncoder::write(MemoryBlock *memblock)
        {
            constexpr unsigned int addr_size = sizeof(void *);

            if (memblock->length > MAXIMUM_TX_BUFFER_SIZE - addr_size - 2) // Make sure that the addition below doesn't blow up
            {
                m_overflow = true;
                return;
            }

            if (addr_size + 2 + memblock->length > static_cast<uint16_t>(m_size_limit - m_cursor))
            {
                m_overflow = true;
                return;
            }

            m_cursor += codecs::encode_address_big_endian(memblock->start_address, &m_buffer[m_cursor]);
            m_cursor += codecs::encode_16_bits_big_endian(memblock->length, &m_buffer[m_cursor]);
            memcpy(&m_buffer[m_cursor], memblock->start_address, memblock->length);
            m_cursor += memblock->length;

            m_response->data_length = m_cursor;
        }

        void ReadMemoryBlocksResponseEncoder::reset()
        {
            m_cursor = 0;
            m_overflow = false;
        }

        //==============================================================

        void WriteMemoryBlocksResponseEncoder::init(Response *response, uint16_t max_size)
        {
            m_size_limit = max_size;
            m_buffer = response->data;
            m_response = response;
            reset();
        }

        void WriteMemoryBlocksResponseEncoder::write(MemoryBlock *memblock)
        {
            constexpr unsigned int addr_size = sizeof(void *);

            if (addr_size + 2u > static_cast<uint16_t>(m_size_limit - m_cursor))
            {
                m_overflow = true;
                return;
            }

            m_cursor += codecs::encode_address_big_endian(memblock->start_address, &m_buffer[m_cursor]);
            m_cursor += codecs::encode_16_bits_big_endian(memblock->length, &m_buffer[m_cursor]);
            m_response->data_length = static_cast<uint16_t>(m_cursor);
        }

        void WriteMemoryBlocksResponseEncoder::reset()
        {
            m_cursor = 0;
            m_overflow = false;
        }

        //==============================================================

        void GetRPVDefinitionResponseEncoder::init(Response *response, const uint16_t max_size)
        {
            m_size_limit = max_size;
            m_buffer = response->data;
            m_response = response;
            reset();
        }

        void GetRPVDefinitionResponseEncoder::write(const RuntimePublishedValue *rpv)
        {
            // id (2) + type (1) + address size (2,4,8)
            if (2u + 1u > static_cast<uint16_t>(m_size_limit - m_cursor))
            {
                m_overflow = true;
                return;
            }

            m_cursor += codecs::encode_16_bits_big_endian(rpv->id, &m_buffer[m_cursor]);
            m_cursor += codecs::encode_8_bits(static_cast<uint8_t>(rpv->type), &m_buffer[m_cursor]);
            m_response->data_length = m_cursor;
        }

        void GetRPVDefinitionResponseEncoder::reset()
        {
            m_cursor = 0;
            m_overflow = false;
        }

        //==============================================================

        void ReadRPVResponseEncoder::init(Response *response, const uint16_t max_size)
        {
            m_size_limit = max_size;
            m_buffer = response->data;
            m_response = response;
            m_size_limit = max_size;
            reset();
        }

        void ReadRPVResponseEncoder::write(const RuntimePublishedValue *rpv, AnyType v)
        {
            const uint8_t typesize = tools::get_type_size(rpv->type);
            // id (2) + type (1)
            if (2u + typesize > static_cast<uint16_t>(m_size_limit - m_cursor))
            {
                m_overflow = true;
                return;
            }

            if (typesize == 1u || typesize == 2u || typesize == 4u
#if SCRUTINY_SUPPORT_64BITS
                || typesize == 8u
#endif
            )
            {
                m_cursor += codecs::encode_16_bits_big_endian(rpv->id, &m_buffer[m_cursor]);
                m_cursor += codecs::encode_anytype_big_endian(&v, typesize, &m_buffer[m_cursor]);
                m_response->data_length = m_cursor;
            }
        }

        void ReadRPVResponseEncoder::reset()
        {
            m_cursor = 0;
            m_overflow = false;
        }

        //==============================================================

        void WriteRPVResponseEncoder::init(Response *response, const uint16_t max_size)
        {
            m_size_limit = max_size;
            m_buffer = response->data;
            m_response = response;
            m_size_limit = max_size;
            reset();
        }

        void WriteRPVResponseEncoder::write(const RuntimePublishedValue *rpv)
        {
            const uint8_t typesize = tools::get_type_size(rpv->type);
            // id (2) + datalen (1)
            if (2u + 1u > static_cast<uint16_t>(m_size_limit - m_cursor))
            {
                m_overflow = true;
                return;
            }

            m_cursor += codecs::encode_16_bits_big_endian(rpv->id, &m_buffer[m_cursor]);
            m_cursor += codecs::encode_8_bits(typesize, &m_buffer[m_cursor]);

            m_response->data_length = m_cursor;
        }

        void WriteRPVResponseEncoder::reset()
        {
            m_cursor = 0;
            m_overflow = false;
        }

        //==============================================================

        void ReadRPVRequestParser::init(const Request *request)
        {
            m_buffer = request->data;
            m_request_len = request->data_length;
            reset();
            validate();
        }

        void ReadRPVRequestParser::validate()
        {
            // It's a list of 16 bits value. So we need an even number of bytes
            if ((m_request_len & 0x01) != 0)
            {
                m_invalid = true;
                return;
            }
        }

        bool ReadRPVRequestParser::next(uint16_t *id)
        {
            if (m_finished || m_invalid)
            {
                return false;
            }

            if (2 > m_request_len - m_bytes_read)
            {
                m_invalid = true;
                return false;
            }

            *id = codecs::decode_16_bits_big_endian(&m_buffer[m_bytes_read]);
            m_bytes_read += 2;

            if (m_bytes_read == m_request_len)
            {
                m_finished = true;
            }

            return !m_invalid;
        }

        void ReadRPVRequestParser::reset()
        {
            m_bytes_read = 0;
            m_invalid = false;
            m_finished = false;
        }

        // ==================================

        void WriteRPVRequestParser::init(const Request *request, MainHandler *main_handler)
        {
            m_buffer = request->data;
            m_request_len = request->data_length;
            m_main_handler = main_handler;
            reset();
        }

        bool WriteRPVRequestParser::next(RuntimePublishedValue *rpv, AnyType *v)
        {
            bool ok_to_process = false;
            // We can't parse the request if a previous entry was not found.
            if (m_finished || m_invalid)
            {
                return false;
            }

            if (2u > static_cast<uint16_t>(m_request_len - m_bytes_read))
            {
                m_invalid = true;
                return false;
            }

            const uint16_t id = codecs::decode_16_bits_big_endian(&m_buffer[m_bytes_read]);
            m_bytes_read += 2;

            const bool found = m_main_handler->get_rpv(id, rpv);

            // Impossible to know the meaning of the rest of the payload. We need to stop all;
            if (!found)
            {
                m_invalid = true;
                return false;
            }

            const uint8_t typesize = tools::get_type_size(rpv->type);

            if (typesize > static_cast<uint16_t>(m_request_len - m_bytes_read))
            {
                m_invalid = true;
                return false;
            }

            ok_to_process = true;
            switch (typesize)
            {
            case 1:
                v->uint8 = m_buffer[m_bytes_read];
                break;
            case 2:
                v->uint16 = codecs::decode_16_bits_big_endian(&m_buffer[m_bytes_read]);
                break;
            case 4:
                v->uint32 = codecs::decode_32_bits_big_endian(&m_buffer[m_bytes_read]);
                break;
#if SCRUTINY_SUPPORT_64BITS
            case 8:
                v->uint64 = codecs::decode_64_bits_big_endian(&m_buffer[m_bytes_read]);
                break;
#endif
            default:
                ok_to_process = false;
                break;
            }

            // We don't flag error on unsupported types. We skip them
            m_bytes_read += typesize;

            if (m_bytes_read == m_request_len)
            {
                m_finished = true;
            }

            return ok_to_process;
        }

        void WriteRPVRequestParser::reset()
        {
            m_bytes_read = 0;
            m_invalid = false;
            m_finished = false;
        }

        //==============================================================

        //==============================================================

        // ===== Encoding =====
        ResponseCode CodecV1_0::encode_response_protocol_version(const ResponseData::GetInfo::GetProtocolVersion *response_data, Response *response)
        {
            constexpr uint16_t datalen = 2;
            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }

            response->data_length = datalen;
            response->data[0] = response_data->major;
            response->data[1] = response_data->minor;

            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::encode_response_software_id(Response *response)
        {
            constexpr uint16_t datalen = sizeof(scrutiny::software_id);

            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }

            response->data_length = datalen;
            memcpy(response->data, scrutiny::software_id, sizeof(scrutiny::software_id));
            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::encode_response_special_memory_region_count(const ResponseData::GetInfo::GetSpecialMemoryRegionCount *response_data, Response *response)
        {
            constexpr uint16_t readonly_region_count_size = sizeof(ResponseData::GetInfo::GetSpecialMemoryRegionCount::nbr_readonly_region);
            constexpr uint16_t forbidden_region_count_size = sizeof(ResponseData::GetInfo::GetSpecialMemoryRegionCount::nbr_forbidden_region);
            constexpr uint16_t datalen = readonly_region_count_size + forbidden_region_count_size;
            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }
            response->data[0] = response_data->nbr_readonly_region;
            response->data[1] = response_data->nbr_forbidden_region;
            response->data_length = 2;
            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::encode_response_special_memory_region_location(const ResponseData::GetInfo::GetSpecialMemoryRegionLocation *response_data, Response *response)
        {
            constexpr unsigned int addr_size = sizeof(void *);
            constexpr uint16_t region_type_size = sizeof(ResponseData::GetInfo::GetSpecialMemoryRegionLocation::region_type);
            constexpr uint16_t region_index_size = sizeof(ResponseData::GetInfo::GetSpecialMemoryRegionLocation::region_index);
            constexpr uint16_t datalen = region_type_size + region_index_size + 2 * addr_size;

            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }

            response->data[0] = static_cast<uint8_t>(response_data->region_type);
            response->data[1] = response_data->region_index;
            codecs::encode_address_big_endian(response_data->start, &response->data[2]);
            codecs::encode_address_big_endian(response_data->end, &response->data[2 + addr_size]);
            response->data_length = 1 + 1 + addr_size + addr_size;

            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::encode_response_supported_features(const ResponseData::GetInfo::GetSupportedFeatures *response_data, Response *response)
        {
            constexpr uint16_t datalen = 1;

            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }

            response->data[0] = 0x00;
            if (response_data->memory_read)
                response->data[0] |= 0x80;

            if (response_data->memory_write)
                response->data[0] |= 0x40;

            if (response_data->datalogging)
                response->data[0] |= 0x20;

            if (response_data->user_command)
                response->data[0] |= 0x10;

            if (response_data->_64bits)
                response->data[0] |= 0x08;

            response->data_length = 1;
            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::decode_request_get_special_memory_region_location(const Request *request, RequestData::GetInfo::GetSpecialMemoryRegionLocation *request_data)
        {
            request_data->region_type = request->data[0];
            request_data->region_index = request->data[1];
            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::decode_request_get_rpv_definition(const Request *request, RequestData::GetInfo::GetRPVDefinition *request_data)
        {
            constexpr uint16_t start_index_pos = 0;
            constexpr uint16_t start_index_size = sizeof(request_data->start_index);
            constexpr uint16_t count_pos = start_index_size;
            constexpr uint16_t count_size = sizeof(request_data->count);

            constexpr uint16_t datalen = start_index_size + count_size;

            if (request->data_length != datalen)
            {
                return ResponseCode::InvalidRequest;
            }

            request_data->start_index = codecs::decode_16_bits_big_endian(&request->data[start_index_pos]);
            request_data->count = codecs::decode_16_bits_big_endian(&request->data[count_pos]);
            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::decode_request_get_loop_definition(const Request *request, RequestData::GetInfo::GetLoopDefinition *request_data)
        {
            constexpr uint16_t loop_id_len = sizeof(request_data->loop_id);
            constexpr uint16_t datalen = loop_id_len;

            if (request->data_length != datalen)
            {
                return ResponseCode::InvalidRequest;
            }

            request_data->loop_id = request->data[0];
            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::encode_response_get_loop_definition(const ResponseData::GetInfo::GetLoopDefinition *response_data, Response *response)
        {
            constexpr uint16_t loop_id_size = sizeof(response_data->loop_id);
            constexpr uint16_t loop_type_size = sizeof(response_data->loop_type);
            constexpr uint16_t timestep_us_size = sizeof(response_data->loop_type_specific.fixed_freq.timestep_us);
            constexpr uint16_t name_length_size = sizeof(response_data->loop_name_length);

            constexpr uint16_t datalen = loop_id_size + loop_type_size + name_length_size;

            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }

            response->data[0] = response_data->loop_id;
            response->data[1] = response_data->loop_type;

            uint16_t cursor = 2;
            switch (static_cast<scrutiny::LoopType>(response_data->loop_type))
            {
            case scrutiny::LoopType::FIXED_FREQ:
                if (cursor + timestep_us_size > response->data_max_length)
                {
                    return ResponseCode::Overflow;
                }
                cursor += codecs::encode_32_bits_big_endian(response_data->loop_type_specific.fixed_freq.timestep_us, &response->data[cursor]);
                break;
            case scrutiny::LoopType::VARIABLE_FREQ:
                break;
            default:
                return ResponseCode::FailureToProceed;
            }

            uint8_t loop_name_length = response_data->loop_name_length;
            loop_name_length = (loop_name_length > MAX_LOOP_NAME_LENGTH) ? MAX_LOOP_NAME_LENGTH : loop_name_length;

            if (cursor + name_length_size + loop_name_length > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }

            response->data[cursor++] = response_data->loop_name_length;
            memcpy(&response->data[cursor], response_data->loop_name, loop_name_length);
            cursor += loop_name_length;

            response->data_length = cursor;
            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::encode_response_get_rpv_count(const ResponseData::GetInfo::GetRPVCount *response_data, Response *response)
        {
            constexpr uint16_t count_size = sizeof(response_data->count);
            constexpr uint16_t datalen = count_size;
            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }

            codecs::encode_16_bits_big_endian(response_data->count, &response->data[0]);
            response->data_length = datalen;
            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::encode_response_get_loop_count(const ResponseData::GetInfo::GetLoopCount *response_data, Response *response)
        {
            constexpr uint16_t count_size = sizeof(response_data->count);
            constexpr uint16_t datalen = count_size;
            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }

            codecs::encode_8_bits(response_data->count, &response->data[0]);
            response->data_length = datalen;
            return ResponseCode::OK;
        }

        // ============================ CommunicationControl ============================

        ResponseCode CodecV1_0::encode_response_comm_discover(Response *response, const ResponseData::CommControl::Discover *response_data)
        {
            constexpr uint16_t software_id_size = sizeof(scrutiny::software_id);
            constexpr uint16_t display_name_length_size = sizeof(response_data->display_name_length);
            constexpr uint16_t proto_maj_pos = 0;
            constexpr uint16_t proto_maj_size = 1;
            constexpr uint16_t proto_min_pos = proto_maj_pos + proto_maj_size;
            constexpr uint16_t proto_min_size = 1;

            constexpr uint16_t firmware_id_pos = proto_min_pos + proto_min_size;
            constexpr uint16_t display_name_length_pos = firmware_id_pos + software_id_size;
            constexpr uint16_t display_name_pos = display_name_length_pos + display_name_length_size;

            uint8_t display_name_length = response_data->display_name_length;
            display_name_length = (display_name_length > MAX_DISPLAY_NAME_LENGTH) ? MAX_DISPLAY_NAME_LENGTH : display_name_length;

            uint16_t datalen = proto_maj_size + proto_min_size + software_id_size + display_name_length_size + display_name_length;

            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }

            response->data_length = proto_maj_size + proto_min_size + software_id_size + display_name_length_size + display_name_length;
            response->data[proto_maj_pos] = SCRUTINY_PROTOCOL_VERSION_MAJOR(SCRUTINY_ACTUAL_PROTOCOL_VERSION);
            response->data[proto_min_pos] = SCRUTINY_PROTOCOL_VERSION_MINOR(SCRUTINY_ACTUAL_PROTOCOL_VERSION);
            memcpy(&response->data[firmware_id_pos], scrutiny::software_id, software_id_size);
            response->data[display_name_length_pos] = static_cast<uint8_t>(display_name_length);
            memcpy(&response->data[display_name_pos], response_data->display_name, display_name_length); // Probably fails with char 16 bits here...
            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::encode_response_comm_heartbeat(const ResponseData::CommControl::Heartbeat *response_data, Response *response)
        {
            constexpr uint16_t session_id_size = sizeof(response_data->session_id);
            constexpr uint16_t challenge_response_size = sizeof(response_data->challenge_response);
            constexpr uint16_t datalen = session_id_size + challenge_response_size;

            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }

            response->data_length = datalen;
            codecs::encode_32_bits_big_endian(response_data->session_id, &response->data[0]);
            codecs::encode_16_bits_big_endian(response_data->challenge_response, &response->data[4]);

            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::encode_response_comm_get_params(const ResponseData::CommControl::GetParams *response_data, Response *response)
        {
            constexpr uint16_t rx_buffer_size_len = sizeof(response_data->data_rx_buffer_size);
            constexpr uint16_t tx_buffer_size_len = sizeof(response_data->data_tx_buffer_size);
            constexpr uint16_t max_bitrate_size = sizeof(response_data->max_bitrate);
            constexpr uint16_t heartbeat_timeout_size = sizeof(response_data->heartbeat_timeout);
            constexpr uint16_t comm_rx_timeout_size = sizeof(response_data->comm_rx_timeout);
            constexpr uint16_t address_size_size = sizeof(response_data->address_size);
            constexpr uint16_t datalen = rx_buffer_size_len + tx_buffer_size_len + max_bitrate_size + heartbeat_timeout_size + comm_rx_timeout_size + address_size_size;

            constexpr uint16_t rx_buffer_size_pos = 0;
            constexpr uint16_t tx_buffer_size_pos = rx_buffer_size_pos + rx_buffer_size_len;
            constexpr uint16_t max_bitrate_pos = tx_buffer_size_pos + tx_buffer_size_len;
            constexpr uint16_t heartbeat_timeout_pos = max_bitrate_pos + max_bitrate_size;
            constexpr uint16_t comm_rx_timeout_pos = heartbeat_timeout_pos + heartbeat_timeout_size;
            constexpr uint16_t address_size_pos = comm_rx_timeout_pos + comm_rx_timeout_size;

            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }

            response->data_length = datalen;

            codecs::encode_16_bits_big_endian(response_data->data_rx_buffer_size, &response->data[rx_buffer_size_pos]);
            codecs::encode_16_bits_big_endian(response_data->data_tx_buffer_size, &response->data[tx_buffer_size_pos]);
            codecs::encode_32_bits_big_endian(response_data->max_bitrate, &response->data[max_bitrate_pos]);
            codecs::encode_32_bits_big_endian(response_data->heartbeat_timeout, &response->data[heartbeat_timeout_pos]);
            codecs::encode_32_bits_big_endian(response_data->comm_rx_timeout, &response->data[comm_rx_timeout_pos]);
            response->data[address_size_pos] = response_data->address_size; // Size in bytes

            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::encode_response_comm_connect(const ResponseData::CommControl::Connect *response_data, Response *response)
        {
            constexpr uint16_t magic_size = sizeof(response_data->magic);
            constexpr uint16_t session_id_size = sizeof(response_data->session_id);
            constexpr uint16_t datalen = magic_size + session_id_size;

            static_assert(sizeof(response_data->magic) == sizeof(CommControl::CONNECT_MAGIC), "Mismatch between codec definition and protocol constant.");
            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }

            response->data_length = datalen;
            memcpy(&response->data[0], response_data->magic, magic_size);
            codecs::encode_32_bits_big_endian(response_data->session_id, &response->data[magic_size]);

            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::decode_request_comm_discover(const Request *request, RequestData::CommControl::Discover *request_data)
        {
            constexpr uint16_t magic_size = sizeof(CommControl::DISCOVER_MAGIC);
            constexpr uint16_t datalen = magic_size;

            if (request->data_length != datalen)
            {
                return ResponseCode::InvalidRequest;
            }

            memcpy(request_data->magic, request->data, magic_size);

            if (memcmp(CommControl::DISCOVER_MAGIC, request_data->magic, magic_size) != 0)
            {
                return ResponseCode::InvalidRequest;
            }

            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::decode_request_comm_heartbeat(const Request *request, RequestData::CommControl::Heartbeat *request_data)
        {
            constexpr uint16_t session_id_size = sizeof(request_data->session_id);
            constexpr uint16_t challenge_size = sizeof(request_data->challenge);
            constexpr uint16_t datalen = session_id_size + challenge_size;

            if (request->data_length != datalen)
            {
                return ResponseCode::InvalidRequest;
            }

            request_data->session_id = codecs::decode_32_bits_big_endian(&request->data[0]);
            request_data->challenge = codecs::decode_16_bits_big_endian(&request->data[4]);

            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::decode_request_comm_connect(const Request *request, RequestData::CommControl::Connect *request_data)
        {
            constexpr uint16_t magic_size = sizeof(CommControl::DISCOVER_MAGIC);
            constexpr uint16_t datalen = magic_size;

            if (request->data_length != datalen)
            {
                return ResponseCode::InvalidRequest;
            }

            memcpy(request_data->magic, request->data, magic_size);

            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::decode_request_comm_disconnect(const Request *request, RequestData::CommControl::Disconnect *request_data)
        {
            constexpr uint16_t session_id_size = sizeof(request_data->session_id);
            constexpr uint16_t datalen = session_id_size;

            if (request->data_length != datalen)
            {
                return ResponseCode::InvalidRequest;
            }

            request_data->session_id = codecs::decode_32_bits_big_endian(&request->data[0]);
            return ResponseCode::OK;
        }

        // ============================ MemoryControl ============================

        ReadMemoryBlocksRequestParser *CodecV1_0::decode_request_memory_control_read(const Request *request)
        {
            parsers.m_memory_control_read_request_parser.init(request);
            return &parsers.m_memory_control_read_request_parser;
        }

        ReadMemoryBlocksResponseEncoder *CodecV1_0::encode_response_memory_control_read(Response *response, uint16_t max_size)
        {
            response->data_length = 0;
            encoders.m_memory_control_read_response_encoder.init(response, max_size);
            return &encoders.m_memory_control_read_response_encoder;
        }

        WriteMemoryBlocksRequestParser *CodecV1_0::decode_request_memory_control_write(const Request *request, const bool masked_write)
        {
            parsers.m_memory_control_write_request_parser.init(request, masked_write);
            return &parsers.m_memory_control_write_request_parser;
        }

        WriteMemoryBlocksResponseEncoder *CodecV1_0::encode_response_memory_control_write(Response *response, uint16_t max_size)
        {
            response->data_length = 0;
            encoders.m_memory_control_write_response_encoder.init(response, max_size);
            return &encoders.m_memory_control_write_response_encoder;
        }

        GetRPVDefinitionResponseEncoder *CodecV1_0::encode_response_get_rpv_definition(Response *response, uint16_t max_size)
        {
            response->data_length = 0;
            encoders.m_get_rpv_definition_response_encoder.init(response, max_size);
            return &encoders.m_get_rpv_definition_response_encoder;
        }

        ReadRPVResponseEncoder *CodecV1_0::encode_response_memory_control_read_rpv(Response *response, const uint16_t max_size)
        {
            response->data_length = 0;
            encoders.m_read_rpv_response_encoder.init(response, max_size);
            return &encoders.m_read_rpv_response_encoder;
        }

        ReadRPVRequestParser *CodecV1_0::decode_request_memory_control_read_rpv(const Request *request)
        {
            parsers.m_memory_control_read_rpv_parser.init(request);
            return &parsers.m_memory_control_read_rpv_parser;
        }

        WriteRPVResponseEncoder *CodecV1_0::encode_response_memory_control_write_rpv(Response *response, const uint16_t max_size)
        {
            response->data_length = 0;
            encoders.m_write_rpv_response_encoder.init(response, max_size);
            return &encoders.m_write_rpv_response_encoder;
        }

        WriteRPVRequestParser *CodecV1_0::decode_request_memory_control_write_rpv(const Request *request, MainHandler *main_handler)
        {
            parsers.m_memory_control_write_rpv_parser.init(request, main_handler);
            return &parsers.m_memory_control_write_rpv_parser;
        }

#if SCRUTINY_ENABLE_DATALOGGING
        ResponseCode CodecV1_0::encode_response_datalogging_buffer_size(const ResponseData::DataLogControl::GetBufferSize *response_data, Response *response)
        {
            constexpr uint16_t datalen = sizeof(response_data->buffer_size);
            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }
            codecs::encode_32_bits_big_endian(response_data->buffer_size, &response->data[0]);
            response->data_length = datalen;

            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::encode_response_datalogging_status(const ResponseData::DataLogControl::GetStatus *response_data, Response *response)
        {
            constexpr uint16_t datalen = sizeof(response_data->state);
            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }
            codecs::encode_8_bits(response_data->state, &response->data[0]);
            response->data_length = 1;

            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::encode_response_datalogging_get_acquisition_metadata(const ResponseData::DataLogControl::GetAcquisitionMetadata *response_data, Response *response)
        {
            constexpr uint16_t acquisition_id_size = sizeof(response_data->acquisition_id);
            constexpr uint16_t config_id_size = sizeof(response_data->config_id);
            constexpr uint16_t number_of_points_size = sizeof(response_data->number_of_points);
            constexpr uint16_t data_size_size = sizeof(response_data->data_size);
            constexpr uint16_t encoding_size = sizeof(response_data->encoding);

            constexpr uint16_t datalen = acquisition_id_size + config_id_size + number_of_points_size + data_size_size + encoding_size;

            if (datalen > MINIMUM_TX_BUFFER_SIZE && datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }

            uint16_t cursor = 0;
            cursor += codecs::encode_16_bits_big_endian(response_data->acquisition_id, &response->data[cursor]);
            cursor += codecs::encode_16_bits_big_endian(response_data->config_id, &response->data[cursor]);
            cursor += codecs::encode_32_bits_big_endian(response_data->number_of_points, &response->data[cursor]);
            cursor += codecs::encode_32_bits_big_endian(response_data->data_size, &response->data[cursor]);
            cursor += codecs::encode_8_bits(response_data->encoding, &response->data[cursor]);
            response->data_length = cursor;

            return ResponseCode::OK;
        }

        ResponseCode CodecV1_0::encode_response_datalogging_read_acquisition(const ResponseData::DataLogControl::ReadAcquisition *response_data, Response *response, bool *finished)
        {
            constexpr uint16_t minimum_datalen = 16;
            if (minimum_datalen > MINIMUM_TX_BUFFER_SIZE && minimum_datalen > response->data_max_length)
            {
                return ResponseCode::Overflow;
            }
            *finished = false;
            // [0] is set at the end.
            response->data[1] = response_data->rolling_counter;
            codecs::encode_16_bits_big_endian(response_data->acquisition_id, &response->data[2]);

            uint32_t nread = response_data->reader->read(&response->data[4], response->data_max_length - 4);
            response->data_length = nread + 4;
            *response_data->crc = tools::crc32(&response->data[4], nread, *response_data->crc);

            if (response_data->reader->finished() && response->data_length <= response->data_max_length - 4)
            {
                codecs::encode_32_bits_big_endian(*response_data->crc, &response->data[response->data_length]);
                response->data_length += 4;
                *finished = true;
            }

            response->data[0] = static_cast<uint8_t>(*finished);

            return protocol::ResponseCode::OK;
        }

        ResponseCode CodecV1_0::decode_datalogging_configure_request(
            const Request *request,
            RequestData::DataLogControl::Configure *request_data,
            datalogging::Configuration *config)
        {
            if (request->data_length < 16)
            {
                return ResponseCode::InvalidRequest;
            }

            request_data->loop_id = request->data[0];
            request_data->config_id = codecs::decode_16_bits_big_endian(&request->data[1]);

            config->decimation = codecs::decode_16_bits_big_endian(&request->data[3]);
            config->probe_location = request->data[5];
            config->timeout_us = codecs::decode_32_bits_big_endian(&request->data[6]);
            config->trigger.condition = static_cast<datalogging::SupportedTriggerConditions>(request->data[10]);
            config->trigger.hold_time_us = codecs::decode_32_bits_big_endian(&request->data[11]);
            config->trigger.operand_count = request->data[15];

            if (config->trigger.operand_count > datalogging::MAX_OPERANDS)
            {
                return ResponseCode::Overflow;
            }

            uint16_t cursor = 16;
            for (uint_fast8_t i = 0; i < config->trigger.operand_count; i++)
            {
                if (request->data_length < cursor + 1)
                {
                    return ResponseCode::InvalidRequest;
                }

                const datalogging::OperandType optype = static_cast<datalogging::OperandType>(request->data[cursor]);
                config->trigger.operands[i].type = optype;
                cursor++;

                switch (optype)
                {
                case datalogging::OperandType::LITERAL:
                {
                    if (request->data_length < cursor + sizeof(float))
                    {
                        return ResponseCode::InvalidRequest;
                    }
                    config->trigger.operands[i].data.literal.val = codecs::decode_float_big_endian(&request->data[cursor]);
                    cursor += sizeof(float);
                    break;
                }
                case datalogging::OperandType::RPV:
                {
                    if (request->data_length < cursor + sizeof(uint16_t))
                    {
                        return ResponseCode::InvalidRequest;
                    }
                    config->trigger.operands[i].data.rpv.id = codecs::decode_16_bits_big_endian(&request->data[cursor]);
                    cursor += sizeof(uint16_t);
                    break;
                }
                case datalogging::OperandType::VAR:
                {
                    if (request->data_length < cursor + sizeof(uint8_t) + sizeof(void *))
                    {
                        return ResponseCode::InvalidRequest;
                    }
                    config->trigger.operands[i].data.var.datatype = static_cast<scrutiny::VariableType>(request->data[cursor++]);
                    cursor += codecs::decode_address_big_endian(&request->data[cursor], reinterpret_cast<uintptr_t *>(&config->trigger.operands[i].data.var.addr));
                    break;
                }
                case datalogging::OperandType::VARBIT:
                {
                    if (request->data_length < cursor + sizeof(uint8_t) + sizeof(void *))
                    {
                        return ResponseCode::InvalidRequest;
                    }

                    config->trigger.operands[i].data.varbit.datatype = static_cast<scrutiny::VariableType>(request->data[cursor++]);
                    cursor += codecs::decode_address_big_endian(&request->data[cursor], reinterpret_cast<uintptr_t *>(&config->trigger.operands[i].data.varbit.addr));
                    config->trigger.operands[i].data.varbit.bitoffset = request->data[cursor++];
                    config->trigger.operands[i].data.varbit.bitsize = request->data[cursor++];
                    break;
                }
                default:
                {
                    return ResponseCode::InvalidRequest;
                }
                }
            }

            if (request->data_length < cursor + sizeof(uint8_t))
            {
                return ResponseCode::InvalidRequest;
            }

            config->items_count = request->data[cursor++];

            if (config->items_count > SCRUTINY_DATALOGGING_MAX_SIGNAL)
            {
                return ResponseCode::Overflow;
            }

            for (uint_fast8_t i = 0; i < config->items_count; i++)
            {
                if (request->data_length < cursor + sizeof(uint8_t))
                {
                    return ResponseCode::InvalidRequest;
                }

                config->items_to_log[i].type = static_cast<datalogging::LoggableType>(request->data[cursor++]);

                switch (config->items_to_log[i].type)
                {
                case datalogging::LoggableType::MEMORY:
                {
                    if (request->data_length < cursor + sizeof(void *) + sizeof(uint8_t))
                    {
                        return ResponseCode::InvalidRequest;
                    }
                    cursor += codecs::decode_address_big_endian(&request->data[cursor], reinterpret_cast<uintptr_t *>(&config->items_to_log[i].data.memory.address));
                    config->items_to_log[i].data.memory.size = request->data[cursor++];
                    break;
                }
                case datalogging::LoggableType::RPV:
                {
                    if (request->data_length < cursor + sizeof(uint16_t))
                    {
                        return ResponseCode::InvalidRequest;
                    }

                    config->items_to_log[i].data.rpv.id = codecs::decode_16_bits_big_endian(&request->data[cursor]);
                    cursor += sizeof(uint16_t);
                    break;
                }
                case datalogging::LoggableType::TIME:
                {
                    break;
                }
                default:
                {
                    return ResponseCode::InvalidRequest;
                }
                }
            }

            if (cursor != request->data_length)
            {
                return ResponseCode::InvalidRequest;
            }

            return ResponseCode::OK;
        }
#endif
    }
}
