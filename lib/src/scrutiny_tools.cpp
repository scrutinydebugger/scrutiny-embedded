//    scrutiny_tools.cpp
//        Some tools used across the project
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny_tools.hpp"
#include "scrutiny_setup.hpp"
#include "scrutiny_types.hpp"
#include <limits.h>
#include <stdint.h>

namespace scrutiny
{
    namespace tools
    {
        VariableTypeSize::eVariableTypeSize get_required_type_size_8bits(uint_fast8_t const size)
        {
#if CHAR_BIT == 8
            if (size <= 1)
            {
                return VariableTypeSize::_8;
            }
            else
#endif
                if (size <= 2)
            {
                return VariableTypeSize::_16;
            }
            else if (size <= 4)
            {
                return VariableTypeSize::_32;
            }
            else if (size <= 8)
            {
                return VariableTypeSize::_64;
            }
            else if (size <= 16)
            {
                return VariableTypeSize::_128;
            }
            else if (size <= 32)
            {
                return VariableTypeSize::_256;
            }
            else
            {
                return VariableTypeSize::_undef;
            }
        }

        VariableTypeSize::eVariableTypeSize get_required_type_size_char(uint_fast8_t const size)
        {
#if CHAR_BIT == 8
            if (size <= sizeof(uint8_t))
            {
                return VariableTypeSize::_8;
            }
            else
#endif
                if (size <= 2 / (CHAR_BIT / 8))
            {
                return VariableTypeSize::_16;
            }
            else if (size <= 4 / (CHAR_BIT / 8))
            {
                return VariableTypeSize::_32;
            }
            else if (size <= 8 / (CHAR_BIT / 8))
            {
                return VariableTypeSize::_64;
            }
            else if (size <= 16 / (CHAR_BIT / 8))
            {
                return VariableTypeSize::_128;
            }
            else if (size <= 32 / (CHAR_BIT / 8))
            {
                return VariableTypeSize::_256;
            }
            else
            {
                return VariableTypeSize::_undef;
            }
        }

        bool is_supported_type(VariableType::eVariableType const vt)
        {
            VariableTypeType::eVariableTypeType tt = get_var_type_type(vt);
            uint_least8_t ts = get_type_size_8bits(vt);

            if (vt == VariableType::boolean) // Native bool. Unknown size.
            {
                return true;
            }

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

#if CHAR_BIT > 8
            if (ts == 1)
            {
                return false;
            }
#endif

            if (tt == VariableTypeType::_boolean)
            {
                return (ts == tools::get_platform_boolean_size_8bits()); // We can only read native bool
            }
            else if (tt == VariableTypeType::_float)
            {
#if SCRUTINY_SUPPORT_64BITS
                return (ts == 4 || ts == 8);
#else
                return (ts == 4);
#endif
            }
            else
            {
                return (tt == VariableTypeType::_uint || tt == VariableTypeType::_sint);
            }
        }

        uint32_t crc32(unsigned char const *data, uint32_t const size, uint32_t const start_value)
        {
            static uint32_t const table[16] = { 0x00000000u, 0x1DB71064u, 0x3B6E20C8u, 0x26D930ACu, 0x76DC4190u, 0x6B6B51F4u,
                                                0x4DB26158u, 0x5005713Cu, 0xEDB88320u, 0xF00F9344u, 0xD6D6A3E8u, 0xCB61B38Cu,
                                                0x9B64C2B0u, 0x86D3D2D4u, 0xA00AE278u, 0xBDBDF21Cu };
            uint32_t crc = ~start_value;
            for (uint32_t i = 0; i < size; i++)
            {
                unsigned char const byte = data[i] & 0xFFu;
                crc = table[(crc ^ byte) & 0x0Fu] ^ (crc >> 4);
                crc = table[(crc ^ (byte >> 4)) & 0x0Fu] ^ (crc >> 4);
            }
            return ~crc;
        }

    } // namespace tools
} // namespace scrutiny
