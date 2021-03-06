//    scrutiny_codec_v1_0.h
//        Definitions of encode/decode functions for the scrutiny protocol V1.0
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_CODEC_V1_0___
#define ___SCRUTINY_CODEC_V1_0___

#include <stdint.h>
#include "scrutiny_protocol_definitions.hpp"
#include "scrutiny_software_id.hpp"
#include "scrutiny_types.hpp"

namespace scrutiny
{
    namespace protocol
    {
        const unsigned int REQUEST_OVERHEAD = 8;
        const unsigned int RESPONSE_OVERHEAD = 9;

        class ReadMemoryBlocksRequestParser
        {
        public:
            ReadMemoryBlocksRequestParser();
            void init(const Request* request);
            void next(MemoryBlock* memblock);
            inline bool finished() { return m_finished; };
            inline bool is_valid() { return !m_invalid; };
            inline uint32_t required_tx_buffer_size() { return m_required_tx_buffer_size; }
            void reset();

        protected:
            void validate();

            uint8_t* m_buffer;
            uint32_t m_bytes_read;
            uint32_t m_size_limit;
            uint32_t m_required_tx_buffer_size;
            bool m_finished;
            bool m_invalid;
        };

        class ReadMemoryBlocksResponseEncoder
        {
        public:
            ReadMemoryBlocksResponseEncoder();
            void init(Response* response, const uint32_t max_size);
            void write(MemoryBlock* memblock);
            inline bool overflow() { return m_overflow; };
            void reset();

        protected:
            uint8_t* m_buffer;
            Response* m_response;
            uint32_t m_cursor;
            uint32_t m_size_limit;
            bool m_overflow;
        };

        class WriteMemoryBlocksRequestParser
        {
        public:
            WriteMemoryBlocksRequestParser();
            void init(const Request* request, bool masked_write);
            void next(MemoryBlock* memblock);
            inline bool finished() { return m_finished; };
            inline bool is_valid() { return !m_invalid; };
            inline uint32_t required_tx_buffer_size() { return m_required_tx_buffer_size; }
            void reset();

        protected:
            void validate();

            uint8_t* m_buffer;
            uint32_t m_bytes_read;
            uint32_t m_size_limit;
            uint32_t m_required_tx_buffer_size;
            bool m_finished;
            bool m_invalid;
            bool m_masked_write;
        };


        class WriteMemoryBlocksResponseEncoder
        {
        public:
            WriteMemoryBlocksResponseEncoder();
            void init(Response* response, uint32_t max_size);
            void write(MemoryBlock* memblock);
            inline bool overflow() { return m_overflow; };
            void reset();

        protected:
            uint8_t* m_buffer;
            Response* m_response;
            uint32_t m_cursor;
            uint32_t m_size_limit;
            bool m_overflow;
        };


        class GetRPVDefinitionResponseEncoder
        {
        public:
            GetRPVDefinitionResponseEncoder();
            void init(Response* response, const uint32_t max_size);
            void write(const RuntimePublishedValue* rpv);
            inline bool overflow() { return m_overflow; };
            void reset();

        protected:
            uint8_t* m_buffer;
            Response* m_response;
            uint32_t m_cursor;
            uint32_t m_size_limit;
            bool m_overflow;
        };

        class ReadRPVResponseEncoder
        {
        public:
            ReadRPVResponseEncoder();
            void init(Response* response, const uint32_t max_size);
            void write(const RuntimePublishedValue* rpv, AnyType v);
            inline bool overflow() { return m_overflow; };
            void reset();

        protected:
            uint8_t* m_buffer;
            Response* m_response;
            uint32_t m_cursor;
            uint32_t m_size_limit;
            bool m_overflow;
            bool m_unsupported_type;
        };


        class ReadRPVRequestParser
        {
        public:
            ReadRPVRequestParser();
            void init(const Request* request, const RuntimePublishedValue *rpvs, const uint16_t len);
            bool next(RuntimePublishedValue* rpv);
            inline bool finished() { return m_finished; };
            inline bool is_valid() { return !m_invalid; };
            inline bool all_known_rpv() { return !m_not_found; };
            inline uint32_t required_tx_buffer_size() { return m_required_tx_buffer_size; }
            void reset();

        protected:
            void validate();

            uint8_t* m_buffer;
            uint16_t m_bytes_read;
            uint16_t m_request_len;
            uint32_t m_required_tx_buffer_size;
            bool m_finished;
            bool m_unsupported_type;
            bool m_invalid;
            bool m_not_found;
            const RuntimePublishedValue *m_rpvs;
            uint16_t m_rpv_table_len;
        };

        class WriteRPVResponseEncoder
        {
        public:
            WriteRPVResponseEncoder();
            void init(Response* response, const uint32_t max_size);
            void write(const RuntimePublishedValue* rpv);
            inline bool overflow() { return m_overflow; };
            void reset();

        protected:
            uint8_t* m_buffer;
            Response* m_response;
            uint32_t m_cursor;
            uint32_t m_size_limit;
            bool m_overflow;
            bool m_unsupported_type;
        };

        class WriteRPVRequestParser
        {
        public:
            WriteRPVRequestParser();
            void init(const Request* request, const RuntimePublishedValue *rpvs, const uint16_t len);
            bool next(RuntimePublishedValue* rpv, AnyType* v);
            inline bool finished() { return m_finished; };
            inline bool is_valid() { return !m_invalid; };
            inline bool all_known_rpv() { return !m_not_found; };
            inline uint32_t required_tx_buffer_size() { return m_required_tx_buffer_size; }
            void reset();

        protected:
            void validate();

            uint8_t* m_buffer;
            uint16_t m_bytes_read;
            uint16_t m_request_len;
            uint32_t m_required_tx_buffer_size;
            bool m_finished;
            bool m_invalid;
            bool m_not_found;
            const RuntimePublishedValue *m_rpvs;
            uint16_t m_rpv_table_len;
        };
        

        namespace ResponseData
        {
            namespace GetInfo
            {
                struct GetProtocolVersion
                {
                    uint8_t major;
                    uint8_t minor;
                };

                struct GetSupportedFeatures
                {
                    bool memory_read;
                    bool memory_write;
                    bool datalog_acquire;
                    bool user_command;
                };

                struct GetSpecialMemoryRegionCount
                {
                    uint8_t nbr_readonly_region;
                    uint8_t nbr_forbidden_region;
                };

                struct GetSpecialMemoryRegionLocation
                {
                    uint8_t region_type;
                    uint8_t region_index;
                    uint64_t start;
                    uint64_t end;
                };

                struct GetRPVCount
                {
                    uint16_t count;
                };
            }

            namespace CommControl
            {
                struct Discover
                {
                    uint8_t display_name_length;
                    const char* display_name;
                };
                struct Heartbeat
                {
                    uint32_t session_id;
                    uint16_t challenge_response;
                };
                struct GetParams
                {
                    uint16_t data_rx_buffer_size;
                    uint16_t data_tx_buffer_size;
                    uint32_t max_bitrate;
                    uint32_t comm_rx_timeout;
                    uint32_t heartbeat_timeout;
                    uint8_t address_size;
                };
                struct Connect
                {
                    uint8_t magic[sizeof(protocol::CommControl::CONNECT_MAGIC)];
                    uint32_t session_id;
                };
            }
        }


        namespace RequestData
        {
            namespace GetInfo
            {
                struct GetSpecialMemoryRegionLocation
                {
                    uint8_t region_type;
                    uint8_t region_index;
                };

                struct GetRPVDefinition
                {
                    uint16_t start_index;
                    uint16_t count;
                };
            }

            namespace CommControl
            {
                struct Discover
                {
                    uint8_t magic[sizeof(protocol::CommControl::DISCOVER_MAGIC)];
                };

                struct Heartbeat
                {
                    uint32_t session_id;
                    uint16_t challenge;
                };

                struct Connect
                {
                    uint8_t magic[sizeof(protocol::CommControl::CONNECT_MAGIC)];
                };

                struct Disconnect
                {
                    uint32_t session_id;
                };
            } 
        }


        class CodecV1_0
        {
        public:

            ResponseCode encode_response_protocol_version(const ResponseData::GetInfo::GetProtocolVersion* response_data, Response* response);
            ResponseCode encode_response_software_id(Response* response);
            ResponseCode encode_response_special_memory_region_count(const ResponseData::GetInfo::GetSpecialMemoryRegionCount* response_data, Response* response);
            ResponseCode encode_response_special_memory_region_location(const ResponseData::GetInfo::GetSpecialMemoryRegionLocation* response_data, Response* response);
            ResponseCode encode_response_supported_features(const ResponseData::GetInfo::GetSupportedFeatures* response_data, Response* response);
            ResponseCode encode_response_get_rpv_count(const ResponseData::GetInfo::GetRPVCount* response_data, Response* response);

            
            ResponseCode encode_response_comm_discover(Response* response, const ResponseData::CommControl::Discover* response_data);
            ResponseCode encode_response_comm_heartbeat(const ResponseData::CommControl::Heartbeat* response_data, Response* response);
            ResponseCode encode_response_comm_get_params(const ResponseData::CommControl::GetParams* response_data, Response* response);
            ResponseCode encode_response_comm_connect(const ResponseData::CommControl::Connect* response_data, Response* response);


            ResponseCode decode_request_get_special_memory_region_location(const Request* request, RequestData::GetInfo::GetSpecialMemoryRegionLocation* request_data);
            ResponseCode decode_request_get_rpv_definition(const Request* request, RequestData::GetInfo::GetRPVDefinition* request_data);

            ResponseCode decode_request_comm_discover(const Request* request, RequestData::CommControl::Discover* request_data);
            ResponseCode decode_request_comm_heartbeat(const Request* request, RequestData::CommControl::Heartbeat* request_data);
            ResponseCode decode_request_comm_connect(const Request* request, RequestData::CommControl::Connect* request_data);
            ResponseCode decode_request_comm_disconnect(const Request* request, RequestData::CommControl::Disconnect* request_data);

            ReadMemoryBlocksRequestParser* decode_request_memory_control_read(const Request* request);
            ReadMemoryBlocksResponseEncoder* encode_response_memory_control_read(Response* response, uint32_t max_size);

            WriteMemoryBlocksRequestParser* decode_request_memory_control_write(const Request* request, const bool masked_wirte);
            WriteMemoryBlocksResponseEncoder* encode_response_memory_control_write(Response* response, uint32_t max_size);

            GetRPVDefinitionResponseEncoder* encode_response_get_rpv_definition(Response* response, uint32_t max_size);
            ReadRPVRequestParser* decode_request_memory_control_read_rpv(const Request* request, const RuntimePublishedValue* rpvs, const uint16_t rpv_len);
            ReadRPVResponseEncoder* encode_response_memory_control_read_rpv(Response* response, const uint32_t max_size);

            WriteRPVRequestParser* decode_request_memory_control_write_rpv(const Request* request, const RuntimePublishedValue* rpvs, const uint16_t rpv_len);
            WriteRPVResponseEncoder* encode_response_memory_control_write_rpv(Response* response, const uint32_t max_size);

        protected:
            ReadMemoryBlocksRequestParser m_memory_control_read_request_parser;
            ReadMemoryBlocksResponseEncoder m_memory_control_read_response_encoder;
            WriteMemoryBlocksRequestParser m_memory_control_write_request_parser;
            WriteMemoryBlocksResponseEncoder m_memory_control_write_response_encoder;
            GetRPVDefinitionResponseEncoder m_get_rpv_definition_response_encoder;
            ReadRPVRequestParser m_memory_control_read_rpv_parser;
            ReadRPVResponseEncoder m_read_rpv_response_encoder;
            WriteRPVRequestParser m_memory_control_write_rpv_parser;
            WriteRPVResponseEncoder m_write_rpv_response_encoder;
        };
    }
}
#endif // ___SCRUTINY_CODEC_V1_0___