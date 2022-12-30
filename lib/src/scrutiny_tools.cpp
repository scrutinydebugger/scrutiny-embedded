//    scrutiny_tools.cpp
//        Some tools used across the project
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include <stdint.h>
#include "scrutiny_types.hpp"
#include "scrutiny_tools.hpp"

namespace scrutiny
{
    namespace tools
    {
        VariableTypeSize get_required_type_size(const uint_fast8_t newsize)
        {
            if (newsize <= 1)
            {
                return VariableTypeSize::_8;
            }
            else if (newsize <= 2)
            {
                return VariableTypeSize::_16;
            }
            else if (newsize <= 4)
            {
                return VariableTypeSize::_32;
            }
            else if (newsize <= 8)
            {
                return VariableTypeSize::_64;
            }
            else if (newsize <= 16)
            {
                return VariableTypeSize::_128;
            }
            else if (newsize <= 32)
            {
                return VariableTypeSize::_256;
            }
            else
            {
                return VariableTypeSize::_undef;
            }
        }

        bool is_supported_type(VariableType vt)
        {
            VariableTypeType tt = get_var_type_type(vt);
            uint8_t ts = get_type_size(vt);

            if (ts == 0)
            {
                return false;
            }

#if SCRUTINY_SUPPORT_64BITS

            if (ts > 8)
            {
                return false;
            }
#else
            if (ts > 4)
            {
                return false;
            }
#endif

            if (tt == VariableTypeType::_boolean)
            {
                return (ts == 1);
            }
            else if (tt == VariableTypeType::_float)
            {
                return (ts == 4 || ts == 8);
            }
            else
            {
                return (tt == VariableTypeType::_uint || tt == VariableTypeType::_sint);
            }
        }
    }
}