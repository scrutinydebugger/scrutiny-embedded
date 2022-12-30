//    scrutiny_main_handler.hpp
//        The main scrutiny class to be manipulated by the user
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

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
        void init(const Config *config);
        bool get_rpv(const uint16_t id, RuntimePublishedValue *rpv) const;
        bool rpv_exists(const uint16_t id) const;
        VariableType get_rpv_type(const uint16_t id) const;

        void process(const uint32_t timestep_us);
        void process_loops(void);
        void process_request(const protocol::Request *const request, protocol::Response *const response);
        protocol::ResponseCode process_get_info(const protocol::Request *const request, protocol::Response *const response);
        protocol::ResponseCode process_comm_control(const protocol::Request *const request, protocol::Response *const response);
        protocol::ResponseCode process_memory_control(const protocol::Request *const request, protocol::Response *const response);
        protocol::ResponseCode process_user_command(const protocol::Request *const request, protocol::Response *const response);

#if SCRUTINY_ENABLE_DATALOGGING
        inline bool datalogging_error(void) const
        {
            return m_datalogging.datalogger.in_error() || m_datalogging.error != DataloggingError::NoError;
        }
        protocol::ResponseCode process_datalog_control(const protocol::Request *const request, protocol::Response *const response);
        void process_datalogging_loop_msg(LoopHandler *sender, LoopHandler::Loop2MainMessage *msg);
        void process_datalogging_logic(void);
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

        inline protocol::CommHandler *comm(void)
        {
            return &m_comm_handler;
        }

        inline Config *get_config(void) { return &m_config; }

    private:
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
#endif

#if SCRUTINY_ENABLE_DATALOGGING
        enum class DataloggingError
        {
            NoError,
            UnexpectedRelease,
            UnexpectedClaim,
            UnexpectedData
        };

        struct
        {

            datalogging::DataLogger datalogger;
            LoopHandler *owner;
            LoopHandler *new_owner;
            DataloggingError error;
            bool data_available;
            bool request_arm_trigger;
            bool request_ownership_release;
            bool pending_ownership_release;
        } m_datalogging;

#endif
    };
}

#endif