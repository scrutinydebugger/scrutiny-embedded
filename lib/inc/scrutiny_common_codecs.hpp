//    scrutiny_common_codecs.hpp
//        Some codecs function used across the project
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_COMMON_CODECS_H___
#define ___SCRUTINY_COMMON_CODECS_H___

#include "scrutiny_setup.hpp"
#include "scrutiny_types.hpp"
#include <stdint.h>
#include <string.h>

namespace scrutiny
{
    namespace codecs
    {
        // =============================
        // =========== ENCODE ==========
        // =============================
        inline uint_least8_t encode_8_bits_8bits(uint_least8_t const value, unsigned char *const buff)
        {
            buff[0] = static_cast<unsigned char>(value & 0xFFu);
            return 1;
        }

        // ===== 16 bits =====
        inline uint_least8_t encode_16_bits_big_endian_8bits(uint16_t const value, unsigned char *const buff)
        {
            buff[0] = static_cast<unsigned char>((value >> 8) & 0xFFu);
            buff[1] = static_cast<unsigned char>((value >> 0) & 0xFFu);
            return 2;
        }

        inline uint_least8_t encode_16_bits_little_endian_8bits(uint16_t const value, unsigned char *const buff)
        {
            buff[1] = static_cast<unsigned char>((value >> 8) & 0xFFu);
            buff[0] = static_cast<unsigned char>((value >> 0) & 0xFFu);
            return 2;
        }

        inline uint_least8_t encode_16_bits_big_endian_char(uint16_t const value, unsigned char *const buff)
        {
#if CHAR_BIT == 8
            return encode_16_bits_big_endian_8bits(value, buff);
#elif CHAR_BIT == 16
            buff[0] = static_cast<unsigned char>(value);
            return 1;
#else
#error
#endif
        }

        inline uint_least8_t encode_16_bits_little_endian_char(uint16_t const value, unsigned char *const buff)
        {
#if CHAR_BIT == 8
            return encode_16_bits_little_endian_8bits(value, buff);
#elif CHAR_BIT == 16
            buff[0] = static_cast<unsigned char>(value);
            return 1;
#else
#error
#endif
        }

        // ===== 32 bits =====
        inline uint_least8_t encode_32_bits_big_endian_8bits(uint32_t const value, unsigned char *const buff)
        {
            buff[0] = static_cast<unsigned char>((value >> 24) & 0xFFu);
            buff[1] = static_cast<unsigned char>((value >> 16) & 0xFFu);
            buff[2] = static_cast<unsigned char>((value >> 8) & 0xFFu);
            buff[3] = static_cast<unsigned char>((value >> 0) & 0xFFu);
            return 4;
        }

        inline uint_least8_t encode_32_bits_little_endian_8bits(uint32_t const value, unsigned char *const buff)
        {
            buff[3] = static_cast<unsigned char>((value >> 24) & 0xFFu);
            buff[2] = static_cast<unsigned char>((value >> 16) & 0xFFu);
            buff[1] = static_cast<unsigned char>((value >> 8) & 0xFFu);
            buff[0] = static_cast<unsigned char>((value >> 0) & 0xFFu);
            return 4;
        }

        inline uint_least8_t encode_32_bits_big_endian_char(uint32_t const value, unsigned char *const buff)
        {
#if CHAR_BIT == 8
            return encode_32_bits_big_endian_8bits(value, buff);
#elif CHAR_BIT == 16
            buff[0] = static_cast<unsigned char>((value >> 16) & 0xFFFF);
            buff[1] = static_cast<unsigned char>(value & 0xFFFF);
            return 2;
#else
#error
#endif
        }

        inline uint_least8_t encode_32_bits_little_endian_char(uint32_t const value, unsigned char *const buff)
        {
#if CHAR_BIT == 8
            return encode_32_bits_little_endian_8bits(value, buff);
#elif CHAR_BIT == 16
            buff[0] = static_cast<unsigned char>(value & 0xFFFF);
            buff[1] = static_cast<unsigned char>((value >> 16) & 0xFFFF);
            return 2;
#else
#error
#endif
        }

        // ==== float ====
        inline uint_least8_t encode_float_big_endian_8bits(float const value, unsigned char *const buff)
        {
            SCRUTINY_STATIC_ASSERT(sizeof(uint32_t) == sizeof(float), "Expect float to be 32 bits");
            uint32_t uv;
            memcpy(&uv, &value, sizeof(uint32_t));
            encode_32_bits_big_endian_8bits(uv, buff);
            return 4;
        }

        inline uint_least8_t encode_float_little_endian_8bits(float const value, unsigned char *const buff)
        {
            SCRUTINY_STATIC_ASSERT(sizeof(uint32_t) == sizeof(float), "Expect float to be 32 bits");
            uint32_t uv;
            memcpy(&uv, &value, sizeof(uint32_t));
            encode_32_bits_little_endian_8bits(uv, buff);
            return 4;
        }

        inline uint_least8_t encode_float_big_endian_char(float const value, unsigned char *const buff)
        {
#if CHAR_BIT == 8
            return encode_float_big_endian_8bits(value, buff);
#elif CHAR_BIT == 16
            SCRUTINY_STATIC_ASSERT(sizeof(uint32_t) == sizeof(float), "Expect float to be 32 bits");
            uint32_t uv;
            memcpy(&uv, &value, sizeof(uint32_t));
            encode_32_bits_big_endian_char(uv, buff);
            return 2;
#else
#error
#endif
        }

        inline uint_least8_t encode_float_little_endian_char(float const value, unsigned char *const buff)
        {
#if CHAR_BIT == 8
            return encode_float_little_endian_8bits(value, buff);
#elif CHAR_BIT == 16
            SCRUTINY_STATIC_ASSERT(sizeof(uint32_t) == sizeof(float), "Expect float to be 32 bits");
            uint32_t uv;
            memcpy(&uv, &value, sizeof(uint32_t));
            encode_32_bits_little_endian_char(uv, buff);
            return 2;
#else
#error
#endif
        }

#if SCRUTINY_SUPPORT_64BITS
        inline uint_least8_t encode_64_bits_big_endian_8bits(uint64_t const value, unsigned char *const buff)
        {
            buff[0] = static_cast<unsigned char>((value >> 56) & 0xFFu);
            buff[1] = static_cast<unsigned char>((value >> 48) & 0xFFu);
            buff[2] = static_cast<unsigned char>((value >> 40) & 0xFFu);
            buff[3] = static_cast<unsigned char>((value >> 32) & 0xFFu);
            buff[4] = static_cast<unsigned char>((value >> 24) & 0xFFu);
            buff[5] = static_cast<unsigned char>((value >> 16) & 0xFFu);
            buff[6] = static_cast<unsigned char>((value >> 8) & 0xFFu);
            buff[7] = static_cast<unsigned char>((value >> 0) & 0xFFu);

            return 8;
        }

        inline uint_least8_t encode_64_bits_little_endian_8bits(uint64_t const value, unsigned char *const buff)
        {
            buff[7] = static_cast<unsigned char>((value >> 56) & 0xFFu);
            buff[6] = static_cast<unsigned char>((value >> 48) & 0xFFu);
            buff[5] = static_cast<unsigned char>((value >> 40) & 0xFFu);
            buff[4] = static_cast<unsigned char>((value >> 32) & 0xFFu);
            buff[3] = static_cast<unsigned char>((value >> 24) & 0xFFu);
            buff[2] = static_cast<unsigned char>((value >> 16) & 0xFFu);
            buff[1] = static_cast<unsigned char>((value >> 8) & 0xFFu);
            buff[0] = static_cast<unsigned char>((value >> 0) & 0xFFu);
            return 8;
        }

        inline uint_least8_t encode_64_bits_big_endian_char(uint64_t const value, unsigned char *const buff)
        {
#if CHAR_BIT == 8
            return encode_64_bits_big_endian_8bits(value, buff);
#elif CHAR_BIT == 16
            buff[0] = static_cast<unsigned char>((value >> 48) & 0xFFFF);
            buff[1] = static_cast<unsigned char>((value >> 32) & 0xFFFF);
            buff[2] = static_cast<unsigned char>((value >> 16) & 0xFFFF);
            buff[3] = static_cast<unsigned char>(value & 0xFFFF);
            return 4;
#else
#error
#endif
        }

        inline uint_least8_t encode_64_bits_little_endian_char(uint64_t const value, unsigned char *const buff)
        {
#if CHAR_BIT == 8
            return encode_64_bits_little_endian_8bits(value, buff);
#elif CHAR_BIT == 16
            buff[0] = static_cast<unsigned char>(value & 0xFFFF);
            buff[1] = static_cast<unsigned char>((value >> 16) & 0xFFFF);
            buff[2] = static_cast<unsigned char>((value >> 32) & 0xFFFF);
            buff[3] = static_cast<unsigned char>((value >> 48) & 0xFFFF);
            return 4;
#else
#error
#endif
        }
#endif

        // =============================
        // =========== DECODE ==========
        // =============================

#if SCRUTINY_SUPPORT_64BITS
        inline uint64_t decode_64_bits_big_endian_8bits(unsigned char const *const buff)
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

        inline uint64_t decode_64_bits_little_endian_8bits(unsigned char const *const buff)
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

        inline uint64_t decode_64_bits_big_endian_char(unsigned char const *const buff)
        {
#if CHAR_BIT == 8
            return decode_64_bits_big_endian_8bits(buff);
#elif CHAR_BIT == 16
            uint_fast64_t v = 0;
            v |= ((static_cast<uint_fast64_t>(buff[0]) << 48) & 0xFFFF000000000000u);
            v |= ((static_cast<uint_fast64_t>(buff[1]) << 32) & 0x0000FFFF00000000u);
            v |= ((static_cast<uint_fast64_t>(buff[2]) << 16) & 0x00000000FFFF0000u);
            v |= ((static_cast<uint_fast64_t>(buff[3]) << 0) & 0x000000000000FFFFu);
            return static_cast<uint64_t>(v);
#else
#error
#endif
        }

        inline uint64_t decode_64_bits_little_endian_char(unsigned char const *const buff)
        {
#if CHAR_BIT == 8
            return decode_64_bits_little_endian_8bits(buff);
#elif CHAR_BIT == 16
            uint_fast64_t v = 0;
            v |= ((static_cast<uint_fast64_t>(buff[0]) << 0) & 0x000000000000FFFFu);
            v |= ((static_cast<uint_fast64_t>(buff[1]) << 16) & 0x00000000FFFF0000u);
            v |= ((static_cast<uint_fast64_t>(buff[2]) << 32) & 0x0000FFFF00000000u);
            v |= ((static_cast<uint_fast64_t>(buff[3]) << 48) & 0xFFFF000000000000u);
            return static_cast<uint64_t>(v);
#else
#error
#endif
        }
#endif

        // ==== 32 bits ====
        inline uint32_t decode_32_bits_little_endian_8bits(unsigned char const *const buff)
        {
            uint_fast32_t v = 0;
            v |= ((static_cast<uint_fast32_t>(buff[3]) << 24) & 0xFF000000u);
            v |= ((static_cast<uint_fast32_t>(buff[2]) << 16) & 0x00FF0000u);
            v |= ((static_cast<uint_fast32_t>(buff[1]) << 8) & 0x0000FF00u);
            v |= ((static_cast<uint_fast32_t>(buff[0]) << 0) & 0x000000FFu);
            return static_cast<uint32_t>(v);
        }

        inline uint32_t decode_32_bits_big_endian_8bits(unsigned char const *const buff)
        {
            uint_fast32_t v = 0;
            v |= ((static_cast<uint_fast32_t>(buff[0]) << 24) & 0xFF000000u);
            v |= ((static_cast<uint_fast32_t>(buff[1]) << 16) & 0x00FF0000u);
            v |= ((static_cast<uint_fast32_t>(buff[2]) << 8) & 0x0000FF00u);
            v |= ((static_cast<uint_fast32_t>(buff[3]) << 0) & 0x000000FFu);
            return static_cast<uint32_t>(v);
        }

        inline uint32_t decode_32_bits_big_endian_char(unsigned char const *const buff)
        {
#if CHAR_BIT == 8
            return decode_32_bits_big_endian_8bits(buff);
#elif CHAR_BIT == 16
            uint_fast32_t v = 0;
            v |= ((static_cast<uint_fast32_t>(buff[0]) << 16) & 0xFFFF0000u);
            v |= ((static_cast<uint_fast32_t>(buff[1]) << 0) & 0x0000FFFFu);
            return static_cast<uint32_t>(v);
#else
#error
#endif
        }

        inline uint32_t decode_32_bits_little_endian_char(unsigned char const *const buff)
        {
#if CHAR_BIT == 8
            return decode_32_bits_little_endian_8bits(buff);
#elif CHAR_BIT == 16
            uint_fast32_t v = 0;
            v |= ((static_cast<uint_fast32_t>(buff[0]) << 0) & 0x0000FFFFu);
            v |= ((static_cast<uint_fast32_t>(buff[1]) << 16) & 0xFFFF0000u);
            return static_cast<uint32_t>(v);
#else
#error
#endif
        }

        // ==== float ====
        inline float decode_float_little_endian_8bits(unsigned char const *const buff)
        {
            SCRUTINY_STATIC_ASSERT(sizeof(uint32_t) == sizeof(float), "Expect float to be 32 bits");
            uint32_t const temp = decode_32_bits_little_endian_8bits(buff);
            float v;
            memcpy(&v, &temp, sizeof(float));
            return v;
        }

        inline float decode_float_big_endian_8bits(unsigned char const *const buff)
        {
            SCRUTINY_STATIC_ASSERT(sizeof(uint32_t) == sizeof(float), "Expect float to be 32 bits");
            uint32_t const temp = decode_32_bits_big_endian_8bits(buff);
            float v;
            memcpy(&v, &temp, sizeof(float));
            return v;
        }

        inline float decode_float_big_endian_char(unsigned char const *const buff)
        {
#if CHAR_BIT == 8
            return decode_float_big_endian_8bits(buff);
#elif CHAR_BIT == 16
            SCRUTINY_STATIC_ASSERT(sizeof(uint32_t) == sizeof(float), "Expect float to be 32 bits");
            uint32_t const temp = decode_32_bits_big_endian_char(buff);
            float v;
            memcpy(&v, &temp, sizeof(float));
            return v;
#else
#error
#endif
        }

        inline float decode_float_little_endian_char(unsigned char const *const buff)
        {
#if CHAR_BIT == 8
            return decode_float_little_endian_8bits(buff);
#elif CHAR_BIT == 16
            SCRUTINY_STATIC_ASSERT(sizeof(uint32_t) == sizeof(float), "Expect float to be 32 bits");
            uint32_t const temp = decode_32_bits_little_endian_char(buff);
            float v;
            memcpy(&v, &temp, sizeof(float));
            return v;
#else
#error
#endif
        }

        // ==== 16 bits ====
        inline uint16_t decode_16_bits_little_endian_8bits(unsigned char const *const buff)
        {
            uint_fast16_t v = 0;
            v |= ((static_cast<uint_fast16_t>(buff[1]) << 8) & 0xFF00u);
            v |= ((static_cast<uint_fast16_t>(buff[0]) << 0) & 0x00FFu);
            return static_cast<uint16_t>(v);
        }

        inline uint16_t decode_16_bits_big_endian_8bits(unsigned char const *const buff)
        {
            uint_fast16_t v = 0;
            v |= ((static_cast<uint_fast16_t>(buff[0]) << 8) & 0xFF00u);
            v |= ((static_cast<uint_fast16_t>(buff[1]) << 0) & 0x00FFu);
            return static_cast<uint16_t>(v);
        }

        inline uint16_t decode_16_bits_big_endian_char(unsigned char const *const buff)
        {
#if CHAR_BIT == 8
            return decode_16_bits_big_endian_8bits(buff);
#elif CHAR_BIT == 16
            return static_cast<uint16_t>(buff[0]);
#else
#error
#endif
        }

        inline uint16_t decode_16_bits_little_endian_char(unsigned char const *const buff)
        {
#if CHAR_BIT == 8
            return decode_16_bits_little_endian_8bits(buff);
#elif CHAR_BIT == 16
            return static_cast<uint16_t>(buff[0]);
#else
#error
#endif
        }

        uint_least8_t decode_address_big_endian_8bits(unsigned char const *const buf, uintptr_t *const addr);
        uint_least8_t encode_address_big_endian_8bits(void const *const addr, unsigned char *const buf);
        uint_least8_t encode_address_big_endian_8bits(uintptr_t const addr, unsigned char *const buf);

        uint_least8_t encode_anytype_big_endian_8bits(
            scrutiny::AnyType const *const val,
            VariableType::eVariableType const vartype,
            unsigned char *const buffer);
        uint_least8_t encode_anytype_big_endian_8bits(scrutiny::AnyType const *const val, uint_least8_t const typesize, unsigned char *const buffer);
        uint_least8_t encode_anytype_big_endian_char(
            AnyType const *const val,
            VariableType::eVariableType const vartype,
            unsigned char *const buffer);

#if SCRUTINY_HAS_CPP11
        template <class T> inline uint_least8_t encode_8_bits_8bits(T const value, unsigned char *const buff) = delete;

        template <class T> inline uint_least8_t encode_16_bits_little_endian_8bits(T const value, unsigned char *const buff) = delete;
        template <class T> inline uint_least8_t encode_16_bits_big_endian_8bits(T const value, unsigned char *const buff) = delete;
        template <class T> inline uint_least8_t encode_16_bits_big_endian_char(T const value, unsigned char *const buff) = delete;
        template <class T> inline uint_least8_t encode_16_bits_little_endian_char(T const value, unsigned char *const buff) = delete;

        template <class T> inline uint_least8_t encode_32_bits_little_endian_8bits(T const value, unsigned char *const buff) = delete;
        template <class T> inline uint_least8_t encode_32_bits_big_endian_8bits(T const value, unsigned char *const buff) = delete;
        template <class T> inline uint_least8_t encode_32_bits_big_endian_char(T const value, unsigned char *const buff) = delete;
        template <class T> inline uint_least8_t encode_32_bits_little_endian_char(T const value, unsigned char *const buff) = delete;

        template <class T> inline uint_least8_t encode_float_little_endian_8bits(T const value, unsigned char *const buff) = delete;
        template <class T> inline uint_least8_t encode_float_big_endian_8bits(T const value, unsigned char *const buff) = delete;
        template <class T> inline uint_least8_t encode_float_big_endian_char(T const value, unsigned char *const buff) = delete;
        template <class T> inline uint_least8_t encode_float_little_endian_char(T const value, unsigned char *const buff) = delete;

#if SCRUTINY_SUPPORT_64BITS
        template <class T> inline uint_least8_t encode_64_bits_big_endian_8bits(T const value, unsigned char *const buff) = delete;
        template <class T> inline uint_least8_t encode_64_bits_little_endian_8bits(T const value, unsigned char *const buff) = delete;
        template <class T> inline uint_least8_t encode_64_bits_big_endian_char(T const value, unsigned char *const buff) = delete;
        template <class T> inline uint_least8_t encode_64_bits_little_endian_char(T const value, unsigned char *const buff) = delete;
#endif
#endif

    } // namespace codecs
} // namespace scrutiny

#endif //___SCRUTINY_COMMON_CODECS_H___
