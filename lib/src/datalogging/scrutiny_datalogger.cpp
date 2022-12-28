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
        void DataLogger::init(MainHandler *main_handler, Timebase *timebase, uint8_t *const buffer, const uint32_t buffer_size)
        {
            m_timebase = timebase;
            m_main_handler = main_handler;
            m_timebase = timebase;
            m_buffer = buffer;
            m_buffer_size = buffer_size;

            m_encoder.init(main_handler, timebase, &m_config, buffer, buffer_size);

            reset();
        }

        void DataLogger::reset(void)
        {

            m_state = State::IDLE;
            m_trigger.previous_val = false;
            m_trigger.rising_edge_timestamp = 0;
            m_trigger_point_stamped = false;

            m_trigger_cursor_location = 0;
            m_trigger_timestamp = 0;
            m_remaining_data_to_write = 0;
        }

        void DataLogger::configure(datalogging::Configuration *config)
        {
            if (m_state != State::IDLE)
            {
                reset();
            }

            m_trigger.previous_val = false;
            m_trigger.rising_edge_timestamp = 0;
            m_decimation_counter = 0;

            m_config.copy_from(config);

            if (m_config.items_count > SCRUTINY_DATALOGGING_MAX_SIGNAL || m_config.items_count == 0)
            {
                m_state = State::ERROR;
                return;
            }

            switch (m_config.trigger.condition)
            {
            case SupportedTriggerConditions::Equal:
                m_trigger.active_condition = &m_trigger.conditions.eq;
                break;
            case SupportedTriggerConditions::NotEqual:
                m_trigger.active_condition = &m_trigger.conditions.neq;
                break;
            case SupportedTriggerConditions::LessThan:
                m_trigger.active_condition = &m_trigger.conditions.lt;
                break;
            case SupportedTriggerConditions::LessOrEqualThan:
                m_trigger.active_condition = &m_trigger.conditions.let;
                break;
            case SupportedTriggerConditions::GreaterThan:
                m_trigger.active_condition = &m_trigger.conditions.gt;
                break;
            case SupportedTriggerConditions::GreaterOrEqualThan:
                m_trigger.active_condition = &m_trigger.conditions.get;
                break;
            case SupportedTriggerConditions::ChangeMoreThan:
                m_trigger.active_condition = &m_trigger.conditions.cmt;
                break;
            default:
                m_state = State::ERROR;
            }

            if (m_state != State::ERROR)
            {
                m_trigger.active_condition->reset(m_trigger.conditions.data());
                m_encoder.reset();
                m_state = State::CONFIGURED;
            }
        }

        void DataLogger::arm_trigger(void)
        {
            if (m_state == State::CONFIGURED)
            {
                m_state = State::ARMED;
            }
        }

        void DataLogger::process(void)
        {

            switch (m_state)
            {
            case State::IDLE:
            case State::ERROR:
            case State::ACQUISITION_COMPLETED:
                break;
            case State::CONFIGURED:
            case State::ARMED:
                if (m_encoder.error())
                {
                    m_state = State::ERROR;
                }
                else
                {
                    process_acquisition();
                    if (m_state == State::ARMED)
                    {
                        if (check_trigger())
                        {
                            if (!m_trigger_point_stamped)
                            {
                                stamp_trigger_point();
                            }
                        }

                        if (acquisition_completed())
                        {
                            m_state = State::ACQUISITION_COMPLETED;
                        }
                    }
                    break;
                }
            }
        }

        void DataLogger::stamp_trigger_point(void)
        {
            m_trigger_point_stamped = true;
            m_trigger_cursor_location = m_encoder.get_write_cursor();
            m_trigger_timestamp = m_timebase->get_timestamp();
            m_encoder.reset_write_counter();

            const uint64_t multiplier = static_cast<uint64_t>((1 << (sizeof(m_config.probe_location) * 8)) - 1 - m_config.probe_location);
            m_remaining_data_to_write = static_cast<uint32_t>((static_cast<uint64_t>(m_buffer_size) * multiplier) >> (sizeof(m_config.probe_location) * 8));

            if (m_remaining_data_to_write > m_buffer_size)
            {
                m_remaining_data_to_write = m_buffer_size;
            }
        }

        bool DataLogger::acquisition_completed(void)
        {
            if (m_trigger_point_stamped)
            {
                if (m_config.timeout_us > 0)
                {
                    if (m_timebase->has_expired(m_trigger_timestamp, m_config.timeout_us))
                    {
                        return true;
                    }
                }

                if (m_encoder.get_write_counter() >= m_remaining_data_to_write)
                {
                    return true;
                }
            }

            return false;
        }

        void DataLogger::process_acquisition(void)
        {
            // todo : Handle stop
            if (++m_decimation_counter >= m_config.decimation)
            {
                m_encoder.encode_next_entry();
                m_decimation_counter = 0;
            }
        }

        bool DataLogger::check_trigger(void)
        {
            static_assert(MAX_OPERANDS >= 2, "Expect at least 2 operands for relational comparison");
            if (m_state != State::ARMED)
            {
                return false;
            }

            bool outval = false;
            AnyType opvals[MAX_OPERANDS];
            VariableType optypes[MAX_OPERANDS];
            const unsigned int nb_operand = m_trigger.active_condition->get_operand_count();

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

            bool condition_result = m_trigger.active_condition->evaluate(
                m_trigger.conditions.data(),
                reinterpret_cast<VariableTypeCompare *>(optypes),
                reinterpret_cast<AnyTypeCompare *>(opvals));

            if (condition_result)
            {
                if (m_trigger.previous_val == false)
                {
                    m_trigger.rising_edge_timestamp = m_timebase->get_timestamp();
                }

                if (m_timebase->has_expired(m_trigger.rising_edge_timestamp, m_config.trigger.hold_time_us))
                {
                    outval = true;
                }
            }
            m_trigger.previous_val = condition_result;
            return outval;
        }
    }
}