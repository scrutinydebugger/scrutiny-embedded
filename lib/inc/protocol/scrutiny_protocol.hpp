//    scrutiny_protocol.hpp
//        Main include file for the scrutiny protocol
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_PROTOCOL_H___
#define ___SCRUTINY_PROTOCOL_H___

#include "scrutiny_setup.hpp"

#include "protocol/scrutiny_comm_handler.hpp"
#include "protocol/scrutiny_protocol_definitions.hpp"
#if SCRUTINY_ACTUAL_PROTOCOL_VERSION == SCRUTINY_PROTOCOL_VERSION(1, 0)
#include "protocol/scrutiny_codec_v1_0.hpp"
#endif

#endif //___SCRUTINY_PROTOCOL_H___