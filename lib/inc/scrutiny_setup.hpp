//    scrutiny_setup.hpp
//        Compile-time configuration of embedded Scrutiny module.
//        This configuration is global for the module and should be set once per project
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

// ========== Definitions ==========
#define SCRUTINY_PROTOCOL_VERSION(MAJOR, MINOR) ((((MAJOR) << 8) & 0xFF00) | ((MINOR) & 0xFF))
#define SCRUTINY_PROTOCOL_VERSION_MAJOR(v) ((v >> 8) & 0xFF)
#define SCRUTINY_PROTOCOL_VERSION_MINOR(v) (v & 0xFF)

#define SCRUTINY_DATALOGGING_ENCODING_RAW 0
// =================================

#ifdef SCRUTINY_STATIC_ANALYSIS
#include "static_analysis_build_config.hpp"
#else
#include "scrutiny_build_config.hpp" // Generated by cmake
#endif

// ================================

// ========== Macros ==========
// These can be replaced by intrinsics if available on the platform
#define SCRUTINY_ABS(x) ((x) >= 0) ? (x) : (-(x))
#define SCRUTINY_MIN(x, y) ((x) < (y)) ? (x) : (y)
#define SCRUTINY_MAX(x, y) ((x) > (y)) ? (x) : (y)

#define SCRUTINY_FABS(x) SCRUTINY_ABS(x)
#define SCRUTINY_FMIN(x, y) SCRUTINY_MIN(x, y)
#define SCRUTINY_FMAX(x, y) SCRUTINY_MAX(x, y)  

// ================================

// ========== Platform detection ==========
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
// ======================================

// ========================= Sanity check =====================
#if SCRUTINY_ACTUAL_PROTOCOL_VERSION != SCRUTINY_PROTOCOL_VERSION(1u, 0u) // Only v1.0 for now.
#error Unsupported protocol version
#endif

#if SCRUTINY_BUILD_WINDOWS && SCRUTINY_BUILD_AVR_GCC
#error Bad detection of build environment
#endif
