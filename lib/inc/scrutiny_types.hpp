//    scrutiny_types.hpp
//        Aliases on the types used by the Scrutiny project
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_TYPES_H___
#define ___SCRUTINY_TYPES_H___

#include <stdint.h>
#include "scrutiny_setup.hpp"

namespace scrutiny
{
    namespace ctypes
    {
#include "scrutiny_c_compatible_types.h"
    }

    /// @brief Represents an storage endianness
    enum class Endianness : uint8_t
    {
        LITTLE = ctypes::SCRUTINY_C_ENDIANNESS_LITTLE,
        BIG = ctypes::SCRUTINY_C_ENDIANNESS_BIG
    };

    /// @brief Represents an address range with a start an a end.
    typedef ctypes::scrutiny_c_address_range_t AddressRange;

    /// @brief User Command Callback function
    typedef ctypes::scrutiny_c_user_command_callback_t user_command_callback_t;

    /// @brief Represent a type type, meaning a type without its size. uint8, uin16, int32 all have type type uint.
    enum class VariableTypeType : uint8_t
    {
        _sint = ctypes::SCRUTINY_C_VARIABLE_TYPE_TYPE_sint,
        _uint = ctypes::SCRUTINY_C_VARIABLE_TYPE_TYPE_uint,
        _float = ctypes::SCRUTINY_C_VARIABLE_TYPE_TYPE_float,
        _boolean = ctypes::SCRUTINY_C_VARIABLE_TYPE_TYPE_boolean,
        _cfloat = ctypes::SCRUTINY_C_VARIABLE_TYPE_TYPE_cfloat,
        _undef = ctypes::SCRUTINY_C_VARIABLE_TYPE_TYPE_undef
    };

    /// @brief Represent a type size
    enum class VariableTypeSize : uint8_t
    {
        _8 = ctypes::SCRUTINY_C_VARIABLE_TYPE_SIZE_8,
        _16 = ctypes::SCRUTINY_C_VARIABLE_TYPE_SIZE_16,
        _32 = ctypes::SCRUTINY_C_VARIABLE_TYPE_SIZE_32,
        _64 = ctypes::SCRUTINY_C_VARIABLE_TYPE_SIZE_64,
        _128 = ctypes::SCRUTINY_C_VARIABLE_TYPE_SIZE_128,
        _256 = ctypes::SCRUTINY_C_VARIABLE_TYPE_SIZE_256,
        _undef = ctypes::SCRUTINY_C_VARIABLE_TYPE_SIZE_undef
    };

    /// @brief  Represent a datatype. Must match the python core module enum
    enum class VariableType : uint8_t
    {
        sint8 = ctypes::SCRUTINY_C_VARIABLE_TYPE_sint8,
        sint16 = ctypes::SCRUTINY_C_VARIABLE_TYPE_sint16,
        sint32 = ctypes::SCRUTINY_C_VARIABLE_TYPE_sint32,
        uint8 = ctypes::SCRUTINY_C_VARIABLE_TYPE_uint8,
        uint16 = ctypes::SCRUTINY_C_VARIABLE_TYPE_uint16,
        uint32 = ctypes::SCRUTINY_C_VARIABLE_TYPE_uint32,
        float8 = ctypes::SCRUTINY_C_VARIABLE_TYPE_float8,
        float16 = ctypes::SCRUTINY_C_VARIABLE_TYPE_float16,
        float32 = ctypes::SCRUTINY_C_VARIABLE_TYPE_float32,
        cfloat8 = ctypes::SCRUTINY_C_VARIABLE_TYPE_cfloat8,
        cfloat16 = ctypes::SCRUTINY_C_VARIABLE_TYPE_cfloat16,
        cfloat32 = ctypes::SCRUTINY_C_VARIABLE_TYPE_cfloat32,
        boolean = ctypes::SCRUTINY_C_VARIABLE_TYPE_boolean,

#if SCRUTINY_SUPPORT_64BITS
        uint64 = ctypes::SCRUTINY_C_VARIABLE_TYPE_uint64,
        sint64 = ctypes::SCRUTINY_C_VARIABLE_TYPE_sint64,
        float64 = ctypes::SCRUTINY_C_VARIABLE_TYPE_float64,
        cfloat64 = ctypes::SCRUTINY_C_VARIABLE_TYPE_cfloat64,
#endif
        unknown = ctypes::SCRUTINY_C_VARIABLE_TYPE_unknown
    };

    /// @brief Union structure to pass any type of data to a function.
    typedef ctypes::scrutiny_c_any_type_t AnyType;

    /// @brief The fast version of AnyType
    typedef ctypes::scrutiny_c_any_type_fast_t AnyTypeFast;

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
        inline void to_c_struct(ctypes::scrutiny_c_runtime_published_value_t *rpv_c) const
        {
            rpv_c->id = id;
            rpv_c->type = static_cast<ctypes::scrutiny_c_variable_type_e>(type);
        }

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