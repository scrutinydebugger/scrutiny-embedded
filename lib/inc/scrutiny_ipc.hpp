//    scrutiny_ipc.hpp
//        Some tools for inter-process communications
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny_setup.hpp"

#if SCRUTINY_BUILD_AVR_GCC
#include "ipc/scrutiny_ipc_avr.hpp"
#elif SCRUTINY_HAS_CPP11
#include "ipc/scrutiny_ipc_std_atomic.hpp"
#elif SCRUTINY_BUILD_X64 || SCRUTINY_BUILD_X86
#include "ipc/scrutiny_ipc_x86.hpp"    
#else
#error "No IPC capabilities"
#endif 
