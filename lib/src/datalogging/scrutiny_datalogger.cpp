//    scrutiny_datalogger.cpp
//        The implementation of the datalogger object that should exist in each loop
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include "datalogging/scrutiny_datalogger.hpp"
#include "datalogging/scrutiny_datalogging.hpp"
#include "scrutiny_ipc.hpp"

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

namespace scrutiny
{
    namespace datalogging
    {
        void DataLogger::init(MainHandler *main_handler)
        {
            m_main_handler = main_handler;
            m_configured = false;
            m_trigger.previous_val = false;
            m_trigger.rising_edge_timestamp = 0;
        }

        void DataLogger::configure(datalogging::Configuration *config)
        {
            m_configured = true;
            m_config.copy_from(config);

            switch (m_config.trigger.condition)
            {
            case SupportedTriggerConditions::Equal:
                m_active_trigger_condition_instance = &m_trigger.conditions.eq;
                break;
            case SupportedTriggerConditions::NotEqual:
                m_active_trigger_condition_instance = &m_trigger.conditions.neq;
                break;
            case SupportedTriggerConditions::LessThan:
                m_active_trigger_condition_instance = &m_trigger.conditions.lt;
                break;
            case SupportedTriggerConditions::LessOrEqualThan:
                m_active_trigger_condition_instance = &m_trigger.conditions.let;
                break;
            case SupportedTriggerConditions::GreaterThan:
                m_active_trigger_condition_instance = &m_trigger.conditions.gt;
                break;
            case SupportedTriggerConditions::GreaterOrEqualThan:
                m_active_trigger_condition_instance = &m_trigger.conditions.get;
                break;
            case SupportedTriggerConditions::ChangeMoreThan:
                m_active_trigger_condition_instance = &m_trigger.conditions.cmt;
                break;
            default:
                m_configured = false;
            }

            if (m_configured)
            {
                m_active_trigger_condition_instance->reset(m_trigger.conditions.data());
            }
        }

        bool DataLogger::check_trigger(Timebase *timebase)
        {
            static_assert(MAX_OPERANDS >= 2, "Expect at least 2 operands for relational comparison");
            bool outval = false;
            AnyType opvals[MAX_OPERANDS];
            VariableType optypes[MAX_OPERANDS];
            if (!m_configured)
            {
                return false;
            }

            const unsigned int nb_operand = m_active_trigger_condition_instance->get_operand_count();

            if (nb_operand > MAX_OPERANDS)
            {
                return false;
            }

            for (unsigned int i = 0; i < nb_operand; i++)
            {
                if (fetch_operand(m_main_handler, &m_config.trigger.operands[i], &opvals[i], &optypes[i]) == false)
                {
                    return false;
                }
                convert_to_compare_type(&optypes[i], &opvals[i]);
            }

            bool condition_result = m_active_trigger_condition_instance->evaluate(
                m_trigger.conditions.data(),
                reinterpret_cast<VariableTypeCompare *>(optypes),
                reinterpret_cast<AnyTypeCompare *>(opvals));

            if (condition_result)
            {
                if (m_trigger.previous_val == false)
                {
                    m_trigger.rising_edge_timestamp = timebase->get_timestamp();
                }

                if (timebase->has_expired(m_trigger.rising_edge_timestamp, m_config.trigger.hold_time_us))
                {
                    outval = true;
                }
            }

            m_trigger.previous_val = condition_result;

            return outval;
        }
    }
}