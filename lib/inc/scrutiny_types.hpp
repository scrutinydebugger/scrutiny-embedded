//    scrutiny_types.h
//        Aliases on the types used by the Scrutiny project
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_TYPES_H___
#define ___SCRUTINY_TYPES_H___

#include "stdint.h"

namespace scrutiny
{
    struct AddressRange
    {
        void *start;
        void *end;
    };

    typedef unsigned int loop_id_t;

    typedef void (*user_command_callback_t)(const uint8_t subfunction, const uint8_t *request_data, const uint16_t request_data_length, uint8_t *response_data, uint16_t *response_data_length, const uint16_t response_max_data_length);

    enum class VariableTypeType
    {
        _sint = 0 << 4,
        _uint = 1 << 4,
        _float = 2 << 4,
        _boolean = 3 << 4,
        _cfloat = 4 << 4
    };

    enum class VariableTypeSize
    {
        _8 = 0,
        _16 = 1,
        _32 = 2,
        _64 = 3,
        _128 = 4,
        _256 = 5
    };

    // This enum must match the definition in the python core module.
    enum class VariableType
    {
        sint8 = static_cast<uint8_t>(VariableTypeType::_sint) | static_cast<uint8_t>(VariableTypeSize::_8),
        sint16 = static_cast<uint8_t>(VariableTypeType::_sint) | static_cast<uint8_t>(VariableTypeSize::_16),
        sint32 = static_cast<uint8_t>(VariableTypeType::_sint) | static_cast<uint8_t>(VariableTypeSize::_32),
        sint64 = static_cast<uint8_t>(VariableTypeType::_sint) | static_cast<uint8_t>(VariableTypeSize::_64),
        sint128 = static_cast<uint8_t>(VariableTypeType::_sint) | static_cast<uint8_t>(VariableTypeSize::_128),
        sint256 = static_cast<uint8_t>(VariableTypeType::_sint) | static_cast<uint8_t>(VariableTypeSize::_256),
        uint8 = static_cast<uint8_t>(VariableTypeType::_uint) | static_cast<uint8_t>(VariableTypeSize::_8),
        uint16 = static_cast<uint8_t>(VariableTypeType::_uint) | static_cast<uint8_t>(VariableTypeSize::_16),
        uint32 = static_cast<uint8_t>(VariableTypeType::_uint) | static_cast<uint8_t>(VariableTypeSize::_32),
        uint64 = static_cast<uint8_t>(VariableTypeType::_uint) | static_cast<uint8_t>(VariableTypeSize::_64),
        uint128 = static_cast<uint8_t>(VariableTypeType::_uint) | static_cast<uint8_t>(VariableTypeSize::_128),
        uint256 = static_cast<uint8_t>(VariableTypeType::_uint) | static_cast<uint8_t>(VariableTypeSize::_256),
        float8 = static_cast<uint8_t>(VariableTypeType::_float) | static_cast<uint8_t>(VariableTypeSize::_8),
        float16 = static_cast<uint8_t>(VariableTypeType::_float) | static_cast<uint8_t>(VariableTypeSize::_16),
        float32 = static_cast<uint8_t>(VariableTypeType::_float) | static_cast<uint8_t>(VariableTypeSize::_32),
        float64 = static_cast<uint8_t>(VariableTypeType::_float) | static_cast<uint8_t>(VariableTypeSize::_64),
        float128 = static_cast<uint8_t>(VariableTypeType::_float) | static_cast<uint8_t>(VariableTypeSize::_128),
        float256 = static_cast<uint8_t>(VariableTypeType::_float) | static_cast<uint8_t>(VariableTypeSize::_256),
        cfloat8 = static_cast<uint8_t>(VariableTypeType::_cfloat) | static_cast<uint8_t>(VariableTypeSize::_8),
        cfloat16 = static_cast<uint8_t>(VariableTypeType::_cfloat) | static_cast<uint8_t>(VariableTypeSize::_16),
        cfloat32 = static_cast<uint8_t>(VariableTypeType::_cfloat) | static_cast<uint8_t>(VariableTypeSize::_32),
        cfloat64 = static_cast<uint8_t>(VariableTypeType::_cfloat) | static_cast<uint8_t>(VariableTypeSize::_64),
        cfloat128 = static_cast<uint8_t>(VariableTypeType::_cfloat) | static_cast<uint8_t>(VariableTypeSize::_128),
        cfloat256 = static_cast<uint8_t>(VariableTypeType::_cfloat) | static_cast<uint8_t>(VariableTypeSize::_256),
        boolean = static_cast<uint8_t>(VariableTypeType::_boolean) | static_cast<uint8_t>(VariableTypeSize::_8),
        unknown = 0xFF
    };

    union AnyType
    {
        uint8_t uint8;
        uint16_t uint16;
        uint32_t uint32;
        uint64_t uint64;

        int8_t sint8;
        int16_t sint16;
        int32_t sint32;
        int64_t sint64;

        float float32;
        double float64;

        bool boolean;
    };

    struct RuntimePublishedValue
    {
    public:
        uint16_t id;
        VariableType type;
    };

    typedef bool (*RpvReadCallback)(const RuntimePublishedValue rpv, AnyType *outval);
    typedef bool (*RpvWriteCallback)(const RuntimePublishedValue rpv, const AnyType *inval);
}

#endif //  ___SCRUTINY_TYPES_H___