//    scrutiny_loop_handler.cpp
//        LoopHandler implementation.
//        Loop Handler is to be run in a specific time domain and will make some features available
//        that depends on the execution requency such as embedded datalogging
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#include "scrutiny_setup.hpp"
#include "scrutiny_loop_handler.hpp"
#include "scrutiny_timebase.hpp"
#include "scrutiny_main_handler.hpp"

namespace scrutiny
{
    void LoopHandler::init(MainHandler *main_handler)
    {
        m_main2loop_msg.clear();
        m_loop2main_msg.clear();
#if SCRUTINY_ENABLE_DATALOGGING
        m_owns_datalogger = false;
        m_datalogger_data_acquired = false;
        m_datalogger = main_handler->datalogger();
#else
        static_cast<void>(main_handler);
#endif
    }

    void LoopHandler::process_common(timediff_t timestep_100ns)
    {
        m_timebase.step(timestep_100ns);

        Loop2MainMessage msg_out{};
        static_cast<void>(msg_out);

        if (m_main2loop_msg.has_content() && !m_loop2main_msg.has_content())
        {
            Main2LoopMessage msg_in = m_main2loop_msg.pop();
            switch (msg_in.message_id)
            {
#if SCRUTINY_ENABLE_DATALOGGING
            case Main2LoopMessageID::TAKE_DATALOGGER_OWNERSHIP:
                m_owns_datalogger = true;
                m_datalogger_data_acquired = false;
                msg_out.message_id = Loop2MainMessageID::DATALOGGER_OWNERSHIP_TAKEN;
                m_loop2main_msg.send(msg_out);
                break;
            case Main2LoopMessageID::DATALOGGER_ARM_TRIGGER:
                if (m_owns_datalogger)
                {
                    m_datalogger->arm_trigger();
                }
                break;
            case Main2LoopMessageID::DATALOGGER_DISARM_TRIGGER:
                if (m_owns_datalogger)
                {
                    m_datalogger->disarm_trigger();
                }
                break;
            case Main2LoopMessageID::RELEASE_DATALOGGER_OWNERSHIP:
                if (m_owns_datalogger)
                {
                    m_owns_datalogger = false;
                    msg_out.message_id = Loop2MainMessageID::DATALOGGER_OWNERSHIP_RELEASED;
                    m_loop2main_msg.send(msg_out);
                }
                break;
#endif
            default:
                break;
            }
        }

#if SCRUTINY_ENABLE_DATALOGGING
        if (m_owns_datalogger)
        {
            m_datalogger->process();

            if (!m_loop2main_msg.has_content())
            {
                if (m_datalogger->data_acquired() && !m_datalogger_data_acquired)
                {
                    msg_out.message_id = Loop2MainMessageID::DATALOGGER_DATA_ACQUIRED;
                    m_loop2main_msg.send(msg_out);
                    m_datalogger_data_acquired = true;
                }
                else // Keep last for lowest priority. Status is given when nothing else is to be done.
                {
                    msg_out.message_id = Loop2MainMessageID::DATALOGGER_STATUS_UPDATE;
                    msg_out.data.datalogger_status_update.state = m_datalogger->get_state();
                    if (msg_out.data.datalogger_status_update.state == datalogging::DataLogger::State::TRIGGERED)
                    {
                        // write counter gets reset on trigger
                        msg_out.data.datalogger_status_update.bytes_to_acquire_from_trigger_to_completion = m_datalogger->get_bytes_to_acquire_from_trigger_to_completion();
                        msg_out.data.datalogger_status_update.write_counter_since_trigger = m_datalogger->data_counter_since_trigger();
                    }
                    else
                    {
                        msg_out.data.datalogger_status_update.bytes_to_acquire_from_trigger_to_completion = 0;
                        msg_out.data.datalogger_status_update.write_counter_since_trigger = 0;
                    }

                    m_loop2main_msg.send(msg_out);
                }
            }
        }
#endif
    }

    void FixedFrequencyLoopHandler::process()
    {
        process_common(m_timestep_100ns);
    }
    void VariableFrequencyLoopHandler::process(timediff_t timestep_100ns)
    {
        process_common(timestep_100ns);
    }

}