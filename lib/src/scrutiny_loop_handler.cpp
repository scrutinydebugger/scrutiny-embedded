//    scrutiny_loop_handler.cpp
//        LoopHandler implementation.
//        Loop Handler is to be run in a specific time domain and will make some features available
//        that depends on the execution requency such as embedded datalogging
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include "scrutiny_loop_handler.hpp"
#include "scrutiny_timebase.hpp"

namespace scrutiny
{
    void LoopHandler::init(void)
    {
        m_main2loop_msg.clear();
        m_loop2main_msg.clear();
#if SCRUTINY_ENABLE_DATALOGGING
        m_owns_datalogger = false;
        m_datalogger_data_acquired = false;
#endif
    }

    void LoopHandler::process_common(timestamp_t timestep_us)
    {
        m_timebase.step(timestep_us);

        Loop2MainMessage msg_out;
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

            if (m_datalogger->data_acquired())
            {
                if (!m_loop2main_msg.has_content() && !m_datalogger_data_acquired)
                {
                    msg_out.message_id = Loop2MainMessageID::DATALOGGER_DATA_ACQUIRED;
                    m_loop2main_msg.send(msg_out);
                }
                m_datalogger_data_acquired = true;
            }
        }
#endif
    }

    void FixedFrequencyLoopHandler::process()
    {
        process_common(m_timestep_us);
    }
    void VariableFrequencyLoopHandler::process(timestamp_t timestep_us)
    {
        process_common(timestep_us);
    }

}