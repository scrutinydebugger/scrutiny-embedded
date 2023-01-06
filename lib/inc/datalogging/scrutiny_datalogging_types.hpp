//    scrutiny_datalogging_types.hpp
//        Types used across datalogging feature
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGING_TYPES_H___
#define ___SCRUTINY_DATALOGGING_TYPES_H___

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

#include "scrutiny_types.hpp"

#include "string.h"

static_assert(SCRUTINY_DATALOGGING_MAX_SIGNAL <= 254, "SCRUTINY_DATALOGGING_MAX_SIGNAL is too big");

namespace scrutiny
{
    namespace datalogging
    {
        constexpr unsigned int MAX_OPERANDS = 4;
        static_assert(MAX_OPERANDS < 255, "Too many operands. uint8 must be enough for iteration.");

        enum class EncodingType
        {
            RAW
        };

        union AnyTypeCompare
        {
            uint_biggest_t _uint;
            int_biggest_t _sint;
            float _float;
        };

        enum class VariableTypeCompare
        {
            _float = static_cast<int>(scrutiny::VariableType::float32),
            _uint = static_cast<int>(scrutiny::BiggestUint),
            _sint = static_cast<int>(scrutiny::BiggestSint)
        };

        enum class OperandType
        {
            LITERAL = 0,
            VAR = 1,
            VARBIT = 2,
            RPV = 3
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

        enum class SupportedTriggerConditions
        {
            AlwaysTrue = 0,
            Equal = 1,
            NotEqual = 2,
            LessThan = 3,
            LessOrEqualThan = 4,
            GreaterThan = 5,
            GreaterOrEqualThan = 6,
            ChangeMoreThan = 7,
            IsWithin = 8
        };

        struct TriggerConfig
        {
            void copy_from(TriggerConfig *other)
            {
                for (unsigned int i = 0; i < MAX_OPERANDS; i++)
                {
                    memcpy(&operands[i], &other->operands[i], sizeof(Operand));
                }

                condition = other->condition;
                operand_count = other->operand_count;
                hold_time_100ns = other->hold_time_100ns;
            }

            SupportedTriggerConditions condition;
            uint8_t operand_count;
            uint32_t hold_time_100ns;
            Operand operands[MAX_OPERANDS];
        };

        enum class LoggableType
        {
            MEMORY,
            RPV,
            TIME
        };
        struct LoggableItem
        {
            LoggableType type;
            union
            {
                struct
                {
                    void *address;
                    uint8_t size;
                } memory;
                struct
                {
                    uint16_t id;
                } rpv;
                struct
                {
                } time;
            } data;
        };

        struct Configuration
        {
            void copy_from(Configuration *other)
            {
                items_count = other->items_count;
                decimation = other->decimation;
                probe_location = other->probe_location;
                timeout_100ns = other->timeout_100ns;
                trigger.copy_from(&other->trigger);
                if (items_count <= SCRUTINY_DATALOGGING_MAX_SIGNAL)
                {
                    for (unsigned int i = 0; i < items_count; i++)
                    {
                        memcpy(&items_to_log[i], &other->items_to_log[i], sizeof(LoggableItem));
                    }
                }
            }

            LoggableItem items_to_log[SCRUTINY_DATALOGGING_MAX_SIGNAL];
            uint8_t items_count;
            uint16_t decimation;
            uint8_t probe_location;
            uint32_t timeout_100ns;
            TriggerConfig trigger;
        };

    }
}

#endif // ___SCRUTINY_DATALOGGING_TYPES_H___