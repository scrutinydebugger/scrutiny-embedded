//    scrutiny_datalogger_raw_encoder.hpp
//        Class that handles the encoding of the datalogger data. RawFormat just copy to memory,
//        no encoding scheme.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGER_RAW_ENCODER___
#define ___SCRUTINY_DATALOGGER_RAW_ENCODER___

#include <stdint.h>
#include "scrutiny_setup.hpp"
#include "datalogging/scrutiny_datalogging_types.hpp"
#include "scrutiny_timebase.hpp"

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

#if SCRUTINY_DATALOGGING_ENCODING != SCRUTINY_DATALOGGING_ENCODING_RAW
#error "Encoding not supported"
#endif

namespace scrutiny
{
    class MainHandler;
    namespace datalogging
    {
        class RawFormatEncoder;

        class RawFormatReader
        {
        public:
            explicit RawFormatReader(const RawFormatEncoder *encoder) : m_encoder(encoder)
            {
            }
            uint32_t read(uint8_t *buffer, const uint32_t max_size);
            inline bool finished(void) { return m_finished; }
            void reset(void);
            inline bool error(void) const;
            inline uint32_t get_entry_count(void) const;
            uint32_t get_total_size(void) const;
            inline datalogging::EncodingType get_encoding(void) const;

        protected:
            const RawFormatEncoder *const m_encoder;
            uint32_t m_read_cursor = 0;
            bool m_finished = false;
            bool m_read_started = false;
        };

        class RawFormatEncoder
        {
            friend class RawFormatReader;

        public:
            static constexpr EncodingType ENCODING = EncodingType::RAW;
            RawFormatEncoder() : m_reader(this)
            {
            }

            void init(MainHandler *main_handler, Timebase *timebase, datalogging::Configuration *config, uint8_t *buffer, uint32_t buffer_size);
            void encode_next_entry(void);
            void reset(void);
            inline void reset_write_counter(void) { m_entry_write_counter = 0; }
            inline uint32_t get_entry_write_counter(void) const { return m_entry_write_counter; }
            inline uint32_t get_data_write_counter(void) const { return m_entry_write_counter * m_entry_size; }
            inline datalogging::EncodingType get_encoding(void) const { return ENCODING; }
            inline uint32_t get_read_cursor(void) const { return m_first_valid_entry_index * m_entry_size; }
            inline uint32_t get_write_cursor(void) const { return m_next_entry_write_index * m_entry_size; }
            inline bool error(void) const { return m_error; }
            inline uint32_t get_entry_count(void) const { return m_entries_count; }
            inline uint32_t get_buffer_effective_size(void) const { return m_entry_size * m_max_entries; }
            inline bool buffer_full(void) const { return m_full; }
            uint32_t remaining_bytes_to_full() const;

            RawFormatReader *get_reader(void)
            {
                return &m_reader;
            };

        protected:
            uint8_t *m_buffer = nullptr;
            uint32_t m_buffer_size = 0;
            const datalogging::Configuration *m_config = nullptr;
            RawFormatReader m_reader;
            MainHandler *m_main_handler = nullptr;
            Timebase *m_timebase_for_log = nullptr;

            uint32_t m_max_entries = 0;
            uint32_t m_next_entry_write_index = 0;
            uint32_t m_first_valid_entry_index = 0;
            uint32_t m_entry_write_counter = 0;
            uint16_t m_entry_size = 0;
            uint32_t m_entries_count = 0;
            bool m_full = false;
            bool m_error = false;
        };

        uint32_t RawFormatReader::get_entry_count(void) const { return m_encoder->get_entry_count(); }
        inline datalogging::EncodingType RawFormatReader::get_encoding(void) const { return m_encoder->get_encoding(); }
        inline bool RawFormatReader::error(void) const { return m_encoder->error(); }
    }
}

#endif // ___SCRUTINY_DATALOGGER_RAW_ENCODER___