//    scrutiny_common_codecs.cpp
//        Some common encoding/decoding functions used across the project.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny_common_codecs.hpp"
#include "scrutiny_setup.hpp"
#include "scrutiny_tools.hpp"
#include <stdint.h>

#if defined(_MSC_VER)
#pragma warning(disable : 4127) // Condition always true
#endif
namespace scrutiny
{
    namespace codecs
    {
        uint_least8_t decode_address_big_endian(unsigned char const *const buf, uintptr_t *const addr)
        {
            SCRUTINY_STATIC_ASSERT(ADDR_SIZE_U8 == 1 || ADDR_SIZE_U8 == 2 || ADDR_SIZE_U8 == 4 || ADDR_SIZE_U8 == 8, "Unsupported address size");

            uintptr_t computed_addr = 0;
            unsigned int i = 0;

            if (ADDR_SIZE_U8 >= 8)
            {
                computed_addr |= ((static_cast<uintptr_t>(buf[i++]) << ((ADDR_SIZE_U8 >= 8) * 56)));
                computed_addr |= ((static_cast<uintptr_t>(buf[i++]) << ((ADDR_SIZE_U8 >= 8) * 48)));
                computed_addr |= ((static_cast<uintptr_t>(buf[i++]) << ((ADDR_SIZE_U8 >= 8) * 40)));
                computed_addr |= ((static_cast<uintptr_t>(buf[i++]) << ((ADDR_SIZE_U8 >= 8) * 32)));
            }

            if (ADDR_SIZE_U8 >= 4)
            {
                computed_addr |= ((static_cast<uintptr_t>(buf[i++]) << ((ADDR_SIZE_U8 >= 4) * 24)));
                computed_addr |= ((static_cast<uintptr_t>(buf[i++]) << ((ADDR_SIZE_U8 >= 4) * 16)));
            }

            if (ADDR_SIZE_U8 >= 2)
            {
                computed_addr |= ((static_cast<uintptr_t>(buf[i++]) << ((ADDR_SIZE_U8 >= 2) * 8)));
            }

            if (ADDR_SIZE_U8 >= 1)
            {
                computed_addr |= ((static_cast<uintptr_t>(buf[i++]) << ((ADDR_SIZE_U8 >= 1) * 0)));
            }

            *addr = computed_addr;

            return static_cast<uint_least8_t>(ADDR_SIZE_U8);
        }

        uint_least8_t encode_address_big_endian(void const *const addr, unsigned char *const buf)
        {
            return encode_address_big_endian(reinterpret_cast<uintptr_t>(addr), buf);
        }

        uint_least8_t encode_address_big_endian(uintptr_t const addr, unsigned char *const buf)
        {
            SCRUTINY_STATIC_ASSERT(ADDR_SIZE_U8 == 1 || ADDR_SIZE_U8 == 2 || ADDR_SIZE_U8 == 4 || ADDR_SIZE_U8 == 8, "Unsupported address size");

            unsigned int i = ADDR_SIZE_U8 - 1;

            if (ADDR_SIZE_U8 >= 1)
            {
                buf[i--] = static_cast<unsigned char>((addr >> (0 * (ADDR_SIZE_U8 >= 1))) & 0xFF);
            }

            if (ADDR_SIZE_U8 >= 2)
            {
                buf[i--] = static_cast<unsigned char>((addr >> (8 * (ADDR_SIZE_U8 >= 2))) & 0xFF);
            }

            if (ADDR_SIZE_U8 >= 4)
            {
                buf[i--] = static_cast<unsigned char>((addr >> (16 * (ADDR_SIZE_U8 >= 4))) & 0xFF);
                buf[i--] = static_cast<unsigned char>((addr >> (24 * (ADDR_SIZE_U8 >= 4))) & 0xFF);
            }

            if (ADDR_SIZE_U8 == 8)
            {
                buf[i--] = static_cast<unsigned char>((addr >> (32 * (ADDR_SIZE_U8 >= 8))) & 0xFF);
                buf[i--] = static_cast<unsigned char>((addr >> (40 * (ADDR_SIZE_U8 >= 8))) & 0xFF);
                buf[i--] = static_cast<unsigned char>((addr >> (48 * (ADDR_SIZE_U8 >= 8))) & 0xFF);
                buf[i--] = static_cast<unsigned char>((addr >> (56 * (ADDR_SIZE_U8 >= 8))) & 0xFF);
            }

            return static_cast<uint_least8_t>(ADDR_SIZE_U8);
        }

        uint_least8_t encode_anytype_big_endian(AnyType const *const val, VariableType::E const vartype, unsigned char *const buffer)
        {
            uint_least8_t const typesize_u8 = tools::get_type_size_u8(vartype);
            return encode_anytype_big_endian(val, typesize_u8, buffer);
        }

        uint_least8_t encode_anytype_big_endian(AnyType const *const val, uint_least8_t const typesize_u8, unsigned char *const buffer)
        {

            switch (typesize_u8)
            {
            case 1:
                codecs::encode_8_bits(val->uint8, buffer);
                break;
            case 2:
                codecs::encode_16_bits_big_endian(val->uint16, buffer);
                break;
            case 4:
                codecs::encode_32_bits_big_endian(val->uint32, buffer);
                break;
#if SCRUTINY_SUPPORT_64BITS
            case 8:
                codecs::encode_64_bits_big_endian(val->uint64, buffer);
                break;
#endif
            default:
                return 0;
            }
            return typesize_u8;
        }
    } // namespace codecs
} // namespace scrutiny
