//    scrutiny_datalogger.hpp
//        The definition of the datalogger object that should exist in each loop
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGER_H___
#define ___SCRUTINY_DATALOGGER_H___

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

#include "datalogging/scrutiny_datalogging_types.hpp"
#include "datalogging/scrutiny_datalogging_trigger.hpp"
#include "scrutiny_main_handler.hpp"

#if SCRUTINY_DATALOGGING_ENCODING == SCRUTINY_DATALOGGING_ENCODING_RAW
#include "datalogging/scrutiny_datalogger_raw_encoder.hpp"
namespace scrutiny
{
    namespace datalogging
    {

        using DataEncoder = RawFormatEncoder;
        using DataReader = RawFormatReader;
    }
}
#endif

namespace scrutiny
{
    namespace datalogging
    {

        class DataLogger
        {
            enum class State
            {
                IDLE,
                CONFIGURED,
                ARMED,
                ACQUISITION_COMPLETED,
                ERROR
            };

        public:
            DataLogger(uint8_t *const buffer, const uint32_t buffer_size) : m_buffer(buffer),
                                                                            m_buffer_size(buffer_size),
                                                                            m_encoder(buffer, buffer_size, &m_config)

            {
            }
            void init(MainHandler *main_handler, Timebase *timebase);
            void configure(datalogging::Configuration *config);
            void process(void);
            void reset(void);

            inline bool data_acquired(void) const { return m_state == State::ACQUISITION_COMPLETED; }
            void arm_trigger(void);

            bool check_trigger(void);

        protected:
            void process_acquisition(void);
            void stamp_trigger_point(void);
            bool acquisition_completed(void);
            void write_uncompressed_entry(void);
            uint16_t read_next_entry_size(uint32_t *cursor);
            void write_diff_bits(uint8_t *new_entry, uint8_t *previous_entry);

            uint8_t *const m_buffer;
            const uint32_t m_buffer_size;

            MainHandler *m_main_handler;
            Timebase *m_timebase;
            State m_state;
            bool m_trigger_point_stamped;
            uint32_t m_trigger_timestamp;
            uint32_t m_trigger_cursor_location;
            uint32_t m_remaining_data_to_write;

            Configuration m_config;
            DataEncoder m_encoder;

            struct
            {
                bool previous_val;
                uint32_t rising_edge_timestamp;
                trigger::ConditionSet conditions;
                trigger::BaseCondition *active_condition;
            } m_trigger;
        };
    }
}
#endif // ___SCRUTINY_DATALOGGER_H___