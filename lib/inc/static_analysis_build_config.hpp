//    static_analysis_build_config.hpp
//        Stubbed configuration file used for static analysis with hardcoded values instead
//        of values coming from cmake
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#ifndef ___STATIC_ANALYSIS_BUILD_CONFIG_H___
#define ___STATIC_ANALYSIS_BUILD_CONFIG_H___

#ifndef SCRUTINY_SUPPORT_64BITS
#define SCRUTINY_SUPPORT_64BITS 1
#endif

#ifndef DSCRUTINY_ENABLE_DATALOGGING
#define DSCRUTINY_ENABLE_DATALOGGING 1
#endif

#define SCRUTINY_REQUEST_MAX_PROCESS_TIME_US 100000u
#define SCRUTINY_COMM_RX_TIMEOUT_US 50000u
#define SCRUTINY_COMM_HEARTBEAT_TIMEOUT_US 5000000u
#define SCRUTINY_ACTUAL_PROTOCOL_VERSION SCRUTINY_PROTOCOL_VERSION(1, 0u)

#if SCRUTINY_ENABLE_DATALOGGING
#define SCRUTINY_DATALOGGING_MAX_SIGNAL 32u
#define SCRUTINY_DATALOGGING_ENCODING SCRUTINY_DATALOGGING_ENCODING_RAW
#define SCRUTINY_DATALOGGING_BUFFER_32BITS 1
#endif

#endif //___STATIC_ANALYSIS_BUILD_CONFIG_H___
