//    scrutiny_tools.hpp
//        Some simple tools used across the project
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#ifndef __SCRUTINY_TOOLS_H__
#define __SCRUTINY_TOOLS_H__

#include <stdlib.h>
#include <stdint.h>

#include "scrutiny_types.hpp"

#if !SCRUTINY_BUILD_AVR_GCC
#include <cmath>
#else
#include <string.h>
#endif

namespace scrutiny
{
    namespace tools
    {
        /// @brief Makes an address range (start/end address)
        /// @param start Start address
        /// @param end End address
        /// @return AddressRange object
        inline AddressRange make_address_range(const uintptr_t start, const uintptr_t end)
        {
            return {reinterpret_cast<void *>(start), reinterpret_cast<void *>(end)};
        }

        /// @brief Makes an address range (start/end address)
        /// @param start Start address
        /// @param end End address
        /// @return AddressRange object
        inline AddressRange make_address_range(const void *start, const void *end)
        {
            return {const_cast<void *>(start), const_cast<void *>(end)};
        }

        /// @brief Makes an address range (start/end address)
        /// @param start Start address
        /// @param size Address range size
        /// @return AddressRange object
        inline AddressRange make_address_range(const void *start, size_t size)
        {
            size = (size == 0) ? 1 : size;
            return {const_cast<void *>(start), reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(start) + size - 1)};
        }

        /// @brief Returns the size of a given type in bytes
        /// @param vt The VariableType object
        /// @return Size in bytes
        inline uint8_t get_type_size(const VariableType vt)
        {
            if (vt == VariableType::unknown)
            {
                return 0;
            }

            return 1 << (static_cast<unsigned int>(vt) & 0xF);
        }

        /// @brief Returns the size of a given TypeSize in bytes
        /// @param ts The VariableTypeSize object
        /// @return Size in bytes
        inline uint8_t get_type_size(const VariableTypeSize ts)
        {
            if (ts == VariableTypeSize::_undef)
            {
                return 0;
            }

            return 1 << (static_cast<unsigned int>(ts) & 0xF);
        }

        /// @brief Returns the Type Type of a given data type.
        /// @param vt The VariableTypeSize object
        /// @return The VariableTypeType enum object
        inline VariableTypeType get_var_type_type(const VariableType vt)
        {
            return static_cast<VariableTypeType>(static_cast<unsigned int>(vt) & 0xF0);
        }

        /// @brief Creates a VariableType from a TypeType and a size.
        /// @param tt The type type (uint, int, float, etc)
        /// @param ts The type size (8,16, 32)
        /// @return The VariableType enum object
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

        /// @brief Returns true if given Variable Type is a floating point type, regardless of its size
        inline bool is_float_type(const VariableType vt)
        {
            return get_var_type_type(vt) == VariableTypeType::_float;
        }

        /// @brief Returns true if given Variable Type is a unsigned int type, regardless of its size
        inline bool is_uint_type(const VariableType vt)
        {
            return get_var_type_type(vt) == VariableTypeType::_uint;
        }

        /// @brief Returns true if given Variable Type is a signed int type, regardless of its size
        inline bool is_sint_type(const VariableType vt)
        {
            return get_var_type_type(vt) == VariableTypeType::_sint;
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
        /// @brief Custom implementation of non-standard strnlen. Gives the length of a string but doesn't read beyond maxlen
        /// @param s The string
        /// @param maxlen Maximum length to check
        /// @return Minimum between the string length and maxlen param
        inline size_t strnlen(const char *const s, const size_t maxlen)
        {
            size_t n = 0;
            if (s == nullptr)
            {
                return 0;
            }

            for (; n < maxlen; n++)
            {
                if (s[n] == '\0')
                {
                    break;
                }
            }

            return n;
        }

        /// @brief Custom implementation of non-standard strncpy. Copy a string with a maximum size.
        /// Guarantee that the destination string will be null terminated, dropping a character if needed.
        /// @param dst Destination string
        /// @param src Source string
        /// @param maxlen Maximum length to copy
        /// @return Number of characters copied.
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

        /// @brief Portable function that tells if a float is a finite value, meaning is not NaNs, inf, -inf
        /// @param val The float to test
        /// @return true if finite value
        inline bool is_float_finite(const float val)
        {
#if SCRUTINY_BUILD_AVR_GCC
            static_assert(sizeof(float) == 4, "Expect float to be 32 bits");
            uint32_t uv;
            memcpy(&uv, &val, 4);
            uint16_t exponent = (uv >> 23) & 0xFF;
            return exponent != 0xFF;
#else
            return std::isfinite(val);
#endif
        }

        /// @brief Returns true if the type is supported by scrutiny
        bool is_supported_type(VariableType vt);

        /// @brief Returns which variable type to use to store a given size
        /// @param size Given size in bytes
        /// @return The size to use.
        VariableTypeSize get_required_type_size(uint8_t size);

        /// @brief Computes a standard CRC32
        /// @param data Input data
        /// @param size Size of data in bytes
        /// @param start_value Start value of CRC. This value can be used to chain CRC calculation.
        /// @return The CRC32 value of the data
        uint32_t crc32(const uint8_t *data, const uint32_t size, const uint32_t start_value = 0);
    }

}

#endif //__SCRUTINY_TOOLS_H__