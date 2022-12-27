//    scrutiny_setup.hpp
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

#define SCRUTINY_DATALOGGING_ENCODING_RAW 0

// ========== Parameters ==========
#ifndef SCRUTINY_ENABLE_DATALOGGING
#define SCRUTINY_ENABLE_DATALOGGING 1
#endif

#ifndef SCRUTINY_SUPPORT_64BITS
#define SCRUTINY_SUPPORT_64BITS 1
#endif

#define SCRUTINY_COMM_RX_TIMEOUT_US 50000u                                 // Reset reception state machine when no data is received for that amount of time.
#define SCRUTINY_COMM_HEARTBEAT_TMEOUT_US 5000000u                         // Disconnect session if no heartbeat request after this delay
#define SCRUTINY_ACTUAL_PROTOCOL_VERSION SCRUTINY_PROTOCOL_VERSION(1u, 0u) // protocol version to use

#if SCRUTINY_ENABLE_DATALOGGING
#define SCRUTINY_DATALOGGING_MAX_SIGNAL 32
#define SCRUTINY_DATALOGGING_ENCODING SCRUTINY_DATALOGGING_ENCODING_RAW
#endif

// ================================

#if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN32__) || defined(__CYGWIN64__) || defined(_MSC_VER) || defined(_WIN64) || defined(__WIN64__) || defined(__MINGW32__) || defined(__MINGW64__)
#define SCRUTINY_BUILD_WINDOWS 1
#else
#define SCRUTINY_BUILD_WINDOWS 0
#endif

#define SCRUTINY_BUILD_NON_WINDOWS (!SCRUTINY_BUILD_WINDOWS)

#if defined(__AVR_ARCH__)
#define SCRUTINY_BUILD_AVR_GCC 1
#else
#define SCRUTINY_BUILD_AVR_GCC 0
#endif

// ========================= Sanity check =====================
#if SCRUTINY_ACTUAL_PROTOCOL_VERSION != SCRUTINY_PROTOCOL_VERSION(1, 0) // Only v1.0 for now.
#error Unsupported protocol version
#endif

#if SCRUTINY_BUILD_WINDOWS && SCRUTINY_BUILD_AVR_GCC
#error Bad detection of build environment
#endif

#endif // ___SCRUTINY_H___