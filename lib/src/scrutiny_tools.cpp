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

        void ConvertValueToBiggestFormat(VariableType *vtype, AnyType *val)
        {
            switch (*vtype)
            {
#if SCRUTINY_SUPPORT_64BITS
            case VariableType::float64:
                *vtype = VariableType::float32;
                val->float32 = static_cast<float>(val->float64);
                break;
#endif
            case VariableType::boolean:
                *vtype = BiggestUint;
                tools::set_biggest_uint(*val, static_cast<uint_biggest_t>(val->boolean));
                break;

            case VariableType::sint8:
                *vtype = BiggestSint;
                tools::set_biggest_sint(*val, static_cast<uint_biggest_t>(val->sint8));
                break;

            case VariableType::sint16:
                *vtype = BiggestSint;
                tools::set_biggest_sint(*val, static_cast<uint_biggest_t>(val->sint16));
                break;

            case VariableType::sint32:
                *vtype = BiggestSint;
                tools::set_biggest_sint(*val, static_cast<uint_biggest_t>(val->sint32));
                break;

            case VariableType::uint8:
                *vtype = BiggestUint;
                tools::set_biggest_uint(*val, static_cast<uint_biggest_t>(val->uint8));
                break;

            case VariableType::uint16:
                *vtype = BiggestUint;
                tools::set_biggest_uint(*val, static_cast<uint_biggest_t>(val->uint16));
                break;

            case VariableType::uint32:
                *vtype = BiggestUint;
                tools::set_biggest_uint(*val, static_cast<uint_biggest_t>(val->uint32));
                break;

            default:
                break;
            }
        }
    }
}