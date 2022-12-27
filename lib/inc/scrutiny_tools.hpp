//    scrutiny_tools.hpp
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

namespace scrutiny
{
    namespace tools
    {
        inline AddressRange make_address_range(const uintptr_t start, const uintptr_t end)
        {
            return {reinterpret_cast<void *>(start), reinterpret_cast<void *>(end)};
        }

        inline AddressRange make_address_range(const void *start, const void *end)
        {
            return {const_cast<void *>(start), const_cast<void *>(end)};
        }

        inline AddressRange make_address_range(const void *start, size_t size)
        {
            size = (size == 0) ? 1 : size;
            return {const_cast<void *>(start), reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(start) + size - 1)};
        }

        inline uint8_t get_type_size(const VariableType v)
        {
            if (v == VariableType::unknown)
            {
                return 0;
            }

            return 1 << (static_cast<unsigned int>(v) & 0xF);
        }

        inline uint8_t get_type_size(const VariableTypeSize ts)
        {
            if (ts == VariableTypeSize::_undef)
            {
                return 0;
            }

            return 1 << (static_cast<unsigned int>(ts) & 0xF);
        }

        inline VariableTypeType get_var_type_type(const VariableType v)
        {
            return static_cast<VariableTypeType>(static_cast<unsigned int>(v) & 0xF0);
        }

        inline VariableType make_type(const VariableTypeType tt, const VariableTypeSize ts)
        {
            if (tt == VariableTypeType::_boolean)
            {
                return (ts == VariableTypeSize::_8) ? VariableType::boolean : VariableType::unknown;
            }
            if (tt == VariableTypeType::_undef || ts == VariableTypeSize::_undef)
            {
                return VariableType::unknown;
            }

            return static_cast<VariableType>(static_cast<unsigned int>(tt) | static_cast<unsigned int>(ts));
        }

        inline bool is_float_type(const VariableType v)
        {
            return get_var_type_type(v) == VariableTypeType::_float;
        }

        inline bool is_uint_type(const VariableType v)
        {
            return get_var_type_type(v) == VariableTypeType::_uint;
        }

        inline bool is_sint_type(const VariableType v)
        {
            return get_var_type_type(v) == VariableTypeType::_sint;
        }

#if SCRUTINY_SUPPORT_64BITS
        inline int_biggest_t read_biggest_sint(const AnyType v)
        {
            return v.sint64;
        }

        inline int_biggest_t read_biggest_uint(const AnyType v)
        {
            return v.uint64;
        }

        inline double read_biggest_float(const AnyType v)
        {
            return v.float64;
        }

        inline void set_biggest_sint(AnyType &v, int_biggest_t val)
        {
            v.sint64 = val;
        }

        inline void set_biggest_uint(AnyType &v, uint_biggest_t val)
        {
            v.uint64 = val;
        }

        inline void set_biggest_float(AnyType &v, float_biggest_t val)
        {
            v.float64 = val;
        }
#else
        inline int_biggest_t read_biggest_sint(const AnyType v)
        {
            return v.sint32;
        }

        inline int_biggest_t read_biggest_uint(const AnyType v)
        {
            return v.uint32;
        }

        inline float read_biggest_float(const AnyType v)
        {
            return v.float32;
        }

        inline void set_biggest_sint(AnyType &v, int_biggest_t val)
        {
            v.sint32 = val;
        }

        inline void set_biggest_uint(AnyType &v, uint_biggest_t val)
        {
            v.uint32 = val;
        }

        inline void set_biggest_float(AnyType &v, float_biggest_t val)
        {
            v.float32 = val;
        }
#endif
        inline size_t strnlen(const char *const s, const size_t maxlen)
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
        inline size_t strncpy(char *const dst, const char *const src, const size_t maxlen)
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

        VariableTypeSize get_required_type_size(uint8_t size);
    }

}

#endif //__SCRUTINY_TOOLS_H__