//    scrutiny_types.hpp
//        Aliases on the types used by the Scrutiny project
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_TYPES_H___
#define ___SCRUTINY_TYPES_H___

#include "stdint.h"
#include "scrutiny_setup.hpp"

namespace scrutiny
{
    enum class Endianness
    {
        LITTLE,
        BIG
    };

    struct AddressRange
    {
        void *start;
        void *end;
    };

    typedef void (*user_command_callback_t)(const uint8_t subfunction, const uint8_t *request_data, const uint16_t request_data_length, uint8_t *response_data, uint16_t *response_data_length, const uint16_t response_max_data_length);

    enum class VariableTypeType
    {
        _sint = 0 << 4,
        _uint = 1 << 4,
        _float = 2 << 4,
        _boolean = 3 << 4,
        _cfloat = 4 << 4,
        _undef = 0xF << 4
    };

    enum class VariableTypeSize
    {
        _8 = 0,
        _16 = 1,
        _32 = 2,
        _64 = 3,
        _128 = 4,
        _256 = 5,
        _undef = 0xF
    };

    /// @brief  Represent a datatype. Must match the python core module enum
    enum class VariableType
    {
        sint8 = static_cast<uint8_t>(VariableTypeType::_sint) | static_cast<uint8_t>(VariableTypeSize::_8),
        sint16 = static_cast<uint8_t>(VariableTypeType::_sint) | static_cast<uint8_t>(VariableTypeSize::_16),
        sint32 = static_cast<uint8_t>(VariableTypeType::_sint) | static_cast<uint8_t>(VariableTypeSize::_32),
        uint8 = static_cast<uint8_t>(VariableTypeType::_uint) | static_cast<uint8_t>(VariableTypeSize::_8),
        uint16 = static_cast<uint8_t>(VariableTypeType::_uint) | static_cast<uint8_t>(VariableTypeSize::_16),
        uint32 = static_cast<uint8_t>(VariableTypeType::_uint) | static_cast<uint8_t>(VariableTypeSize::_32),
        float8 = static_cast<uint8_t>(VariableTypeType::_float) | static_cast<uint8_t>(VariableTypeSize::_8),
        float16 = static_cast<uint8_t>(VariableTypeType::_float) | static_cast<uint8_t>(VariableTypeSize::_16),
        float32 = static_cast<uint8_t>(VariableTypeType::_float) | static_cast<uint8_t>(VariableTypeSize::_32),
        cfloat8 = static_cast<uint8_t>(VariableTypeType::_cfloat) | static_cast<uint8_t>(VariableTypeSize::_8),
        cfloat16 = static_cast<uint8_t>(VariableTypeType::_cfloat) | static_cast<uint8_t>(VariableTypeSize::_16),
        cfloat32 = static_cast<uint8_t>(VariableTypeType::_cfloat) | static_cast<uint8_t>(VariableTypeSize::_32),
        boolean = static_cast<uint8_t>(VariableTypeType::_boolean) | static_cast<uint8_t>(VariableTypeSize::_8),

#if SCRUTINY_SUPPORT_64BITS
        uint64 = static_cast<uint8_t>(VariableTypeType::_uint) | static_cast<uint8_t>(VariableTypeSize::_64),
        sint64 = static_cast<uint8_t>(VariableTypeType::_sint) | static_cast<uint8_t>(VariableTypeSize::_64),
        float64 = static_cast<uint8_t>(VariableTypeType::_float) | static_cast<uint8_t>(VariableTypeSize::_64),
        cfloat64 = static_cast<uint8_t>(VariableTypeType::_cfloat) | static_cast<uint8_t>(VariableTypeSize::_64),
#endif
        unknown = 0xFF
    };

    union AnyType
    {
        uint8_t uint8;
        uint16_t uint16;
        uint32_t uint32;

        int8_t sint8;
        int16_t sint16;
        int32_t sint32;

        float float32;
        bool boolean;
#if SCRUTINY_SUPPORT_64BITS
        int64_t sint64;
        uint64_t uint64;
        double float64;
#endif
    };

    union AnyTypeFast
    {
        uint_fast8_t uint8;
        uint_fast16_t uint16;
        uint_fast32_t uint32;

        int_fast8_t sint8;
        int_fast16_t sint16;
        int_fast32_t sint32;

        float float32;
        bool boolean;
#if SCRUTINY_SUPPORT_64BITS
        uint_fast64_t uint64;
        int_fast64_t sint64;
        double float64;
#endif
    };

#if SCRUTINY_SUPPORT_64BITS
    typedef uint64_t uint_biggest_t;
    typedef int64_t int_biggest_t;
    typedef double float_biggest_t;

    constexpr VariableType BiggestUint = VariableType::uint64;
    constexpr VariableType BiggestSint = VariableType::sint64;
    constexpr VariableType BiggestFloat = VariableType::float64;

#else
    typedef uint32_t uint_biggest_t;
    typedef int32_t int_biggest_t;
    typedef float float_biggest_t;

    constexpr VariableType BiggestUint = VariableType::uint32;
    constexpr VariableType BiggestSint = VariableType::sint32;
    constexpr VariableType BiggestFloat = VariableType::float32;
#endif

    struct RuntimePublishedValue
    {
    public:
        uint16_t id;
        VariableType type;
    };

    typedef bool (*RpvReadCallback)(const RuntimePublishedValue rpv, AnyType *outval);
    typedef bool (*RpvWriteCallback)(const RuntimePublishedValue rpv, const AnyType *inval);

    struct MemoryBlock
    {
        uint8_t *start_address;
        uint16_t length;
        uint8_t *source_data;
        uint8_t *mask;
    };
}

#endif //  ___SCRUTINY_TYPES_H___