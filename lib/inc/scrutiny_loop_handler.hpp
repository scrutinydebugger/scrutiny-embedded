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
                    uint32_t bytes_to_acquire_from_trigger_to_completion;
                    uint32_t write_counter_since_trigger;
                } datalogger_status_update;
#endif
            } data;
        };

        LoopHandler(const char *name = "") : m_name(name),
                                             m_datalogger(nullptr),
                                             m_owns_datalogger(false),
                                             m_datalogger_data_acquired(false)
        {
#if SCRUTINY_ENABLE_DATALOGGING
            m_support_datalogging = true;
#endif
        }

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

        inline void allow_datalogging(bool val)
        {
            m_support_datalogging = val;
        }

        inline bool datalogging_allowed(void) { return m_support_datalogging; }

        inline bool owns_datalogger(void) const
        {
            return m_owns_datalogger;
        }
#endif

    protected:
        /// @brief Initialize the Loop Handler
        void init(MainHandler *main_handler);

        /// @brief Process method common to both FixedFreqLoop and VariableFreqLoop
        /// @param timestep_100ns Timestep since last call
        void process_common(timediff_t timestep_100ns);

        Timebase m_timebase;
        /// @brief  Atomic message transferred from the Main Handler to the Loop Handler
        scrutiny::IPCMessage<Main2LoopMessage> m_main2loop_msg;
        /// @brief  Atomic message transferred from the Loop Handler to the Main Handler
        scrutiny::IPCMessage<Loop2MainMessage> m_loop2main_msg;
        const char *m_name;

#if SCRUTINY_ENABLE_DATALOGGING
        /// @brief A pointer to the datalogger object part of the Main Handler
        datalogging::DataLogger *m_datalogger;
        /// @brief Tells wether this loop is the owner of the datalogger
        bool m_owns_datalogger;
        /// @brief Indicates if data has been acquired and ready to be downloaded or saved
        bool m_datalogger_data_acquired;
        /// @brief Indicates if this loop can do datalogging
        bool m_support_datalogging;
#endif
    };

    /// @brief Handler for Fixed Frequency loops. (Same delta-time between each call to process)
    /// This act as a probe to execute some code in tasks that are not synchronized with the Main Handler task
    class FixedFrequencyLoopHandler : public LoopHandler
    {
    public:
        /// @brief Constructor
        /// @param timestep_100ns Time delta between each call to process() in multiple of 100ns
        /// @param name The name of the loop
        explicit FixedFrequencyLoopHandler(timediff_t timestep_100ns, const char *name = "") : LoopHandler(name),
                                                                                               m_timestep_100ns(timestep_100ns)
        {
        }
        /// @brief Process function be called at each iteration of the loop.
        void process(void);

        /// @brief Return the type of loop handler
        virtual LoopType loop_type(void) const override { return LoopType::FIXED_FREQ; }

        /// @brief Returns the time delta assigned to the loop (in multiple of 100ns)
        virtual uint32_t get_timestep_100ns(void) const override { return m_timestep_100ns; }

    protected:
        const uint32_t m_timestep_100ns;
    };

    /// @brief Handler for Variable Frequency loops. (Variable delta-time between each call to process)
    /// This act as a probe to execute some code in tasks that are not synchronized with the Main Handler task
    class VariableFrequencyLoopHandler : public LoopHandler
    {
    public:
        /// @brief Constructor
        /// @param name The name of the loop
        VariableFrequencyLoopHandler(const char *name = "") : LoopHandler(name)
        {
        }

        /// @brief Stubbed implementation that always return 0
        virtual uint32_t get_timestep_100ns(void) const override { return 0; }

        /// @brief Process function be called at each iteration of the loop.
        /// @param timestep_100ns Time delta since last call to process() in multiple of 100ns
        void process(timediff_t timestep_100ns);

        /// @brief Return the type of loop handler
        virtual LoopType loop_type(void) const override { return LoopType::VARIABLE_FREQ; }
    };
}

#endif //___SCRUTINY_LOOP_HANDLER_H___