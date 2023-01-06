//    scrutiny_loop_handler.hpp
//        LoopHandler definition.
//        Loop Handler is to be run in a specific time domain and will make some features available
//        that depends on the execution requency such as embedded datalogging
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

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
    class MainHandler;

    enum class LoopType
    {
        FIXED_FREQ,
        VARIABLE_FREQ
    };

    class LoopHandler
    {
        friend class scrutiny::MainHandler;

    public:
        enum class Main2LoopMessageID
        {
#if SCRUTINY_ENABLE_DATALOGGING
            RELEASE_DATALOGGER_OWNERSHIP,
            TAKE_DATALOGGER_OWNERSHIP,
            DATALOGGER_ARM_TRIGGER,
            DATALOGGER_DISARM_TRIGGER
#endif
        };

        enum class Loop2MainMessageID
        {
#if SCRUTINY_ENABLE_DATALOGGING
            DATALOGGER_OWNERSHIP_TAKEN,
            DATALOGGER_OWNERSHIP_RELEASED,
            DATALOGGER_DATA_ACQUIRED,
            DATALOGGER_STATUS_UPDATE
#endif
        };

        struct Main2LoopMessage
        {
            Main2LoopMessageID message_id;
        };

        struct Loop2MainMessage
        {
            Loop2MainMessageID message_id;
            union
            {
#if SCRUTINY_ENABLE_DATALOGGING
                struct
                {
                    datalogging::DataLogger::State state;
                } datalogger_status_update;
#endif
            } data;
        };

        LoopHandler(const char *name = nullptr) : m_name(name) {}

        /// @brief Returns the loop type: Fixed frequency or variable frequency
        virtual LoopType loop_type(void) const = 0;

        virtual uint32_t get_timestep_100ns(void) const = 0;

        /// @brief Return the timebase used by the Loop Handler
        inline Timebase *get_timebase(void) { return &m_timebase; }

        /// @brief Returns the IPC object to send a message to the Loop Handler
        inline scrutiny::IPCMessage<Main2LoopMessage> *ipc_main2loop(void) { return &m_main2loop_msg; }

        /// @brief Returns the IPC object to receive a message from the Loop Handler
        inline scrutiny::IPCMessage<Loop2MainMessage> *ipc_loop2main(void) { return &m_loop2main_msg; }

        /// @brief Returns the name of the loop. May be nullptr if not set.
        inline const char *get_name(void) const { return m_name; }
#if SCRUTINY_ENABLE_DATALOGGING
        inline bool owns_datalogger(void) const
        {
            return m_owns_datalogger;
        }
#endif

    protected:
        /// @brief Initialize the Loop Handler
        void init(MainHandler *main_handler);
        void process_common(timestamp_t timestep_100ns);

        Timebase m_timebase;
        /// @brief  Atomic message transferred from the Main Handler to the Loop Handler
        scrutiny::IPCMessage<Main2LoopMessage> m_main2loop_msg;
        /// @brief  Atomic message transferred from the Loop Handler to the Main Handler
        scrutiny::IPCMessage<Loop2MainMessage> m_loop2main_msg;
        const char *m_name;

#if SCRUTINY_ENABLE_DATALOGGING
        /// @brief A pointer to the datalogger object part of the Main Handler
        datalogging::DataLogger *m_datalogger;
        /// @brief TElls wehter this loop is the owner of the datalogger
        bool m_owns_datalogger;
        /// @brief Indicates if data has been acquired and ready to be downlaoded or saved
        bool m_datalogger_data_acquired;
#endif
    };

    class FixedFrequencyLoopHandler : public LoopHandler
    {
    public:
        FixedFrequencyLoopHandler(timediff_t timestep_100ns, const char *name = nullptr) : LoopHandler(name),
                                                                                           m_timestep_100ns(timestep_100ns)
        {
        }
        void process(void);
        virtual LoopType loop_type(void) const { return LoopType::FIXED_FREQ; }
        virtual uint32_t get_timestep_100ns(void) const { return m_timestep_100ns; }

    protected:
        const uint32_t m_timestep_100ns;
    };

    class VariableFrequencyLoopHandler : public LoopHandler
    {
    public:
        VariableFrequencyLoopHandler(const char *name = nullptr) : LoopHandler(name)
        {
        }
        virtual uint32_t get_timestep_100ns(void) const { return 0; }
        void process(timediff_t timestep_100ns);
        virtual LoopType loop_type(void) const { return LoopType::VARIABLE_FREQ; }
    };
}

#endif //___SCRUTINY_LOOP_HANDLER_H___