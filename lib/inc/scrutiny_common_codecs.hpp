//    scrutiny_common_codecs.hpp
//        Some codecs function used across the project
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_COMMON_CODECS_H___
#define ___SCRUTINY_COMMON_CODECS_H___

#include "scrutiny_setup.hpp"
#include "scrutiny_types.hpp"
#include <stdint.h>
#include <string.h>
#include <limits.h>

namespace scrutiny
{
    // These constants exists to deal with target with char > 8.
    // Communication buffers are u8. To know the number of u8 we need, we cannot rely on sizeof
    // ex. sizeof(uint16_t) = 1 if CHAR_BIT=16. But we need to char to store that value (we don't pack the buffer)
    static SCRUTINY_CONSTEXPR unsigned int ADDR_SIZE_U8 = sizeof(void *) * CHAR_BIT/8;
    static SCRUTINY_CONSTEXPR unsigned int FLOAT_SIZE_U8 = 4;
    static SCRUTINY_CONSTEXPR unsigned int UINT8_SIZE_U8 = 1;
    static SCRUTINY_CONSTEXPR unsigned int UINT16_SIZE_U8 = 2;
    static SCRUTINY_CONSTEXPR unsigned int UINT32_SIZE_U8 = 4;
    static SCRUTINY_CONSTEXPR unsigned int UINT64_SIZE_U8 = 8;
    namespace codecs
    {

#if SCRUTINY_HAS_CPP11
        static_assert(sizeof(float)*CHAR_BIT/8 == FLOAT_SIZE_U8, "Expect float to be 32bits");
#endif

        inline uint_least8_t encode_8_bits(uint_least8_t const value, unsigned char *const buff)
        {
            buff[0] = value & 0xFF;
            return UINT8_SIZE_U8;
        }
#if SCRUTINY_HAS_CPP11
        template <class T> inline uint_least8_t encode_8_bits(T const value, unsigned char *buff) = delete;
#endif

        inline uint_least8_t encode_16_bits_big_endian(uint16_t const value, unsigned char *buff)
        {
            buff[0] = static_cast<unsigned char>((value >> 8) & 0xFF);
            buff[1] = static_cast<unsigned char>((value >> 0) & 0xFF);
            return UINT16_SIZE_U8;
        }
#if SCRUTINY_HAS_CPP11
        template <class T> inline uint_least8_t encode_16_bits_big_endian(T const value, unsigned char *buff) = delete;
#endif

        inline uint_least8_t encode_32_bits_big_endian(uint32_t const value, unsigned char *buff)
        {
            buff[0] = static_cast<unsigned char>((value >> 24) & 0xFFu);
            buff[1] = static_cast<unsigned char>((value >> 16) & 0xFFu);
            buff[2] = static_cast<unsigned char>((value >> 8) & 0xFFu);
            buff[3] = static_cast<unsigned char>((value >> 0) & 0xFFu);
            return UINT32_SIZE_U8;
        }
#if SCRUTINY_HAS_CPP11
        template <class T> inline uint_least8_t encode_32_bits_big_endian(T const value, unsigned char *buff) = delete;
#endif

        inline uint_least8_t encode_float_big_endian(float const value, unsigned char *buff)
        {
            uint32_t uv;
            memcpy(&uv, &value, sizeof(uint32_t));
            encode_32_bits_big_endian(uv, buff);
            return FLOAT_SIZE_U8;
        }
#if SCRUTINY_HAS_CPP11
        template <class T> inline uint_least8_t encode_float_big_endian(T const value, unsigned char *buff) = delete;
#endif

#if SCRUTINY_SUPPORT_64BITS
        inline uint_least8_t encode_64_bits_big_endian(uint64_t const value, unsigned char *buff)
        {
            buff[0] = static_cast<unsigned char>((value >> 56) & 0xFFu);
            buff[1] = static_cast<unsigned char>((value >> 48) & 0xFFu);
            buff[2] = static_cast<unsigned char>((value >> 40) & 0xFFu);
            buff[3] = static_cast<unsigned char>((value >> 32) & 0xFFu);
            buff[4] = static_cast<unsigned char>((value >> 24) & 0xFFu);
            buff[5] = static_cast<unsigned char>((value >> 16) & 0xFFu);
            buff[6] = static_cast<unsigned char>((value >> 8) & 0xFFu);
            buff[7] = static_cast<unsigned char>((value >> 0) & 0xFFu);

            return UINT64_SIZE_U8;
        }
#if SCRUTINY_HAS_CPP11
        template <class T> inline uint_least8_t encode_64_bits_big_endian(T const value, unsigned char *buff) = delete;
#endif
#endif

        inline uint_least8_t encode_16_bits_little_endian(uint16_t const value, unsigned char *buff)
        {
            buff[1] = static_cast<unsigned char>((value >> 8) & 0xFFu);
            buff[0] = static_cast<unsigned char>((value >> 0) & 0xFFu);
            return UINT16_SIZE_U8;
        }
#if SCRUTINY_HAS_CPP11
        template <class T> inline uint_least8_t encode_16_bits_little_endian(T const value, unsigned char *buff) = delete;
#endif

        inline uint_least8_t encode_32_bits_little_endian(uint32_t const value, unsigned char *buff)
        {
            buff[3] = static_cast<unsigned char>((value >> 24) & 0xFFu);
            buff[2] = static_cast<unsigned char>((value >> 16) & 0xFFu);
            buff[1] = static_cast<unsigned char>((value >> 8) & 0xFFu);
            buff[0] = static_cast<unsigned char>((value >> 0) & 0xFFu);
            return UINT32_SIZE_U8;
        }
#if SCRUTINY_HAS_CPP11
        template <class T> inline uint_least8_t encode_32_bits_little_endian(T const value, unsigned char *buff) = delete;
#endif

        inline uint_least8_t encode_float_little_endian(float const value, unsigned char *buff)
        {
            uint32_t uv;
            memcpy(&uv, &value, sizeof(uint32_t));
            encode_32_bits_little_endian(uv, buff);
            return FLOAT_SIZE_U8;
        }
#if SCRUTINY_HAS_CPP11
        template <class T> inline uint_least8_t encode_float_little_endian(T const value, unsigned char *buff) = delete;
#endif

#if SCRUTINY_SUPPORT_64BITS
        inline uint_least8_t encode_64_bits_little_endian(uint64_t const value, unsigned char *buff)
        {
            buff[7] = static_cast<unsigned char>((value >> 56) & 0xFFu);
            buff[6] = static_cast<unsigned char>((value >> 48) & 0xFFu);
            buff[5] = static_cast<unsigned char>((value >> 40) & 0xFFu);
            buff[4] = static_cast<unsigned char>((value >> 32) & 0xFFu);
            buff[3] = static_cast<unsigned char>((value >> 24) & 0xFFu);
            buff[2] = static_cast<unsigned char>((value >> 16) & 0xFFu);
            buff[1] = static_cast<unsigned char>((value >> 8) & 0xFFu);
            buff[0] = static_cast<unsigned char>((value >> 0) & 0xFFu);
            return UINT64_SIZE_U8;
        }
#if SCRUTINY_HAS_CPP11
        template <class T> inline uint_least8_t encode_64_bits_little_endian(T const value, unsigned char *buff) = delete;
#endif
        inline uint64_t decode_64_bits_big_endian(unsigned char const *const buff)
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

        inline uint32_t decode_32_bits_big_endian(unsigned char const *const buff)
        {
            uint_fast32_t v = 0;
            v |= ((static_cast<uint_fast32_t>(buff[0]) << 24) & 0xFF000000u);
            v |= ((static_cast<uint_fast32_t>(buff[1]) << 16) & 0x00FF0000u);
            v |= ((static_cast<uint_fast32_t>(buff[2]) << 8) & 0x0000FF00u);
            v |= ((static_cast<uint_fast32_t>(buff[3]) << 0) & 0x000000FFu);
            return static_cast<uint32_t>(v);
        }

        inline float decode_float_big_endian(unsigned char const *const buff)
        {
            SCRUTINY_STATIC_ASSERT(sizeof(float)*CHAR_BIT/8 == 4, "Expect float to be 32 bits");
            uint32_t const temp = decode_32_bits_big_endian(buff);
            float v;
            memcpy(&v, &temp, sizeof(float));
            return v;
        }

        inline uint16_t decode_16_bits_big_endian(unsigned char const *const buff)
        {
            uint_fast16_t v = 0;
            v |= ((static_cast<uint_fast16_t>(buff[0]) << 8) & 0xFF00u);
            v |= ((static_cast<uint_fast16_t>(buff[1]) << 0) & 0x00FFu);
            return static_cast<uint16_t>(v);
        }

        inline uint_least8_t decode_8_bits(unsigned char const *const buff)
        {
            return static_cast<uint_least8_t>(buff[0] & 0xFF);
        }

#if SCRUTINY_SUPPORT_64BITS
        inline uint64_t decode_64_bits_little_endian(unsigned char const *const buff)
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
        inline uint32_t decode_32_bits_little_endian(unsigned char const *const buff)
        {
            uint_fast32_t v = 0;
            v |= ((static_cast<uint_fast32_t>(buff[3]) << 24) & 0xFF000000u);
            v |= ((static_cast<uint_fast32_t>(buff[2]) << 16) & 0x00FF0000u);
            v |= ((static_cast<uint_fast32_t>(buff[1]) << 8) & 0x0000FF00u);
            v |= ((static_cast<uint_fast32_t>(buff[0]) << 0) & 0x000000FFu);
            return static_cast<uint32_t>(v);
        }

        inline float decode_float_little_endian(unsigned char const *const buff)
        {
            uint32_t const temp = decode_32_bits_little_endian(buff);
            float v;
            memcpy(&v, &temp, sizeof(float));
            return v;
        }

        inline uint16_t decode_16_bits_little_endian(unsigned char const *const buff)
        {
            uint_fast16_t v = 0;
            v |= ((static_cast<uint_fast16_t>(buff[1]) << 8) & 0xFF00u);
            v |= ((static_cast<uint_fast16_t>(buff[0]) << 0) & 0x00FFu);
            return static_cast<uint16_t>(v);
        }

        uint_least8_t decode_address_big_endian(unsigned char const *const buf, uintptr_t *const addr);
        uint_least8_t encode_address_big_endian(void const *const addr, unsigned char *const buf);
        uint_least8_t encode_address_big_endian(uintptr_t const addr, unsigned char *const buf);

        uint_least8_t encode_anytype_big_endian(scrutiny::AnyType const *const val, VariableType::E const vartype, unsigned char *const buffer);
        uint_least8_t encode_anytype_big_endian(scrutiny::AnyType const *const val, uint_least8_t const typesize, unsigned char *const buffer);
    } // namespace codecs
} // namespace scrutiny

#endif //___SCRUTINY_COMMON_CODECS_H___
