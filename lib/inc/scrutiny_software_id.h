//    scrutiny_software_id.h
//        Contains the definition of the Scrutiny ID global var used for tagging of firmware.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny)
//
//   Copyright (c) 2021-2022 scrutinydebugger

#ifndef ___SCRUTINY_SOFTWARE_ID_H___
#define ___SCRUTINY_SOFTWARE_ID_H___

#include <cstdint>

#define SCRUTINY_SOFTWARE_ID_LENGTH 32u

//Randomly generated
#define SCRUTINY_SOFTWARE_ID_PLACEHOLDER {            \
    0xA9, 0xDC, 0xC0, 0x65, 0x28, 0xFD, 0x41, 0xFA,   \
    0x7C, 0xE8, 0x63, 0xD6, 0xAA, 0x94, 0xA7, 0x08,   \
    0x62, 0xAF, 0x62, 0x6E, 0x41, 0x71, 0x6B, 0x6E,   \
    0x35, 0x01, 0xB6, 0x43, 0x86, 0x4A, 0x0A, 0xBC    \
    }  

namespace scrutiny
{
	extern const uint8_t software_id[SCRUTINY_SOFTWARE_ID_LENGTH];
}
#endif   // ___SCRUTINY_SOFTWARE_ID_H___

