#ifndef ___SCRUTINY_COMMON_CODECS_H___
#define ___SCRUTINY_COMMON_CODECS_H___

#include <stdint.h>

namespace scrutiny
{
    namespace codecs
    {
        inline void encode_16_bits_big_endian(const uint16_t value, uint8_t *buff)
        {
            buff[0] = static_cast<uint8_t>((value >> 8) & 0xFF);
            buff[1] = static_cast<uint8_t>((value >> 0) & 0xFF);
        }

        inline void encode_32_bits_big_endian(const uint32_t value, uint8_t *buff)
        {
            buff[0] = static_cast<uint8_t>((value >> 24) & 0xFFu);
            buff[1] = static_cast<uint8_t>((value >> 16) & 0xFFu);
            buff[2] = static_cast<uint8_t>((value >> 8) & 0xFFu);
            buff[3] = static_cast<uint8_t>((value >> 0) & 0xFFu);
        }

        inline void encode_64_bits_big_endian(const uint64_t value, uint8_t *buff)
        {
            buff[0] = static_cast<uint8_t>((value >> 56) & 0xFFu);
            buff[1] = static_cast<uint8_t>((value >> 48) & 0xFFu);
            buff[2] = static_cast<uint8_t>((value >> 40) & 0xFFu);
            buff[3] = static_cast<uint8_t>((value >> 32) & 0xFFu);
            buff[4] = static_cast<uint8_t>((value >> 24) & 0xFFu);
            buff[5] = static_cast<uint8_t>((value >> 16) & 0xFFu);
            buff[6] = static_cast<uint8_t>((value >> 8) & 0xFFu);
            buff[7] = static_cast<uint8_t>((value >> 0) & 0xFFu);
        }

        inline void encode_16_bits_little_endian(const uint16_t value, uint8_t *buff)
        {
            buff[1] = static_cast<uint8_t>((value >> 8) & 0xFFu);
            buff[0] = static_cast<uint8_t>((value >> 0) & 0xFFu);
        }

        inline void encode_32_bits_little_endian(const uint32_t value, uint8_t *buff)
        {
            buff[3] = static_cast<uint8_t>((value >> 24) & 0xFFu);
            buff[2] = static_cast<uint8_t>((value >> 16) & 0xFFu);
            buff[1] = static_cast<uint8_t>((value >> 8) & 0xFFu);
            buff[0] = static_cast<uint8_t>((value >> 0) & 0xFFu);
        }

        inline void encode_64_bits_little_endian(const uint64_t value, uint8_t *buff)
        {
            buff[7] = static_cast<uint8_t>((value >> 56) & 0xFFu);
            buff[6] = static_cast<uint8_t>((value >> 48) & 0xFFu);
            buff[5] = static_cast<uint8_t>((value >> 40) & 0xFFu);
            buff[4] = static_cast<uint8_t>((value >> 32) & 0xFFu);
            buff[3] = static_cast<uint8_t>((value >> 24) & 0xFFu);
            buff[2] = static_cast<uint8_t>((value >> 16) & 0xFFu);
            buff[1] = static_cast<uint8_t>((value >> 8) & 0xFFu);
            buff[0] = static_cast<uint8_t>((value >> 0) & 0xFFu);
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

        inline uint32_t decode_32_bits_big_endian(const uint8_t *buff)
        {
            uint_fast32_t v = 0;
            v |= ((static_cast<uint_fast32_t>(buff[0]) << 24) & 0xFF000000u);
            v |= ((static_cast<uint_fast32_t>(buff[1]) << 16) & 0x00FF0000u);
            v |= ((static_cast<uint_fast32_t>(buff[2]) << 8) & 0x0000FF00u);
            v |= ((static_cast<uint_fast32_t>(buff[3]) << 0) & 0x000000FFu);
            return static_cast<uint32_t>(v);
        }

        inline uint16_t decode_16_bits_big_endian(const uint8_t *buff)
        {
            uint_fast16_t v = 0;
            v |= ((static_cast<uint_fast16_t>(buff[0]) << 8) & 0xFF00u);
            v |= ((static_cast<uint_fast16_t>(buff[1]) << 0) & 0x00FFu);
            return static_cast<uint16_t>(v);
        }

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

        inline uint32_t decode_32_bits_little_endian(const uint8_t *buff)
        {
            uint_fast32_t v = 0;
            v |= ((static_cast<uint_fast32_t>(buff[3]) << 24) & 0xFF000000u);
            v |= ((static_cast<uint_fast32_t>(buff[2]) << 16) & 0x00FF0000u);
            v |= ((static_cast<uint_fast32_t>(buff[1]) << 8) & 0x0000FF00u);
            v |= ((static_cast<uint_fast32_t>(buff[0]) << 0) & 0x000000FFu);
            return static_cast<uint32_t>(v);
        }

        inline uint16_t decode_16_bits_little_endian(const uint8_t *buff)
        {
            uint_fast16_t v = 0;
            v |= ((static_cast<uint_fast16_t>(buff[1]) << 8) & 0xFF00u);
            v |= ((static_cast<uint_fast16_t>(buff[0]) << 0) & 0x00FFu);
            return static_cast<uint16_t>(v);
        }

        uint8_t decode_address_big_endian(uint8_t *buf, uintptr_t *addr);
        uint8_t encode_address_big_endian(uint8_t *buf, void *ptr);
        uint8_t encode_address_big_endian(uint8_t *buf, uintptr_t addr);
    }
}

#endif //___SCRUTINY_COMMON_CODECS_H___