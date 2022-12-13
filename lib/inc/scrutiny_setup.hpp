//    scrutiny_setup.h
//        Compile-time configuration of embedded Scrutiny module.
//        This configuration is global for the module and should be set once per project
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_SETUP_H___
#define ___SCRUTINY_SETUP_H___

#include "scrutiny_software_id.hpp"

#define SCRUTINY_PROTOCOL_VERSION(MAJOR, MINOR) ((((MAJOR) << 8) & 0xFF00) | ((MINOR)&0xFF))
#define SCRUTINY_PROTOCOL_VERSION_MAJOR(v) ((v >> 8) & 0xFF)
#define SCRUTINY_PROTOCOL_VERSION_MINOR(v) (v & 0xFF)

// ========== Parameters ==========

#define SCRUTINY_COMM_RX_TIMEOUT_US 50000u                                 // Reset reception state machine when no data is received for that amount of time.
#define SCRUTINY_COMM_HEARTBEAT_TMEOUT_US 5000000u                         // Disconnect session if no heartbeat request after this delay
#define SCRUTINY_ACTUAL_PROTOCOL_VERSION SCRUTINY_PROTOCOL_VERSION(1u, 0u) // protocol version to use

// ================================

// ========================= Sanity check =====================
#if SCRUTINY_ACTUAL_PROTOCOL_VERSION != SCRUTINY_PROTOCOL_VERSION(1, 0) // Only v1.0 for now.
#error Unsupported protocol version
#endif

#if SCRUTINY_TX_BUFFER_SIZE > 0xFFFF || SCRUTINY_RX_BUFFER_SIZE > 0xFFFF
#error Scrutiny protocol is limited to 16bits data length
#endif

#endif // ___SCRUTINY_H___