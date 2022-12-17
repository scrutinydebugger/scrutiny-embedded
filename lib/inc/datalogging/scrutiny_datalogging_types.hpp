//    scrutiny_datalogging_types.hpp
//        Types used across datalogging feature
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGING_TYPES_H___
#define ___SCRUTINY_DATALOGGING_TYPES_H___

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

#include "scrutiny_types.hpp"

namespace scrutiny
{
    namespace datalogging
    {
        typedef bool (*TriggerCondition)(...);

        enum class OperandType
        {
            LITERAL,
            VAR,
            VARBIT,
            RPV
        };

        union OperandData
        {
            struct
            {
                float val;
            } literal;
            struct
            {
                void *addr;
                VariableType datatype;
            } var;
            struct
            {
                void *addr;
                VariableType datatype;
                uint8_t bitoffset;
                uint8_t bitsize;
            } varbit;
            struct
            {
                uint16_t id;
            } rpv;
        };

        struct Operand
        {
            OperandType type;
            OperandData data;
        };

        struct Configuration
        {
            TriggerCondition condition;
            uint8_t operand_count;
            uint16_t decimation;
            Operand *operands;
        };
    }
}

#endif // ___SCRUTINY_DATALOGGING_TYPES_H___