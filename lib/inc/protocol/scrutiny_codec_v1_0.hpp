//    scrutiny_codec_v1_0.hpp
//        Definitions of encode/decode functions for the scrutiny protocol V1.0
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#ifndef ___SCRUTINY_CODEC_V1_0___
#define ___SCRUTINY_CODEC_V1_0___

#include <stdint.h>

#include "scrutiny_setup.hpp"
#include "scrutiny_protocol_definitions.hpp"
#include "scrutiny_software_id.hpp"
#include "scrutiny_types.hpp"

#if SCRUTINY_ENABLE_DATALOGGING
#include "datalogging/scrutiny_datalogging_types.hpp"
#include "datalogging/scrutiny_datalogging_data_encoding.hpp"
#endif

namespace scrutiny
{
    class MainHandler;

    namespace protocol
    {
        constexpr unsigned int REQUEST_OVERHEAD = 8;                                     // Number of bytes in a request that are not part of the payload
        constexpr unsigned int RESPONSE_OVERHEAD = 9;                                    // Number of bytes in a response that are not part of the payload
        constexpr unsigned int MAX_DISPLAY_NAME_LENGTH = 64;                             // Maximum length of a display name given on discover
        constexpr unsigned int MAX_LOOP_NAME_LENGTH = 32;                                // Maximum length given to a loop
        constexpr unsigned int MINIMUM_RX_BUFFER_SIZE = 32;                              // Minimum size of the reception buffer
        constexpr unsigned int MINIMUM_TX_BUFFER_SIZE = 32;                              // Minimum size of the transmit buffer
        constexpr uint16_t BUFFER_OVERFLOW_MARGIN = 16;                                  // This margin let us detect overflow in CommHandler with very few calculations.
        constexpr unsigned int MAXIMUM_RX_BUFFER_SIZE = 0xFFFF - BUFFER_OVERFLOW_MARGIN; // Maximum reception buffer size in bytes
        constexpr unsigned int MAXIMUM_TX_BUFFER_SIZE = 0xFFFF - BUFFER_OVERFLOW_MARGIN; // Maximum transmission buffer size in bytes

        class ReadMemoryBlocksRequestParser
        {
        public:
            void init(const Request *request);
            void next(MemoryBlock *memblock);
            inline bool finished() { return m_finished; };
            inline bool is_valid() { return !m_invalid; };
            inline uint16_t required_tx_buffer_size() { return m_required_tx_buffer_size; }
            void reset();

        protected:
            void validate();
            uint8_t *m_buffer;
            uint16_t m_bytes_read;
            uint16_t m_request_datasize;
            uint16_t m_required_tx_buffer_size;
            bool m_finished;
            bool m_invalid;
        };

        class ReadMemoryBlocksResponseEncoder
        {
        public:
            void init(Response *response, const uint16_t max_size);
            void write(MemoryBlock *memblock);
            inline bool overflow() { return m_overflow; };
            void reset();

        protected:
            uint8_t *m_buffer;
            Response *m_response;
            uint16_t m_cursor;
            uint16_t m_size_limit;
            bool m_overflow;
        };

        class WriteMemoryBlocksRequestParser
        {
        public:
            void init(const Request *request, bool masked_write);
            void next(MemoryBlock *memblock);
            inline bool finished() { return m_finished; };
            inline bool is_valid() { return !m_invalid; };
            inline uint32_t required_tx_buffer_size() { return m_required_tx_buffer_size; }
            void reset();

        protected:
            void validate();

            uint8_t *m_buffer;
            uint16_t m_bytes_read;
            uint16_t m_size_limit;
            uint16_t m_required_tx_buffer_size;
            bool m_finished;
            bool m_invalid;
            bool m_masked_write;
        };

        class WriteMemoryBlocksResponseEncoder
        {
        public:
            void init(Response *response, uint16_t max_size);
            void write(MemoryBlock *memblock);
            inline bool overflow() { return m_overflow; };
            void reset();

        protected:
            uint8_t *m_buffer;
            Response *m_response;
            uint16_t m_cursor;
            uint16_t m_size_limit;
            bool m_overflow;
        };

        class GetRPVDefinitionResponseEncoder
        {
        public:
            void init(Response *response, const uint16_t max_size);
            void write(const RuntimePublishedValue *rpv);
            inline bool overflow() { return m_overflow; };
            void reset();

        protected:
            uint8_t *m_buffer;
            Response *m_response;
            uint16_t m_cursor;
            uint16_t m_size_limit;
            bool m_overflow;
        };

        class ReadRPVResponseEncoder
        {
        public:
            void init(Response *response, const uint16_t max_size);
            void write(const RuntimePublishedValue *rpv, AnyType v);
            inline bool overflow() { return m_overflow; };
            void reset();

        protected:
            uint8_t *m_buffer;
            Response *m_response;
            uint16_t m_cursor;
            uint16_t m_size_limit;
            bool m_overflow;
        };

        class ReadRPVRequestParser
        {
        public:
            void init(const Request *request);
            bool next(uint16_t *id);
            inline bool finished() { return m_finished; };
            inline bool is_valid() { return !m_invalid; };
            void reset();

        protected:
            void validate();

            uint8_t *m_buffer;
            uint16_t m_bytes_read;
            uint16_t m_request_len;
            bool m_finished;
            bool m_invalid;
        };

        class WriteRPVResponseEncoder
        {
        public:
            void init(Response *response, const uint16_t max_size);
            void write(const RuntimePublishedValue *rpv);
            inline bool overflow() { return m_overflow; };
            void reset();

        protected:
            uint8_t *m_buffer;
            Response *m_response;
            uint16_t m_cursor;
            uint16_t m_size_limit;
            bool m_overflow;
        };

        class WriteRPVRequestParser
        {
        public:
            void init(const Request *request, MainHandler *main_handler);
            bool next(RuntimePublishedValue *rpv, AnyType *v);
            inline bool finished() { return m_finished; };
            inline bool is_valid() { return !m_invalid; };
            void reset();

        protected:
            uint8_t *m_buffer;
            uint16_t m_bytes_read;
            uint16_t m_request_len;
            bool m_finished;
            bool m_invalid;
            MainHandler *m_main_handler;
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
                    bool memory_write;
                    bool datalogging;
                    bool user_command;
                    bool _64bits;
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
                    uintptr_t start;
                    uintptr_t end;
                };

                struct GetRPVCount
                {
                    uint16_t count;
                };

                struct GetLoopCount
                {
                    uint8_t count;
                };

                struct GetLoopDefinition
                {
                    uint8_t loop_id;
                    uint8_t loop_type;
                    bool support_datalogging;
                    union
                    {
                        struct
                        {
                            uint32_t timestep_100ns;
                        } fixed_freq;
                    } loop_type_specific;

                    uint8_t loop_name_length;
                    const char *loop_name;
                };
            }

            namespace CommControl
            {
                struct Discover
                {
                    uint8_t display_name_length;
                    const char *display_name;
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

#if SCRUTINY_ENABLE_DATALOGGING
            namespace DataLogControl
            {
                struct GetSetup
                {
                    uint32_t buffer_size;
                    uint8_t data_encoding;
                    uint8_t max_signal_count;
                };

                struct GetStatus
                {
                    uint8_t state;
                    uint32_t bytes_to_acquire_from_trigger_to_completion;
                    uint32_t write_counter_since_trigger;
                };

                struct GetAcquisitionMetadata
                {
                    uint16_t acquisition_id;
                    uint16_t config_id;
                    uint32_t number_of_points;
                    uint32_t data_size;
                    uint32_t points_after_trigger;
                };

                struct ReadAcquisition
                {
                    uint16_t acquisition_id;
                    uint8_t rolling_counter;
                    datalogging::DataReader *reader;
                    uint32_t *crc;
                };
            }

#endif
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

                struct GetLoopCount
                {
                    uint8_t loop_count;
                };

                struct GetLoopDefinition
                {
                    uint8_t loop_id;
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

#if SCRUTINY_ENABLE_DATALOGGING

            namespace DataLogControl
            {
                struct Configure
                {
                    uint8_t loop_id;
                    uint16_t config_id;
                    // Rest is directly written to datalogger config. So not in this struct.
                };
            }
#endif
        }

        class CodecV1_0
        {
        public:
            ResponseCode encode_response_protocol_version(const ResponseData::GetInfo::GetProtocolVersion *response_data, Response *response);
            ResponseCode encode_response_software_id(Response *response);
            ResponseCode encode_response_special_memory_region_count(const ResponseData::GetInfo::GetSpecialMemoryRegionCount *response_data, Response *response);
            ResponseCode encode_response_special_memory_region_location(const ResponseData::GetInfo::GetSpecialMemoryRegionLocation *response_data, Response *response);
            ResponseCode encode_response_supported_features(const ResponseData::GetInfo::GetSupportedFeatures *response_data, Response *response);
            ResponseCode encode_response_get_rpv_count(const ResponseData::GetInfo::GetRPVCount *response_data, Response *response);
            ResponseCode encode_response_get_loop_count(const ResponseData::GetInfo::GetLoopCount *response_data, Response *response);
            ResponseCode encode_response_get_loop_definition(const ResponseData::GetInfo::GetLoopDefinition *response_data, Response *response);

            ResponseCode encode_response_comm_discover(Response *response, const ResponseData::CommControl::Discover *response_data);
            ResponseCode encode_response_comm_heartbeat(const ResponseData::CommControl::Heartbeat *response_data, Response *response);
            ResponseCode encode_response_comm_get_params(const ResponseData::CommControl::GetParams *response_data, Response *response);
            ResponseCode encode_response_comm_connect(const ResponseData::CommControl::Connect *response_data, Response *response);

            ResponseCode decode_request_get_special_memory_region_location(const Request *request, RequestData::GetInfo::GetSpecialMemoryRegionLocation *request_data);
            ResponseCode decode_request_get_rpv_definition(const Request *request, RequestData::GetInfo::GetRPVDefinition *request_data);
            ResponseCode decode_request_get_loop_definition(const Request *request, RequestData::GetInfo::GetLoopDefinition *request_data);

            ResponseCode decode_request_comm_discover(const Request *request, RequestData::CommControl::Discover *request_data);
            ResponseCode decode_request_comm_heartbeat(const Request *request, RequestData::CommControl::Heartbeat *request_data);
            ResponseCode decode_request_comm_connect(const Request *request, RequestData::CommControl::Connect *request_data);
            ResponseCode decode_request_comm_disconnect(const Request *request, RequestData::CommControl::Disconnect *request_data);

            ReadMemoryBlocksRequestParser *decode_request_memory_control_read(const Request *request);
            ReadMemoryBlocksResponseEncoder *encode_response_memory_control_read(Response *response, uint16_t max_size);

            WriteMemoryBlocksRequestParser *decode_request_memory_control_write(const Request *request, const bool masked_wirte);
            WriteMemoryBlocksResponseEncoder *encode_response_memory_control_write(Response *response, uint16_t max_size);

            GetRPVDefinitionResponseEncoder *encode_response_get_rpv_definition(Response *response, uint16_t max_size);
            ReadRPVRequestParser *decode_request_memory_control_read_rpv(const Request *request);
            ReadRPVResponseEncoder *encode_response_memory_control_read_rpv(Response *response, const uint16_t max_size);

            WriteRPVRequestParser *decode_request_memory_control_write_rpv(const Request *request, MainHandler *main_handler);
            WriteRPVResponseEncoder *encode_response_memory_control_write_rpv(Response *response, const uint16_t max_size);

#if SCRUTINY_ENABLE_DATALOGGING
            ResponseCode encode_response_datalogging_get_setup(const ResponseData::DataLogControl::GetSetup *response_data, Response *response);
            ResponseCode encode_response_datalogging_status(const ResponseData::DataLogControl::GetStatus *response_data, Response *response);
            ResponseCode encode_response_datalogging_get_acquisition_metadata(const ResponseData::DataLogControl::GetAcquisitionMetadata *response_data, Response *response);
            ResponseCode encode_response_datalogging_read_acquisition(const ResponseData::DataLogControl::ReadAcquisition *response_data, Response *response, bool *finished);
            ResponseCode decode_datalogging_configure_request(
                const Request *request,
                RequestData::DataLogControl::Configure *request_data,
                datalogging::Configuration *config);
#endif

        protected:
            union
            {
                ReadMemoryBlocksRequestParser m_memory_control_read_request_parser;
                WriteMemoryBlocksRequestParser m_memory_control_write_request_parser;
                ReadRPVRequestParser m_memory_control_read_rpv_parser;
                WriteRPVRequestParser m_memory_control_write_rpv_parser;
            } parsers;

            union
            {
                ReadMemoryBlocksResponseEncoder m_memory_control_read_response_encoder;
                WriteMemoryBlocksResponseEncoder m_memory_control_write_response_encoder;
                GetRPVDefinitionResponseEncoder m_get_rpv_definition_response_encoder;
                ReadRPVResponseEncoder m_read_rpv_response_encoder;
                WriteRPVResponseEncoder m_write_rpv_response_encoder;
            } encoders;
        };
    }
}
#endif // ___SCRUTINY_CODEC_V1_0___