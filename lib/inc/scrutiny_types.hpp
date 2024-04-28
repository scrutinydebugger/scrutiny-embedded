//    scrutiny_types.hpp
//        Aliases on the types used by the Scrutiny project
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#ifndef ___SCRUTINY_TYPES_H___
#define ___SCRUTINY_TYPES_H___

#include <stdint.h>
#include "scrutiny_setup.hpp"

namespace scrutiny
{
    /// @brief Represents an storage endianness
    enum class Endianness : uint8_t
    {
        LITTLE,
        BIG
    };

    /// @brief Represents an address range with a start an a end.
    struct AddressRange
    {
        void *start;
        void *end;
    };

    /// @brief User Command Callback function
    typedef void (*user_command_callback_t)(const uint8_t subfunction, const uint8_t *request_data, const uint16_t request_data_length, uint8_t *response_data, uint16_t *response_data_length, const uint16_t response_max_data_length);

    /// @brief Represent a type type, meaning a type without its size. uint8, uin16, int32 all have type type uint.
    enum class VariableTypeType : uint8_t
    {
        _sint = 0 << 4,
        _uint = 1 << 4,
        _float = 2 << 4,
        _boolean = 3 << 4,
        _cfloat = 4 << 4,
        _undef = 0xF << 4
    };

    /// @brief Represent a type size
    enum class VariableTypeSize : uint8_t
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
    enum class VariableType : uint8_t
    {
        sint8 = static_cast<uint8_t>(VariableTypeType::_sint) | static_cast<uint8_t>(VariableTypeSize::_8),       // cppcheck-suppress[badBitmaskCheck]
        sint16 = static_cast<uint8_t>(VariableTypeType::_sint) | static_cast<uint8_t>(VariableTypeSize::_16),     // cppcheck-suppress[badBitmaskCheck]
        sint32 = static_cast<uint8_t>(VariableTypeType::_sint) | static_cast<uint8_t>(VariableTypeSize::_32),     // cppcheck-suppress[badBitmaskCheck]
        uint8 = static_cast<uint8_t>(VariableTypeType::_uint) | static_cast<uint8_t>(VariableTypeSize::_8),       // cppcheck-suppress[badBitmaskCheck]
        uint16 = static_cast<uint8_t>(VariableTypeType::_uint) | static_cast<uint8_t>(VariableTypeSize::_16),     // cppcheck-suppress[badBitmaskCheck]
        uint32 = static_cast<uint8_t>(VariableTypeType::_uint) | static_cast<uint8_t>(VariableTypeSize::_32),     // cppcheck-suppress[badBitmaskCheck]
        float8 = static_cast<uint8_t>(VariableTypeType::_float) | static_cast<uint8_t>(VariableTypeSize::_8),     // cppcheck-suppress[badBitmaskCheck]
        float16 = static_cast<uint8_t>(VariableTypeType::_float) | static_cast<uint8_t>(VariableTypeSize::_16),   // cppcheck-suppress[badBitmaskCheck]
        float32 = static_cast<uint8_t>(VariableTypeType::_float) | static_cast<uint8_t>(VariableTypeSize::_32),   // cppcheck-suppress[badBitmaskCheck]
        cfloat8 = static_cast<uint8_t>(VariableTypeType::_cfloat) | static_cast<uint8_t>(VariableTypeSize::_8),   // cppcheck-suppress[badBitmaskCheck]
        cfloat16 = static_cast<uint8_t>(VariableTypeType::_cfloat) | static_cast<uint8_t>(VariableTypeSize::_16), // cppcheck-suppress[badBitmaskCheck]
        cfloat32 = static_cast<uint8_t>(VariableTypeType::_cfloat) | static_cast<uint8_t>(VariableTypeSize::_32), // cppcheck-suppress[badBitmaskCheck]
        boolean = static_cast<uint8_t>(VariableTypeType::_boolean) | static_cast<uint8_t>(VariableTypeSize::_8),  // cppcheck-suppress[badBitmaskCheck]

#if SCRUTINY_SUPPORT_64BITS
        uint64 = static_cast<uint8_t>(VariableTypeType::_uint) | static_cast<uint8_t>(VariableTypeSize::_64),     // cppcheck-suppress[badBitmaskCheck]
        sint64 = static_cast<uint8_t>(VariableTypeType::_sint) | static_cast<uint8_t>(VariableTypeSize::_64),     // cppcheck-suppress[badBitmaskCheck]
        float64 = static_cast<uint8_t>(VariableTypeType::_float) | static_cast<uint8_t>(VariableTypeSize::_64),   // cppcheck-suppress[badBitmaskCheck]
        cfloat64 = static_cast<uint8_t>(VariableTypeType::_cfloat) | static_cast<uint8_t>(VariableTypeSize::_64), // cppcheck-suppress[badBitmaskCheck]
#endif
        unknown = 0xFF
    };

    /// @brief Union structure to pass any type of data to a function.
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

    /// @brief The fast version of AnyType
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

    /// @brief  Represent a RuntimePublishedValue definition. It is a data object with a type and a ID that can be read/written by the server
    struct RuntimePublishedValue
    {
    public:
        uint16_t id;
        VariableType type;
    };

    /// @brief Callback called on Runtime Published Value read
    typedef bool (*RpvReadCallback)(const RuntimePublishedValue rpv, AnyType *outval);
    /// @brief Callback called on Runtime Published Value write
    typedef bool (*RpvWriteCallback)(const RuntimePublishedValue rpv, const AnyType *inval);

    /// @brief Represents a memory block with data/mask pointer. Mainly used for memory write operations.
    struct MemoryBlock
    {
        uint8_t *start_address;
        uint16_t length;
        uint8_t *source_data;
        uint8_t *mask;
    };
}

#endif //  ___SCRUTINY_TYPES_H___