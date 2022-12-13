//    scrutiny_main_handler.h
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

namespace scrutiny
{
    class MainHandler
    {

    public:
        void init(Config *config);
        bool get_rpv(uint16_t id, RuntimePublishedValue *rpv);
        VariableType get_rpv_type(uint16_t id);

        void process(const uint32_t timestep_us);

        void process_request(const protocol::Request *request, protocol::Response *response);
        protocol::ResponseCode process_get_info(const protocol::Request *request, protocol::Response *response);
        protocol::ResponseCode process_comm_control(const protocol::Request *request, protocol::Response *response);
        protocol::ResponseCode process_memory_control(const protocol::Request *request, protocol::Response *response);
        protocol::ResponseCode process_user_command(const protocol::Request *request, protocol::Response *response);

        inline protocol::CommHandler *comm()
        {
            return &m_comm_handler;
        }

        inline Config *get_config() { return &m_config; }

    private:
        bool touches_forbidden_region(const protocol::MemoryBlock *block);
        bool touches_readonly_region(const protocol::MemoryBlock *block);
        void check_config();

        Timebase m_timebase;
        protocol::CommHandler m_comm_handler;
        bool m_processing_request;
        bool m_disconnect_pending;
        Config m_config;
        bool m_enabled;
#if SCRUTINY_ACTUAL_PROTOCOL_VERSION == SCRUTINY_PROTOCOL_VERSION(1, 0)
        protocol::CodecV1_0 m_codec;
#endif
    };
}

#endif