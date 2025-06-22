//    scrutiny_software_id.cpp
//        Instanciation of the global ID used for firmware tagging
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include <stdint.h>

#include "scrutiny_software_id.hpp"

namespace scrutiny
{
    unsigned char const software_id[SCRUTINY_SOFTWARE_ID_LENGTH] = SCRUTINY_SOFTWARE_ID_PLACEHOLDER;
}
