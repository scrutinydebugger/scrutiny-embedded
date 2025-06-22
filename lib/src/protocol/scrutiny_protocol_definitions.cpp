//    scrutiny_protocol_definitions.cpp
//        Instanciation of protocol definitions such as magic payload.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "protocol/scrutiny_protocol_definitions.hpp"
#include <stdint.h>

// Randomly generated
unsigned char const scrutiny::protocol::CommControl::DISCOVER_MAGIC[4] = { 0x7E, 0x18, 0xFC, 0x68 };
unsigned char const scrutiny::protocol::CommControl::CONNECT_MAGIC[4] = { 0x82, 0x90, 0x22, 0x66 };
