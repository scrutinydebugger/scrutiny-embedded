//    scrutiny_loop_handler.hpp
//        LoopHandler definition.
//        Loop Handler is to be run in a specific time domain and will make some features available
//        that depends on the execution requency such as embedded datalogging
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_LOOP_HANDLER_H___
#define ___SCRUTINY_LOOP_HANDLER_H___

#include <stdint.h>
#include "scrutiny_setup.hpp"
#include "scrutiny_timebase.hpp"
#include "scrutiny_ipc.hpp"

#if SCRUTINY_ENABLE_DATALOGGING
#include "datalogging/scrutiny_datalogging.hpp"
#endif

namespace scrutiny
{

    enum class LoopType
    {
        FIXED_FREQ,
        VARIABLE_FREQ
    };

    class LoopHandler
    {
    public:
        enum class Main2LoopMessageID
        {
#if SCRUTINY_ENABLE_DATALOGGING
            RELEASE_DATALOGGER_OWNERSHIP,
            TAKE_DATALOGGER_OWNERSHIP,
            DATALOGGER_ARM_TRIGGER
#endif
        };

        enum class Loop2MainMessageID
        {
#if SCRUTINY_ENABLE_DATALOGGING
            DATALOGGER_OWNERSHIP_TAKEN,
            DATALOGGER_OWNERSHIP_RELEASED,
            DATALOGGER_DATA_ACQUIRED
#endif
        };

        struct Main2LoopMessage
        {
            Main2LoopMessageID message_id;
        };

        struct Loop2MainMessage
        {
            Loop2MainMessageID message_id;
        };

        void init(void);
        virtual inline LoopType loop_type(void) const = 0;
        void process_common(timestamp_t timestep_us);
        inline Timebase *get_timebase(void) { return &m_timebase; }

        inline scrutiny::IPCMessage<Main2LoopMessage> *ipc_main2loop(void) { return &m_main2loop_msg; }
        inline scrutiny::IPCMessage<Loop2MainMessage> *ipc_loop2main(void) { return &m_loop2main_msg; }

    protected:
        Timebase m_timebase;
        scrutiny::IPCMessage<Main2LoopMessage> m_main2loop_msg;
        scrutiny::IPCMessage<Loop2MainMessage> m_loop2main_msg;

#if SCRUTINY_ENABLE_DATALOGGING
        datalogging::DataLogger *m_datalogger;
        bool m_owns_datalogger;
        bool m_datalogger_data_acquired;
#endif
    };

    class FixedFrequencyLoopHandler : public LoopHandler
    {
    public:
        FixedFrequencyLoopHandler(timestamp_t timestep_us) : m_timestep_us(timestep_us)
        {
        }
        void process(void);
        virtual inline LoopType loop_type(void) const { return LoopType::FIXED_FREQ; }

    protected:
        const uint32_t m_timestep_us;
    };

    class VariableFrequencyLoopHandler : public LoopHandler
    {
    public:
        void process(timestamp_t timestep_us);
        virtual inline LoopType loop_type(void) const { return LoopType::VARIABLE_FREQ; }
    };
}

#endif //___SCRUTINY_LOOP_HANDLER_H___