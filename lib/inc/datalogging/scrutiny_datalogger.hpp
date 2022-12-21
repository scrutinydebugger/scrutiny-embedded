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

namespace scrutiny
{
    namespace datalogging
    {
        class DataLogger
        {
        public:
            void init(MainHandler *main_handler);
            void configure(datalogging::Configuration *config);
            bool check_trigger(Timebase *timebase);

        protected:
            uint8_t *m_buffer;
            MainHandler *m_main_handler;

            trigger::BaseCondition *m_active_trigger_condition_instance;
            bool m_configured;
            Configuration m_config;
            struct
            {
                bool previous_val;
                uint32_t rising_edge_timestamp;
                trigger::ConditionSet conditions;
            } m_trigger;
        };
    }
}
#endif // ___SCRUTINY_DATALOGGER_H___