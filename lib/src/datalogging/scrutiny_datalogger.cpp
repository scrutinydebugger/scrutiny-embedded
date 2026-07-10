//    scrutiny_datalogger.cpp
//        The implementation of the datalogger object that should exist in each loop
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "datalogging/scrutiny_datalogger.hpp"
#include "datalogging/scrutiny_datalogging.hpp"
#include "scrutiny_ipc.hpp"
#include "scrutiny_main_handler.hpp"
#include "scrutiny_setup.hpp"
#include "scrutiny_tools.hpp"

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

namespace scrutiny
{
    namespace datalogging
    {

        static trigger::ActiveCondition const CONDITION_CONFIG_LUT[9] = {
            // IMPORTANT. Keep in sync with  SupportedTriggerConditions enum.
            { // AlwaysTrue
              trigger::AlwaysTrueCondition::evaluate,
              SCRUTINY_NULL_FN_PTR(trigger::ResetFn),
              trigger::AlwaysTrueCondition::get_operand_count() },
            { // Equal
              trigger::EqualCondition::evaluate,
              SCRUTINY_NULL_FN_PTR(trigger::ResetFn),
              trigger::EqualCondition::get_operand_count() },
            { // NotEqual
              trigger::NotEqualCondition::evaluate,
              SCRUTINY_NULL_FN_PTR(trigger::ResetFn),
              trigger::NotEqualCondition::get_operand_count() },
            { // LessThan
              trigger::LessThanCondition::evaluate,
              SCRUTINY_NULL_FN_PTR(trigger::ResetFn),
              trigger::LessThanCondition::get_operand_count() },
            { // LessOrEqualThan
              trigger::LessOrEqualThanCondition::evaluate,
              SCRUTINY_NULL_FN_PTR(trigger::ResetFn),
              trigger::LessOrEqualThanCondition::get_operand_count() },
            { // GreaterThan
              trigger::GreaterThanCondition::evaluate,
              SCRUTINY_NULL_FN_PTR(trigger::ResetFn),
              trigger::GreaterThanCondition::get_operand_count() },
            { // GreaterOrEqualThan
              trigger::GreaterOrEqualThanCondition::evaluate,
              SCRUTINY_NULL_FN_PTR(trigger::ResetFn),
              trigger::GreaterOrEqualThanCondition::get_operand_count() },
            { // ChangeMoreThan
              trigger::ChangeMoreThanCondition::evaluate,
              trigger::ChangeMoreThanCondition::reset,
              trigger::ChangeMoreThanCondition::get_operand_count() },
            { // IsWithin
              trigger::IsWithinCondition::evaluate,
              SCRUTINY_NULL_FN_PTR(trigger::ResetFn),
              trigger::IsWithinCondition::get_operand_count() }
        };

        Status::eStatus DataLogger::init(
            MainHandler const *const main_handler,
            unsigned char *const buffer,
            buffer_size_t const buffer_size,
            trigger_callback_t trigger_callback)
        {
            m_timebase = SCRUTINY_NULL;
            m_main_handler = main_handler;
            m_buffer_size = buffer_size;
            m_trigger_callback = trigger_callback;
            m_owner = SCRUTINY_NULL;

            m_encoder.init(main_handler, &m_config, buffer, buffer_size);
            m_acquisition_id = 0;

            reset();

            return Status::SUCCESS;
        }

        void DataLogger::reset(void)
        {
            m_state = State::Idle;
            m_trigger.previous_val = false;
            m_trigger.rising_edge_timestamp = 0;
            m_trigger.active_condition.eval_fn = trigger::AlwaysTrueCondition::evaluate;
            m_trigger.active_condition.reset_fn = SCRUTINY_NULL_FN_PTR(trigger::ResetFn);
            m_trigger.active_condition.operand_count = trigger::AlwaysTrueCondition::get_operand_count();

            m_trigger_cursor_location = 0;
            m_trigger_timestamp = 0;
            m_remaining_data_to_write = 0;
            m_config_valid = false;
            m_manual_trigger = false;

            m_decimation_counter = 0;
            m_log_points_after_trigger = 0;
        }

        void DataLogger::configure(Timebase *timebase, uint16_t config_id)
        {
            reset();

            m_config_valid = true;
            m_timebase = timebase;
            m_config_id = config_id;

            if (m_config.items_count > SCRUTINY_DATALOGGING_MAX_SIGNAL || m_config.items_count == 0)
            {
                m_config_valid = false;
            }

            if (m_config.trigger.operand_count > datalogging::MAX_OPERANDS)
            {
                m_config_valid = false;
            }

            uint_least8_t const condition_lut_index = static_cast<uint_least8_t>(m_config.trigger.condition);
            if (condition_lut_index < sizeof(CONDITION_CONFIG_LUT) / sizeof(CONDITION_CONFIG_LUT[0]))
            {
                m_trigger.active_condition = CONDITION_CONFIG_LUT[condition_lut_index];
            }
            else
            {
                m_config_valid = false;
            }

            if (m_config.trigger.operand_count != m_trigger.active_condition.operand_count)
            {
                m_config_valid = false;
            }

            if (m_config.decimation == 0)
            {
                m_config_valid = false;
            }

            // Size are consistent so far, we can read the operand and items definition without crashing anything
            if (m_config_valid)
            {
                for (uint_least8_t i = 0; i < m_config.trigger.operand_count; i++)
                {
                    if (m_config.trigger.operands[i].common.type == OperandType::Literal)
                    {
                        if (!tools::is_float_finite(m_config.trigger.operands[i].literal.val))
                        {
                            m_config_valid = false;
                        }
                    }
                    else if (m_config.trigger.operands[i].common.type == OperandType::Rpv)
                    {
                        if (!m_main_handler->get_config_ro()->is_read_published_values_configured())
                        {
                            m_config_valid = false;
                        }

                        if (!m_main_handler->rpv_exists(m_config.trigger.operands[i].rpv.id))
                        {
                            m_config_valid = false;
                        }
                    }
                    else if (m_config.trigger.operands[i].common.type == OperandType::Var)
                    {
                        if (!tools::is_supported_type(m_config.trigger.operands[i].varbit.datatype))
                        {
                            m_config_valid = false;
                        }
                    }
                    else if (m_config.trigger.operands[i].common.type == OperandType::VarBit)
                    {
                        // Works with and without 64bits support
                        if (m_config.trigger.operands[i].varbit.bitoffset > (sizeof(scrutiny::BiggestUint) * CHAR_BIT - 1) ||
                            m_config.trigger.operands[i].varbit.bitsize > sizeof(scrutiny::BiggestUint) * CHAR_BIT)
                        {
                            m_config_valid = false;
                        }

                        if (!tools::is_supported_type(m_config.trigger.operands[i].varbit.datatype))
                        {
                            m_config_valid = false;
                        }

                        if (m_config.trigger.operands[i].varbit.bitoffset + m_config.trigger.operands[i].varbit.bitsize >
                            tools::get_type_size_char(m_config.trigger.operands[i].varbit.datatype) * CHAR_BIT)
                        {
                            m_config_valid = false;
                        }
                    }
                    else
                    {
                        m_config_valid = false;
                    }
                }

                for (uint_least8_t i = 0; i < m_config.items_count; i++)
                {
                    if (m_config.items_to_log[i].common.type == LoggableType::Rpv)
                    {
                        if (!m_main_handler->get_config_ro()->is_read_published_values_configured())
                        {
                            m_config_valid = false;
                        }

                        if (!m_main_handler->rpv_exists(m_config.items_to_log[i].rpv.id))
                        {
                            m_config_valid = false;
                        }
                    }
                    else if (m_config.items_to_log[i].common.type == LoggableType::Memory)
                    {
                        // Nothing to validate
                    }
                    else if (m_config.items_to_log[i].common.type == LoggableType::Time)
                    {
                        // Nothing to validate
                    }
                    else
                    {
                        m_config_valid = false;
                    }
                }
            }

            // The configuration is good. Let's initialize to start logging
            if (m_config_valid)
            {
                m_encoder.set_timebase(m_timebase);
                if (m_trigger.active_condition.reset_fn != SCRUTINY_NULL_FN_PTR(trigger::ResetFn))
                {
                    m_trigger.active_condition.reset_fn(&m_trigger.condition_data);
                }
                m_encoder.reset();
                m_state = State::Configured;
            }
            else
            {
                m_state = State::Error;
            }
        }

        void DataLogger::arm_trigger(void)
        {
            if (m_state == State::Configured || m_state == State::AcquisitionCompleted || m_state == State::Triggered)
            {
                m_state = State::Armed;
            }
        }

        void DataLogger::disarm_trigger(void)
        {
            if (m_state == State::Armed || m_state == State::AcquisitionCompleted || m_state == State::Triggered)
            {
                m_state = State::Configured;
            }
        }

        void DataLogger::process(void)
        {
            // Idle --> Configured --> Armed --> Triggered --> AcquisitionCompleted.
            // We acquire in both Configured and Armed state so that we can have data before the trigger as well.

            switch (m_state)
            {
            case State::Idle:
            case State::Error:
            case State::AcquisitionCompleted:
                break;
            case State::Configured:
            case State::Armed:
            case State::Triggered:
                if (m_encoder.error())
                {
                    m_state = State::Error;
                }
                else
                {
                    process_acquisition();
                    if (m_state == State::Armed)
                    {
                        if (check_trigger())
                        {
                            if (m_trigger_callback != SCRUTINY_NULL)
                            {
                                m_trigger_callback();
                            }
                            stamp_trigger_point();
                            m_state = State::Triggered;
                        }
                    }

                    if (m_state == State::Triggered)
                    {
                        if (acquisition_completed())
                        {
                            m_acquisition_id++;
                            m_state = State::AcquisitionCompleted;
                            m_log_points_after_trigger = m_encoder.get_entry_write_counter();
                        }
                    }
                    break;
                }
            }
        }

        void DataLogger::stamp_trigger_point(void)
        {
            m_trigger_cursor_location = m_encoder.get_write_cursor();
            m_trigger_timestamp = m_timebase->get_timestamp();
            m_encoder.reset_write_counter(); // Completion logic uses that counter directly without processing

            uint64_t const multiplier = static_cast<uint64_t>((1 << (sizeof(m_config.probe_location) * 8)) - 1 - m_config.probe_location);
            m_remaining_data_to_write =
                static_cast<buffer_size_t>((static_cast<uint64_t>(m_buffer_size) * multiplier) >> (sizeof(m_config.probe_location) * 8));
            if (!m_encoder.buffer_full())
            {
                m_remaining_data_to_write = SCRUTINY_MAX(m_remaining_data_to_write, m_encoder.remaining_bytes_to_full());
            }

            if (m_remaining_data_to_write > m_buffer_size)
            {
                m_remaining_data_to_write = m_buffer_size;
            }
        }

        bool DataLogger::acquisition_completed(void)
        {
            if (m_state == State::AcquisitionCompleted)
            {
                return true;
            }

            if (m_state == State::Triggered)
            {
                if (m_config.timeout_100ns > 0)
                {
                    if (m_timebase->has_expired(m_trigger_timestamp, m_config.timeout_100ns))
                    {
                        return true;
                    }
                }

                if (m_encoder.get_data_write_counter() >= m_remaining_data_to_write)
                {
                    return true;
                }
            }

            return false;
        }

        void DataLogger::process_acquisition(void)
        {
            if (++m_decimation_counter >= m_config.decimation)
            {
                m_encoder.encode_next_entry(m_owner);
                m_decimation_counter = 0;
            }
        }

        bool DataLogger::check_trigger(void)
        {
            SCRUTINY_STATIC_ASSERT(MAX_OPERANDS >= 2, "Expect at least 2 operands for relational comparison");
            if (m_state != State::Armed)
            {
                return false;
            }

            bool outval = false;
            const unsigned int nb_operand = m_trigger.active_condition.operand_count;

            if (nb_operand > MAX_OPERANDS)
            {
                return false;
            }

            if (m_manual_trigger)
            {
                m_manual_trigger = false;
                outval = true;
                m_trigger.previous_val = true;
            }
            else
            {

                for (unsigned int i = 0; i < nb_operand; i++)
                {
                    if (fetch_operand(m_main_handler, &m_config.trigger.operands[i], &m_stack_data.check_trigger.ops_data[i], m_owner) == false)
                    {
                        return false;
                    }
                    convert_to_compare_type(&m_stack_data.check_trigger.ops_data[i]);
                }

                bool const condition_result = m_trigger.active_condition.eval_fn(
                    &m_trigger.condition_data,
                    reinterpret_cast<AnyValAndTypeComparePair *>(m_stack_data.check_trigger.ops_data));

                if (condition_result)
                {
                    if (m_trigger.previous_val == false)
                    {
                        m_trigger.rising_edge_timestamp = m_timebase->get_timestamp();
                    }

                    if (m_timebase->has_expired(m_trigger.rising_edge_timestamp, m_config.trigger.hold_time_100ns))
                    {
                        outval = true;
                    }
                }
                m_trigger.previous_val = condition_result;
            }
            return outval;
        }
    } // namespace datalogging
} // namespace scrutiny
