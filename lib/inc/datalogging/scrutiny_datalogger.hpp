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
#include "datalogging/scrutiny_data_encoding.hpp"
#include "scrutiny_timebase.hpp"

namespace scrutiny
{
    class MainHandler;

    namespace datalogging
    {

        class DataLogger
        {
        public:
            enum class State
            {
                IDLE,
                CONFIGURED,
                ARMED,
                ACQUISITION_COMPLETED,
                ERROR
            };
            void init(MainHandler *main_handler, Timebase *timebase, uint8_t *const buffer, const uint32_t buffer_size);
            void configure(Timebase *timebase_for_log, uint16_t config_id = 0);
            void process(void);
            void reset(void);

            inline bool data_acquired(void) const { return m_state == State::ACQUISITION_COMPLETED; }
            inline uint16_t get_acquisition_id(void) const { return m_acquisition_id; }
            inline uint16_t get_config_id(void) const { return m_config_id; }
            inline bool armed(void) const { return m_state == State::ARMED; }
            inline DataLogger::State get_state(void) const { return m_state; }
            void arm_trigger(void);
            void disarm_trigger(void);

            bool check_trigger(void);
            inline DataReader *get_reader(void) { return m_encoder.get_reader(); }
            inline DataEncoder *get_encoder(void) { return &m_encoder; }
            inline Configuration *config(void) { return &m_config; }
            inline bool in_error(void) const { return m_state == State::ERROR; }
            inline bool config_valid(void) const { return m_config_valid; }
            void force_trigger(void)
            {
                if (m_state == State::ARMED)
                {
                    m_manual_trigger = true;
                }
            }

        protected:
            void process_acquisition(void);
            void stamp_trigger_point(void);
            bool acquisition_completed(void);
            void write_uncompressed_entry(void);
            uint16_t read_next_entry_size(uint32_t *cursor);
            void write_diff_bits(uint8_t *new_entry, uint8_t *previous_entry);

            MainHandler *m_main_handler;
            uint32_t m_buffer_size;

            Timebase *m_timebase;
            Timebase *m_timebase_for_log;
            State m_state;
            bool m_trigger_point_stamped;
            uint32_t m_trigger_timestamp;
            uint32_t m_trigger_cursor_location;
            uint32_t m_remaining_data_to_write;
            bool m_manual_trigger;

            Configuration m_config;
            bool m_config_valid;
            DataEncoder m_encoder;
            uint16_t m_decimation_counter;
            uint16_t m_acquisition_id;
            uint16_t m_config_id;
            uint32_t m_log_points_after_trigger;

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