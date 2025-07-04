//    scrutiny_software_id.hpp
//        Contains the definition of the Scrutiny ID global var used for tagging of firmware.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_SOFTWARE_ID_H___
#define ___SCRUTINY_SOFTWARE_ID_H___

#include "scrutiny_setup.hpp"
#include <stdint.h>

#define SCRUTINY_SOFTWARE_ID_LENGTH 16u

// Randomly generated
#define SCRUTINY_SOFTWARE_ID_PLACEHOLDER                                                                                                             \
    {                                                                                                                                                \
        0xA9, 0xDC, 0xC0, 0x65, 0x28, 0xFD, 0x41, 0xFA, 0x7C, 0xE8, 0x63, 0xD6, 0xAA, 0x94, 0xA7, 0x08                                               \
    }

namespace scrutiny
{
    /// @brief The Scrutiny software ID used to identify the firmware with a unique hash.
    /// The offline toolchain will generate and inject its value after the binary has been produced.
    extern uint8_t const software_id[SCRUTINY_SOFTWARE_ID_LENGTH];
} // namespace scrutiny
#endif // ___SCRUTINY_SOFTWARE_ID_H___
