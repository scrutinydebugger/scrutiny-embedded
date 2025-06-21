//    scrutiny_compiler.hpp
//        Macros for compiler feature and environment detection
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_COMPILER_HPP___
#define ___SCRUTINY_COMPILER_HPP___

#ifdef __cplusplus
#if __cplusplus >= 201103
#define SCRUTINY_HAS_CPP11 true
#else
#define SCRUTINY_HAS_CPP11 false
#endif
#endif

// =========== Compiler features =========
#if SCRUTINY_HAS_CPP11
#define SCRUTINY_CONSTEXPR constexpr
#define SCRUTINY_ENUM(type) enum E : type
#define SCRUTINY_STATIC_ASSERT(x, y) static_assert(x, y)
#define SCRUTINY_NULL nullptr
#define SCRUTINY_OVERRIDE override
#define SCRUTINY_FINAL final
#define SCRUTINY_EXPLICIT explicit
#else
#define SCRUTINY_CONSTEXPR const
#define SCRUTINY_ENUM(type) enum E
#define SCRUTINY_STATIC_ASSERT(x, y)
#define SCRUTINY_NULL NULL
#define SCRUTINY_OVERRIDE
#define SCRUTINY_FINAL
#define SCRUTINY_EXPLICIT
#endif

// ========== Platform detection ==========

#define SCRUTINY_BUILD_WINDOWS 0
#define SCRUTINY_BUILD_AVR_GCC 0
#define SCRUTINY_BUILD_X64 0
#define SCRUTINY_BUILD_X86 0

#if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN32__) || defined(__CYGWIN64__) || defined(_MSC_VER) || defined(_WIN64) || defined(__WIN64__) ||   \
    defined(__MINGW32__) || defined(__MINGW64__)
#undef SCRUTINY_BUILD_WINDOWS
#define SCRUTINY_BUILD_WINDOWS 1
#elif defined(__AVR_ARCH__)
#undef SCRUTINY_BUILD_AVR_GCC
#define SCRUTINY_BUILD_AVR_GCC 1
#elif defined(__x86_64__) || defined(_M_X64)
#undef SCRUTINY_BUILD_X64
#define SCRUTINY_BUILD_X64 1
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#undef SCRUTINY_BUILD_X86
#define SCRUTINY_BUILD_X86 1
#endif

#endif //___SCRUTINY_COMPILER_HPP___
