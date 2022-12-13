//    scrutiny_tools.h
//        Some simple tools used across the project
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef __SCRUTINY_TOOLS_H__
#define __SCRUTINY_TOOLS_H__

#include <stdlib.h>
#include <stdint.h>

#include "scrutiny_types.hpp"

#include <stdint.h>

namespace scrutiny
{
    namespace tools
    {
        inline AddressRange make_address_range(uintptr_t start, uintptr_t end)
        {
            return {reinterpret_cast<void *>(start), reinterpret_cast<void *>(end)};
        }

        inline AddressRange make_address_range(void *start, void *end)
        {
            return {start, end};
        }

        inline uint8_t get_type_size(VariableType v)
        {
            if (v == VariableType::unknown)
            {
                return 0;
            }

            return 1 << (static_cast<uint8_t>(v) & 0xF);
        }

        inline size_t strnlen(const char *s, size_t maxlen)
        {
            size_t n = 0;
            for (; n < maxlen; n++)
            {
                if (s[n] == '\0')
                {
                    break;
                }
            }

            return n;
        }

        // strncpy is not standard + non-standard implementation does shenanigans.
        inline size_t strncpy(char *dst, const char *src, size_t maxlen)
        {
            size_t n = 0;
            for (; n < maxlen; n++)
            {
                dst[n] = src[n];
                if (src[n] == '\0')
                {
                    break;
                }
            }

            if (n >= maxlen)
            {
                dst[maxlen - 1] = '\0';
            }
            return n;
        }

        // Taken from https://gist.github.com/tommyettinger/46a874533244883189143505d203312
        class MulBerry32PRNG
        {
        public:
            MulBerry32PRNG(uint32_t seed) : m_state(seed) {}
            MulBerry32PRNG() : m_state(0) {}

            void seed(uint32_t _seed) { m_state = _seed; }
            uint32_t get()
            {
                uint32_t z = m_state += 0x6D2B79F5;
                z = (z ^ z >> 15) * (1 | z);
                z ^= z + (z ^ z >> 7) * (61 | z);
                return z ^ z >> 14;
            }

        private:
            uint32_t m_state;
        };
    }
}

#endif //__SCRUTINY_TOOLS_H__