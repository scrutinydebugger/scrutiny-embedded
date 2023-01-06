//    scrutiny_main_handler.hpp
//        The main scrutiny class to be manipulated by the user
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#ifndef __SCRUTINY_HANDLER_H__
#define __SCRUTINY_HANDLER_H__

#include <stdint.h>

#include "scrutiny_setup.hpp"
#include "scrutiny_loop_handler.hpp"
#include "scrutiny_timebase.hpp"
#include "protocol/scrutiny_protocol.hpp"
#include "scrutiny_config.hpp"

#if SCRUTINY_ENABLE_DATALOGGING
#include "datalogging/scrutiny_datalogging.hpp"
#endif

namespace scrutiny
{
    class MainHandler
    {

    public:
        /// @brief Initialize the scrutiny Main Handler
        /// @param config A pointer to allocated configuration object. No copy will be made, config must stay allocated forever.
        void init(const Config *config);

        /// @brief Gets the Runtime Published Definition from its ID
        /// @param id The RPV ID
        /// @param rpv The Runtime Published Value object to writes to
        /// @return true if the RPV has been found, false otherwise
        bool get_rpv(const uint16_t id, RuntimePublishedValue *rpv) const;

        /// @brief Tells if a Runtime Published Values with the given ID has been defined.
        /// @param id The RPV ID
        /// @return True if RPV exists in configuration.
        bool rpv_exists(const uint16_t id) const;

        /// @brief Returns the type of a Runtime Published Value identified by its ID
        /// @param id The RPV ID
        /// @return The VariableType object of the RPV.  VariableType::Unknown if the given ID is not set in the configuration.
        VariableType get_rpv_type(const uint16_t id) const;

        /// @brief Periodic process loop to be called as fast as possible
        /// @param timestep_100ns The time elapsed since last call to this function, in multiple of 100ns.
        void process(const timediff_t timestep_100ns);

#if SCRUTINY_ENABLE_DATALOGGING
        /// @brief Returns the state of the datalogger. Thread safe
        inline datalogging::DataLogger::State get_datalogger_state(void) const
        {
            return m_datalogging.datalogger_state_thread_safe;
        }

        /// @brief  Returns true if the datalogger has data available. Thread safe
        inline bool datalogging_data_available(void) const
        {
            return m_datalogging.datalogger_state_thread_safe == datalogging::DataLogger::State::ACQUISITION_COMPLETED; // Thread safe.
        }

        /// @brief Returns true if the datalogger is in an error state. Thread safe
        inline bool datalogging_error(void) const
        {
            return (m_datalogging.datalogger_state_thread_safe == datalogging::DataLogger::State::ERROR) || m_datalogging.error != DataloggingError::NoError;
        }

        bool read_memory(void *dst, const void *src, const uint32_t size) const;
        bool fetch_variable(const void *addr, const VariableType variable_type, AnyType *val) const;
        bool fetch_variable_bitfield(
            const void *addr,
            const VariableTypeType var_tt,
            const uint_fast8_t bitoffset,
            const uint_fast8_t bitsize,
            AnyType *val,
            VariableType *output_type) const;
        inline datalogging::DataLogger *datalogger(void) { return &m_datalogging.datalogger; }
#endif
        inline RpvReadCallback get_rpv_read_callback(void) const
        {
            return m_config.get_rpv_read_callback();
        }

        inline protocol::CommHandler *comm(void) { return &m_comm_handler; }

        inline Config *get_config(void) { return &m_config; }

    private:
        void process_loops(void);
        void process_request(const protocol::Request *const request, protocol::Response *const response);
        protocol::ResponseCode process_get_info(const protocol::Request *const request, protocol::Response *const response);
        protocol::ResponseCode process_comm_control(const protocol::Request *const request, protocol::Response *const response);
        protocol::ResponseCode process_memory_control(const protocol::Request *const request, protocol::Response *const response);
        protocol::ResponseCode process_user_command(const protocol::Request *const request, protocol::Response *const response);

#if SCRUTINY_ENABLE_DATALOGGING
        protocol::ResponseCode process_datalog_control(const protocol::Request *const request, protocol::Response *const response);
        void process_datalogging_loop_msg(LoopHandler *sender, LoopHandler::Loop2MainMessage *msg);
        void process_datalogging_logic(void);
#endif
        bool touches_forbidden_region(const MemoryBlock *block) const;
        bool touches_forbidden_region(const void *addr_start, const size_t length) const;
        bool touches_readonly_region(const MemoryBlock *block) const;
        bool touches_readonly_region(const void *addr_start, const size_t length) const;
        void check_config(void);

        Timebase m_timebase;
        protocol::CommHandler m_comm_handler;
        bool m_processing_request;
        bool m_disconnect_pending;
        Config m_config;
        bool m_enabled;
        bool m_process_again_timestamp_taken;
        timestamp_t m_process_again_timestamp;
#if SCRUTINY_ACTUAL_PROTOCOL_VERSION == SCRUTINY_PROTOCOL_VERSION(1, 0)
        protocol::CodecV1_0 m_codec;
#else
#error Unsupported codec
#endif

#if SCRUTINY_ENABLE_DATALOGGING
        enum class DataloggingError
        {
            NoError,
            UnexpectedRelease,
            UnexpectedClaim
        };

        struct
        {

            datalogging::DataLogger datalogger;
            datalogging::DataLogger::State datalogger_state_thread_safe;
            LoopHandler *owner;
            LoopHandler *new_owner;
            DataloggingError error;
            bool request_arm_trigger;
            bool request_ownership_release;
            bool request_disarm_trigger;
            bool pending_ownership_release;
            bool reading_in_progress;
            uint8_t read_acquisition_rolling_counter;
            uint32_t read_acquisition_crc;
        } m_datalogging;

#endif
    };
}

#endif