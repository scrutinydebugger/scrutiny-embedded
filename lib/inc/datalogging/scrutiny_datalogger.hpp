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
            static constexpr unsigned int MAX_OPERANDS = 2;

            void configure();

        protected:
            bool check_trigger();
            uint8_t *m_buffer;
            MainHandler *m_main_handler;
            Operand m_operands[MAX_OPERANDS];
            trigger::BaseCondition *m_active_condition;
            bool m_configured;
            
        };
    }
}
#endif // ___SCRUTINY_DATALOGGER_H___