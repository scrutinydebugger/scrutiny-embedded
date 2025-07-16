//    scrutiny_codec_v1_0.hpp
//        Definitions of encode/decode functions for the scrutiny protocol V1.0
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_CODEC_V1_0___
#define ___SCRUTINY_CODEC_V1_0___

#include <stdint.h>

#include "scrutiny_protocol_definitions.hpp"
#include "scrutiny_setup.hpp"
#include "scrutiny_software_id.hpp"
#include "scrutiny_types.hpp"

#if SCRUTINY_ENABLE_DATALOGGING
#include "datalogging/scrutiny_datalogging_data_encoding.hpp"
#include "datalogging/scrutiny_datalogging_types.hpp"
#endif

namespace scrutiny
{
    class MainHandler;

    namespace protocol
    {
        SCRUTINY_CONSTEXPR unsigned int REQUEST_OVERHEAD = 8;         // Number of bytes in a request that are not part of the payload
        SCRUTINY_CONSTEXPR unsigned int RESPONSE_OVERHEAD = 9;        // Number of bytes in a response that are not part of the payload
        SCRUTINY_CONSTEXPR unsigned int MAX_DISPLAY_NAME_LENGTH = 64; // Maximum length of a display name given on discover
        SCRUTINY_CONSTEXPR unsigned int MAX_LOOP_NAME_LENGTH = 32;    // Maximum length given to a loop
        SCRUTINY_CONSTEXPR unsigned int MINIMUM_RX_BUFFER_SIZE = 32;  // Minimum size of the reception buffer
        SCRUTINY_CONSTEXPR unsigned int MINIMUM_TX_BUFFER_SIZE = 32;  // Minimum size of the transmit buffer
        SCRUTINY_CONSTEXPR uint16_t BUFFER_OVERFLOW_MARGIN = 16;      // This margin let us detect overflow in CommHandler with very few calculations.
        SCRUTINY_CONSTEXPR unsigned int MAXIMUM_RX_BUFFER_SIZE = 0xFFFF - BUFFER_OVERFLOW_MARGIN; // Maximum reception buffer size in bytes
        SCRUTINY_CONSTEXPR unsigned int MAXIMUM_TX_BUFFER_SIZE = 0xFFFF - BUFFER_OVERFLOW_MARGIN; // Maximum transmission buffer size in bytes

        class ReadMemoryBlocksRequestParser
        {
          public:
            void init(Request const *const request);
            void next(MemoryBlock *const memblock);
            inline bool finished(void) const { return m_finished; };
            inline bool is_valid(void) const { return !m_invalid; };
            inline uint16_t required_tx_buffer_size(void) const { return m_required_tx_buffer_size; }
            void reset(void);

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
            void init(Response *const response, uint16_t const max_size);
            void write(MemoryBlock const *const memblock);
            inline bool overflow(void) const { return m_overflow; };
            void reset(void);

          protected:
            uint8_t *m_buffer;
            Response *m_response;
            uint16_t m_cursor;
            uint16_t m_size_limit;
            bool m_overflow;
        };

        class TypedReadRequestParser
        {
          public:
            void init(Request const *const request);
            void next(TypedMemoryRegion *const mem_region);
            inline bool finished(void) const { return m_finished; };
            inline bool is_valid(void) const { return !m_invalid; };
            inline uint16_t required_tx_buffer_size(void) const { return m_required_tx_buffer_size; }
            void reset(void);

          protected:
            void validate();
            uint8_t *m_buffer;
            uint16_t m_bytes_read;
            uint16_t m_request_datasize;
            uint16_t m_required_tx_buffer_size;
            bool m_finished;
            bool m_invalid;
        };

        class TypedReadResponseEncoder
        {
          public:
            void init(Response *const response, uint16_t const max_size);
            void write(TypedMemoryRegion const *const typedmem);
            inline bool overflow(void) const { return m_overflow; };
            void reset(void);

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
            void init(Request const *const request, bool const masked_write);
            void next(MemoryBlock *const memblock);
            inline bool finished(void) const { return m_finished; };
            inline bool is_valid(void) const { return !m_invalid; };
            inline uint32_t required_tx_buffer_size(void) const { return m_required_tx_buffer_size; }
            void reset(void);

          protected:
            void validate(void);

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
            void init(Response *const response, uint16_t const max_size);
            void write(MemoryBlock const *const memblock);
            inline bool overflow(void) const { return m_overflow; };
            void reset(void);

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
            void init(Response *const response, uint16_t const max_size);
            void write(RuntimePublishedValue const *const rpv);
            inline bool overflow(void) const { return m_overflow; };
            void reset(void);

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
            void init(Response *const response, uint16_t const max_size);
            void write(RuntimePublishedValue const *const rpv, AnyType const v);
            inline bool overflow(void) const { return m_overflow; };
            void reset(void);

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
            void init(Request const *const request);
            bool next(uint16_t *const id);
            inline bool finished(void) const { return m_finished; };
            inline bool is_valid(void) const { return !m_invalid; };
            void reset(void);

          protected:
            void validate(void);

            uint8_t *m_buffer;
            uint16_t m_bytes_read;
            uint16_t m_request_len;
            bool m_finished;
            bool m_invalid;
        };

        class WriteRPVResponseEncoder
        {
          public:
            void init(Response *const response, uint16_t const max_size);
            void write(RuntimePublishedValue const *const rpv);
            inline bool overflow(void) const { return m_overflow; };
            void reset(void);

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
            void init(Request const *const request, MainHandler const *const main_handler);
            bool next(RuntimePublishedValue *const rpv, AnyType *const v);
            inline bool finished(void) const { return m_finished; };
            inline bool is_valid(void) const { return !m_invalid; };
            void reset(void);

          protected:
            uint8_t *m_buffer;
            uint16_t m_bytes_read;
            uint16_t m_request_len;
            bool m_finished;
            bool m_invalid;
            MainHandler const *m_main_handler;
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
                    char const *loop_name;
                };
            } // namespace GetInfo

            namespace CommControl
            {
                struct Discover
                {
                    uint8_t display_name_length;
                    char const *display_name;
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
            } // namespace CommControl

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
            } // namespace DataLogControl

#endif
        } // namespace ResponseData

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
            } // namespace GetInfo

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
            } // namespace CommControl

#if SCRUTINY_ENABLE_DATALOGGING

            namespace DataLogControl
            {
                struct Configure
                {
                    uint8_t loop_id;
                    uint16_t config_id;
                    // Rest is directly written to datalogger config. So not in this struct.
                };
            } // namespace DataLogControl
#endif
        } // namespace RequestData

        class CodecV1_0
        {
          public:
            ResponseCode::eResponseCode encode_response_protocol_version(
                ResponseData::GetInfo::GetProtocolVersion const *const response_data,
                Response *const response);
            ResponseCode::eResponseCode encode_response_software_id(Response *const response);
            ResponseCode::eResponseCode encode_response_special_memory_region_count(
                ResponseData::GetInfo::GetSpecialMemoryRegionCount const *const response_data,
                Response *const response);
            ResponseCode::eResponseCode encode_response_special_memory_region_location(
                ResponseData::GetInfo::GetSpecialMemoryRegionLocation const *const response_data,
                Response *const response);
            ResponseCode::eResponseCode encode_response_supported_features(
                ResponseData::GetInfo::GetSupportedFeatures const *const response_data,
                Response *const response);
            ResponseCode::eResponseCode encode_response_get_rpv_count(
                ResponseData::GetInfo::GetRPVCount const *const response_data,
                Response *const response);
            ResponseCode::eResponseCode encode_response_get_loop_count(
                ResponseData::GetInfo::GetLoopCount const *const response_data,
                Response *const response);
            ResponseCode::eResponseCode encode_response_get_loop_definition(
                ResponseData::GetInfo::GetLoopDefinition const *const response_data,
                Response *const response);

            ResponseCode::eResponseCode encode_response_comm_discover(
                ResponseData::CommControl::Discover const *const response_data,
                Response *const response);
            ResponseCode::eResponseCode encode_response_comm_heartbeat(
                ResponseData::CommControl::Heartbeat const *const response_data,
                Response *const response);
            ResponseCode::eResponseCode encode_response_comm_get_params(
                ResponseData::CommControl::GetParams const *const response_data,
                Response *const response);
            ResponseCode::eResponseCode encode_response_comm_connect(
                ResponseData::CommControl::Connect const *const response_data,
                Response *const response);

            ResponseCode::eResponseCode decode_request_get_special_memory_region_location(
                Request const *const request,
                RequestData::GetInfo::GetSpecialMemoryRegionLocation *const request_data);
            ResponseCode::eResponseCode decode_request_get_rpv_definition(
                Request const *const request,
                RequestData::GetInfo::GetRPVDefinition *const request_data);
            ResponseCode::eResponseCode decode_request_get_loop_definition(
                Request const *const request,
                RequestData::GetInfo::GetLoopDefinition *const request_data);

            ResponseCode::eResponseCode decode_request_comm_discover(
                Request const *const request,
                RequestData::CommControl::Discover *const request_data);
            ResponseCode::eResponseCode decode_request_comm_heartbeat(
                Request const *const request,
                RequestData::CommControl::Heartbeat *const request_data);
            ResponseCode::eResponseCode decode_request_comm_connect(
                Request const *const request,
                RequestData::CommControl::Connect *const request_data);
            ResponseCode::eResponseCode decode_request_comm_disconnect(
                Request const *const request,
                RequestData::CommControl::Disconnect *const request_data);

            ReadMemoryBlocksRequestParser *decode_request_memory_control_read(Request const *const request);
            ReadMemoryBlocksResponseEncoder *encode_response_memory_control_read(Response *const response, uint16_t const max_size);

            TypedReadRequestParser *decode_request_memory_control_typed_read(Request const *const request);
            TypedReadResponseEncoder *encode_response_memory_control_typed_read(Response *const response, uint16_t const max_size);

            WriteMemoryBlocksRequestParser *decode_request_memory_control_write(Request const *const request, bool const masked_wirte);
            WriteMemoryBlocksResponseEncoder *encode_response_memory_control_write(Response *const response, uint16_t const max_size);

            GetRPVDefinitionResponseEncoder *encode_response_get_rpv_definition(Response *const response, uint16_t const max_size);
            ReadRPVRequestParser *decode_request_memory_control_read_rpv(Request const *const request);
            ReadRPVResponseEncoder *encode_response_memory_control_read_rpv(Response *const response, uint16_t const max_size);

            WriteRPVRequestParser *decode_request_memory_control_write_rpv(Request const *const request, MainHandler *main_handler);
            WriteRPVResponseEncoder *encode_response_memory_control_write_rpv(Response *const response, uint16_t const max_size);

#if SCRUTINY_ENABLE_DATALOGGING
            ResponseCode::eResponseCode encode_response_datalogging_get_setup(
                ResponseData::DataLogControl::GetSetup const *const response_data,
                Response *const response);
            ResponseCode::eResponseCode encode_response_datalogging_status(
                ResponseData::DataLogControl::GetStatus const *const response_data,
                Response *const response);
            ResponseCode::eResponseCode encode_response_datalogging_get_acquisition_metadata(
                ResponseData::DataLogControl::GetAcquisitionMetadata const *const response_data,
                Response *const response);
            ResponseCode::eResponseCode encode_response_datalogging_read_acquisition(
                ResponseData::DataLogControl::ReadAcquisition const *const response_data,
                Response *const response,
                bool *const finished);
            ResponseCode::eResponseCode decode_datalogging_configure_request(
                Request const *const request,
                RequestData::DataLogControl::Configure *const request_data,
                datalogging::Configuration *const config);
#endif

          protected:
            union
            {
                ReadMemoryBlocksRequestParser m_memory_control_read_request_parser;
                TypedReadRequestParser m_memory_control_typed_read_request_parser;
                WriteMemoryBlocksRequestParser m_memory_control_write_request_parser;
                ReadRPVRequestParser m_memory_control_read_rpv_parser;
                WriteRPVRequestParser m_memory_control_write_rpv_parser;
            } parsers;

            union
            {
                ReadMemoryBlocksResponseEncoder m_memory_control_read_response_encoder;
                TypedReadResponseEncoder m_memory_control_typed_read_response_encoder;
                WriteMemoryBlocksResponseEncoder m_memory_control_write_response_encoder;
                GetRPVDefinitionResponseEncoder m_get_rpv_definition_response_encoder;
                ReadRPVResponseEncoder m_read_rpv_response_encoder;
                WriteRPVResponseEncoder m_write_rpv_response_encoder;
            } encoders;
        };
    } // namespace protocol
} // namespace scrutiny
#endif // ___SCRUTINY_CODEC_V1_0___
