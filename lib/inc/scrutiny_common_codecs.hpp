//    scrutiny_common_codecs.hpp
//        Some codecs function used across the project
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#ifndef ___SCRUTINY_COMMON_CODECS_H___
#define ___SCRUTINY_COMMON_CODECS_H___

#include <stdint.h>
#include <string.h>
#include "scrutiny_setup.hpp"
#include "scrutiny_types.hpp"

namespace scrutiny
{
    namespace codecs
    {

        inline uint8_t encode_8_bits(const uint8_t value, uint8_t *buff)
        {
            buff[0] = value;
            return sizeof(uint8_t);
        }

        inline uint8_t encode_16_bits_big_endian(const uint16_t value, uint8_t *buff)
        {
            buff[0] = static_cast<uint8_t>((value >> 8) & 0xFF);
            buff[1] = static_cast<uint8_t>((value >> 0) & 0xFF);
            return sizeof(uint16_t);
        }

        inline uint8_t encode_32_bits_big_endian(const uint32_t value, uint8_t *buff)
        {
            buff[0] = static_cast<uint8_t>((value >> 24) & 0xFFu);
            buff[1] = static_cast<uint8_t>((value >> 16) & 0xFFu);
            buff[2] = static_cast<uint8_t>((value >> 8) & 0xFFu);
            buff[3] = static_cast<uint8_t>((value >> 0) & 0xFFu);
            return sizeof(uint32_t);
        }

        inline uint8_t encode_float_big_endian(const float value, uint8_t *buff)
        {
            uint32_t uv;
            memcpy(&uv, &value, sizeof(uint32_t));
            encode_32_bits_big_endian(uv, buff);
            return sizeof(float);
        }
#if SCRUTINY_SUPPORT_64BITS
        inline uint8_t encode_64_bits_big_endian(const uint64_t value, uint8_t *buff)
        {
            buff[0] = static_cast<uint8_t>((value >> 56) & 0xFFu);
            buff[1] = static_cast<uint8_t>((value >> 48) & 0xFFu);
            buff[2] = static_cast<uint8_t>((value >> 40) & 0xFFu);
            buff[3] = static_cast<uint8_t>((value >> 32) & 0xFFu);
            buff[4] = static_cast<uint8_t>((value >> 24) & 0xFFu);
            buff[5] = static_cast<uint8_t>((value >> 16) & 0xFFu);
            buff[6] = static_cast<uint8_t>((value >> 8) & 0xFFu);
            buff[7] = static_cast<uint8_t>((value >> 0) & 0xFFu);

            return sizeof(uint64_t);
        }
#endif

        inline uint8_t encode_16_bits_little_endian(const uint16_t value, uint8_t *buff)
        {
            buff[1] = static_cast<uint8_t>((value >> 8) & 0xFFu);
            buff[0] = static_cast<uint8_t>((value >> 0) & 0xFFu);
            return sizeof(uint16_t);
        }

        inline uint8_t encode_32_bits_little_endian(const uint32_t value, uint8_t *buff)
        {
            buff[3] = static_cast<uint8_t>((value >> 24) & 0xFFu);
            buff[2] = static_cast<uint8_t>((value >> 16) & 0xFFu);
            buff[1] = static_cast<uint8_t>((value >> 8) & 0xFFu);
            buff[0] = static_cast<uint8_t>((value >> 0) & 0xFFu);
            return sizeof(uint32_t);
        }

        inline uint8_t encode_float_little_endian(const float value, uint8_t *buff)
        {
            uint32_t uv;
            memcpy(&uv, &value, sizeof(uint32_t));
            encode_32_bits_little_endian(uv, buff);
            return sizeof(float);
        }

#if SCRUTINY_SUPPORT_64BITS
        inline uint8_t encode_64_bits_little_endian(const uint64_t value, uint8_t *buff)
        {
            buff[7] = static_cast<uint8_t>((value >> 56) & 0xFFu);
            buff[6] = static_cast<uint8_t>((value >> 48) & 0xFFu);
            buff[5] = static_cast<uint8_t>((value >> 40) & 0xFFu);
            buff[4] = static_cast<uint8_t>((value >> 32) & 0xFFu);
            buff[3] = static_cast<uint8_t>((value >> 24) & 0xFFu);
            buff[2] = static_cast<uint8_t>((value >> 16) & 0xFFu);
            buff[1] = static_cast<uint8_t>((value >> 8) & 0xFFu);
            buff[0] = static_cast<uint8_t>((value >> 0) & 0xFFu);
            return sizeof(uint64_t);
        }
        inline uint64_t decode_64_bits_big_endian(const uint8_t *buff)
        {
            uint_fast64_t v = 0;
            v |= ((static_cast<uint_fast64_t>(buff[0]) << 56) & 0xFF00000000000000u);
            v |= ((static_cast<uint_fast64_t>(buff[1]) << 48) & 0x00FF000000000000u);
            v |= ((static_cast<uint_fast64_t>(buff[2]) << 40) & 0x0000FF0000000000u);
            v |= ((static_cast<uint_fast64_t>(buff[3]) << 32) & 0x000000FF00000000u);
            v |= ((static_cast<uint_fast64_t>(buff[4]) << 24) & 0x00000000FF000000u);
            v |= ((static_cast<uint_fast64_t>(buff[5]) << 16) & 0x0000000000FF0000u);
            v |= ((static_cast<uint_fast64_t>(buff[6]) << 8) & 0x000000000000FF00u);
            v |= ((static_cast<uint_fast64_t>(buff[7]) << 0) & 0x00000000000000FFu);
            return static_cast<uint64_t>(v);
        }
#endif

        inline uint32_t decode_32_bits_big_endian(const uint8_t *buff)
        {
            uint_fast32_t v = 0;
            v |= ((static_cast<uint_fast32_t>(buff[0]) << 24) & 0xFF000000u);
            v |= ((static_cast<uint_fast32_t>(buff[1]) << 16) & 0x00FF0000u);
            v |= ((static_cast<uint_fast32_t>(buff[2]) << 8) & 0x0000FF00u);
            v |= ((static_cast<uint_fast32_t>(buff[3]) << 0) & 0x000000FFu);
            return static_cast<uint32_t>(v);
        }

        inline float decode_float_big_endian(const uint8_t *buff)
        {
            static_assert(sizeof(float) == 4, "Expect float to be 4 bytes");
            const uint32_t temp = decode_32_bits_big_endian(buff);
            float v;
            memcpy(&v, &temp, sizeof(float));
            return v;
        }

        inline uint16_t decode_16_bits_big_endian(const uint8_t *buff)
        {
            uint_fast16_t v = 0;
            v |= ((static_cast<uint_fast16_t>(buff[0]) << 8) & 0xFF00u);
            v |= ((static_cast<uint_fast16_t>(buff[1]) << 0) & 0x00FFu);
            return static_cast<uint16_t>(v);
        }

#if SCRUTINY_SUPPORT_64BITS
        inline uint64_t decode_64_bits_little_endian(const uint8_t *buff)
        {
            uint_fast64_t v = 0;
            v |= ((static_cast<uint_fast64_t>(buff[7]) << 56) & 0xFF00000000000000u);
            v |= ((static_cast<uint_fast64_t>(buff[6]) << 48) & 0x00FF000000000000u);
            v |= ((static_cast<uint_fast64_t>(buff[5]) << 40) & 0x0000FF0000000000u);
            v |= ((static_cast<uint_fast64_t>(buff[4]) << 32) & 0x000000FF00000000u);
            v |= ((static_cast<uint_fast64_t>(buff[3]) << 24) & 0x00000000FF000000u);
            v |= ((static_cast<uint_fast64_t>(buff[2]) << 16) & 0x0000000000FF0000u);
            v |= ((static_cast<uint_fast64_t>(buff[1]) << 8) & 0x000000000000FF00u);
            v |= ((static_cast<uint_fast64_t>(buff[0]) << 0) & 0x00000000000000FFu);
            return static_cast<uint64_t>(v);
        }
#endif
        inline uint32_t decode_32_bits_little_endian(const uint8_t *buff)
        {
            uint_fast32_t v = 0;
            v |= ((static_cast<uint_fast32_t>(buff[3]) << 24) & 0xFF000000u);
            v |= ((static_cast<uint_fast32_t>(buff[2]) << 16) & 0x00FF0000u);
            v |= ((static_cast<uint_fast32_t>(buff[1]) << 8) & 0x0000FF00u);
            v |= ((static_cast<uint_fast32_t>(buff[0]) << 0) & 0x000000FFu);
            return static_cast<uint32_t>(v);
        }

        inline float decode_float_little_endian(const uint8_t *buff)
        {
            static_assert(sizeof(float) == 4, "Expect float to be 4 bytes");
            const uint32_t temp = decode_32_bits_little_endian(buff);
            float v;
            memcpy(&v, &temp, sizeof(float));
            return v;
        }

        inline uint16_t decode_16_bits_little_endian(const uint8_t *buff)
        {
            uint_fast16_t v = 0;
            v |= ((static_cast<uint_fast16_t>(buff[1]) << 8) & 0xFF00u);
            v |= ((static_cast<uint_fast16_t>(buff[0]) << 0) & 0x00FFu);
            return static_cast<uint16_t>(v);
        }

        uint8_t decode_address_big_endian(const uint8_t *buf, uintptr_t *addr);
        uint8_t encode_address_big_endian(const void *ptr, uint8_t *buf);
        uint8_t encode_address_big_endian(const uintptr_t addr, uint8_t *buf);

        uint8_t encode_anytype_big_endian(const scrutiny::AnyType *val, const VariableType vartype, uint8_t *buffer);
        uint8_t encode_anytype_big_endian(const scrutiny::AnyType *val, const uint8_t typesize, uint8_t *buffer);
    }
}

#endif //___SCRUTINY_COMMON_CODECS_H___