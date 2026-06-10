//    scrutiny_types.hpp
//        Aliases on the types used by the Scrutiny project
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_TYPES_H___
#define ___SCRUTINY_TYPES_H___

#include "scrutiny_setup.hpp"
#include <stdint.h>

namespace scrutiny
{
    class LoopHandler;

    namespace ctypes
    {
#include "scrutiny_c_compatible_types.h"
    }

    /// @brief Represents an storage endianness
    class Endianness
    {
      public:
        // clang-format off
        SCRUTINY_ENUM(eEndianness, uint_least8_t)
        {
            LITTLE = ctypes::SCRUTINY_C_ENDIANNESS_LITTLE,
            BIG = ctypes::SCRUTINY_C_ENDIANNESS_BIG
        };
        // clang-format on
    };

    /// @brief A status code returned by scrutiny functions
    class Status
    {
      public:
        // clang-format off
        SCRUTINY_ENUM(eStatus, uint_least8_t)
        {
            SUCCESS = ctypes::SCRUTINY_C_SUCCESS,
            ERROR = ctypes::SCRUTINY_C_ERROR
        };
        // clang-format on
    };

    /// @brief Represents an address range with a start an a end.
    typedef ctypes::scrutiny_c_address_range_t AddressRange;

    /// @brief User Command Callback function
    typedef ctypes::scrutiny_c_user_command_callback_t user_command_callback_t;

    /// @brief Represent a type type, meaning a type without its size. uint8, uin16, int32 all have type type uint.
    class VariableTypeType
    {
      public:
        // clang-format off
        SCRUTINY_ENUM(eVariableTypeType, uint_least8_t)
        {
            _sint = ctypes::SCRUTINY_C_VARIABLE_TYPE_TYPE_sint,
            _uint = ctypes::SCRUTINY_C_VARIABLE_TYPE_TYPE_uint,
            _float = ctypes::SCRUTINY_C_VARIABLE_TYPE_TYPE_float,
            _boolean = ctypes::SCRUTINY_C_VARIABLE_TYPE_TYPE_boolean,
            _cfloat = ctypes::SCRUTINY_C_VARIABLE_TYPE_TYPE_cfloat,
            _undef = ctypes::SCRUTINY_C_VARIABLE_TYPE_TYPE_undef
        };
        // clang-format on
    };

    /// @brief Represent a type size
    class VariableTypeSize
    {
      public:
        // clang-format off
        SCRUTINY_ENUM(eVariableTypeSize, uint_least8_t)
        {
#if CHAR_BIT==8            
            _8 = ctypes::SCRUTINY_C_VARIABLE_TYPE_SIZE_8,
#endif            
            _16 = ctypes::SCRUTINY_C_VARIABLE_TYPE_SIZE_16,
            _32 = ctypes::SCRUTINY_C_VARIABLE_TYPE_SIZE_32,
            _64 = ctypes::SCRUTINY_C_VARIABLE_TYPE_SIZE_64,
            _128 = ctypes::SCRUTINY_C_VARIABLE_TYPE_SIZE_128,
            _256 = ctypes::SCRUTINY_C_VARIABLE_TYPE_SIZE_256,
            _undef = ctypes::SCRUTINY_C_VARIABLE_TYPE_SIZE_undef
        };
        // clang-format on
    };

    /// @brief  Represent a datatype. Must match the python core module enum
    class VariableType
    {
      public:
        // clang-format off
        SCRUTINY_ENUM(eVariableType, uint_least8_t)
        {
#if CHAR_BIT == 8
            sint8 = ctypes::SCRUTINY_C_VARIABLE_TYPE_sint8,
            uint8 = ctypes::SCRUTINY_C_VARIABLE_TYPE_uint8,
            float8 = ctypes::SCRUTINY_C_VARIABLE_TYPE_float8,
            cfloat8 = ctypes::SCRUTINY_C_VARIABLE_TYPE_cfloat8,
            boolean8 = ctypes::SCRUTINY_C_VARIABLE_TYPE_boolean8,
#endif
            sint16 = ctypes::SCRUTINY_C_VARIABLE_TYPE_sint16,
            sint32 = ctypes::SCRUTINY_C_VARIABLE_TYPE_sint32,
            uint16 = ctypes::SCRUTINY_C_VARIABLE_TYPE_uint16,
            uint32 = ctypes::SCRUTINY_C_VARIABLE_TYPE_uint32,
            float16 = ctypes::SCRUTINY_C_VARIABLE_TYPE_float16,
            float32 = ctypes::SCRUTINY_C_VARIABLE_TYPE_float32,
            cfloat16 = ctypes::SCRUTINY_C_VARIABLE_TYPE_cfloat16,
            cfloat32 = ctypes::SCRUTINY_C_VARIABLE_TYPE_cfloat32,
            boolean16 = ctypes::SCRUTINY_C_VARIABLE_TYPE_boolean16,
            boolean32 = ctypes::SCRUTINY_C_VARIABLE_TYPE_boolean32,

            boolean = ctypes::SCRUTINY_C_VARIABLE_TYPE_boolean, // No size = platform default

#if SCRUTINY_SUPPORT_64BITS
            uint64 = ctypes::SCRUTINY_C_VARIABLE_TYPE_uint64,
            sint64 = ctypes::SCRUTINY_C_VARIABLE_TYPE_sint64,
            float64 = ctypes::SCRUTINY_C_VARIABLE_TYPE_float64,
            cfloat64 = ctypes::SCRUTINY_C_VARIABLE_TYPE_cfloat64,
            boolean64 = ctypes::SCRUTINY_C_VARIABLE_TYPE_boolean64,
#endif
            unknown = ctypes::SCRUTINY_C_VARIABLE_TYPE_unknown
        };
        // clang-format on
    };

    /// @brief Union structure to pass any type of data to a function.
    typedef ctypes::scrutiny_c_any_type_t AnyType;

    /// @brief The fast version of AnyType
    typedef ctypes::scrutiny_c_any_type_fast_t AnyTypeFast;

#if SCRUTINY_SUPPORT_64BITS
    typedef uint64_t uint_biggest_t;
    typedef int64_t int_biggest_t;
    typedef double float_biggest_t;

#define BiggestUint VariableType::uint64
#define BiggestSint VariableType::sint64
#define BiggestFloat VariableType::float64

#else
    typedef uint32_t uint_biggest_t;
    typedef int32_t int_biggest_t;
    typedef float float_biggest_t;

#define BiggestUint VariableType::uint32
#define BiggestSint VariableType::sint32
#define BiggestFloat VariableType::float32
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
        VariableType::eVariableType type;
    };

    /// @brief Callback called on Runtime Published Value read
    typedef bool (*RpvReadCallback)(RuntimePublishedValue const rpv, AnyType *outval, LoopHandler *const caller);
    /// @brief Callback called on Runtime Published Value write
    typedef bool (*RpvWriteCallback)(RuntimePublishedValue const rpv, AnyType const *inval, LoopHandler *const caller);

    /// @brief Represents a memory block with data/mask pointer. Mainly used for memory write operations.
    struct MemoryBlockNative
    {
        unsigned char *start_address;
        uint16_t length;
        unsigned char *source_data;
        unsigned char *mask;

        inline uint16_t length_8bits(void) const { return length * (CHAR_BIT/8);}
    };

    /// @brief Represents a memory block with data/mask pointer where the data is encoded with 8bits per char. 
    /// Mainly used for memory write operations.
    struct MemoryBlock8Bits
    {
        unsigned char *start_address;
        uint16_t length;
        unsigned char *source_data;
        unsigned char *mask;

        inline uint16_t length_char(void) const { return length / (CHAR_BIT/8);}
    };
} // namespace scrutiny

#endif //  ___SCRUTINY_TYPES_H___
