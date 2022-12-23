#ifndef ___SCRUTINY_DATALOGGER_ENCODER___
#define ___SCRUTINY_DATALOGGER_ENCODER___

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

#if SCRUTINY_DATALOGGING_ENCODING != SCRUTINY_DATALOGGING_ENCODING_RAW
#error "Encoding not supported"
#endif

#include "stdint.h"
#include "datalogging/scrutiny_datalogging_types.hpp"

namespace scrutiny
{
    namespace datalogging
    {
        class RawFormatEncoder;

        class RawFormatReader
        {
        public:
            RawFormatReader(const RawFormatEncoder *encoder) : m_encoder(encoder)
            {
            }
            uint32_t read(uint8_t *buffer, const uint32_t max_size);
            void reset(void);

        protected:
            const RawFormatEncoder *const m_encoder;
            uint32_t m_read_cursor;
        };

        class RawFormatEncoder
        {
            friend class RawFormatReader;

        public:
            static constexpr EncodingType ENCODING = EncodingType::RAW;
            RawFormatEncoder(uint8_t *buffer,
                             uint32_t buffer_size,
                             datalogging::Configuration *config) : m_buffer(buffer),
                                                                   m_buffer_size(buffer_size),
                                                                   m_config(config),
                                                                   m_reader(this)
            {
            }

            void init();
            void encode_next_entry(void);
            inline void reset_write_counter(void) { m_write_counter = 0; }
            inline uint32_t get_write_counter(void) const { return m_write_counter; }
            inline datalogging::EncodingType get_encoding(void) const { return ENCODING; }
            inline uint32_t get_read_cursor(void) const { return m_first_valid_entry_index * m_blocksize_sum; }
            inline uint32_t get_write_cursor(void) const { return m_next_entry_write_index * m_blocksize_sum; }

            RawFormatReader *get_reader(void)
            {
                m_reader.reset();
                return &m_reader;
            };

        protected:
            uint8_t *const m_buffer;
            const uint32_t m_buffer_size;
            const datalogging::Configuration *m_config;
            RawFormatReader m_reader;

            uint32_t m_max_entries;
            uint32_t m_next_entry_write_index;
            uint32_t m_first_valid_entry_index;
            uint32_t m_write_counter;
            uint16_t m_blocksize_sum;
            uint32_t m_entries_count;
            bool m_full;
        };
    }
}

#endif // ___SCRUTINY_DATALOGGER_ENCODER___