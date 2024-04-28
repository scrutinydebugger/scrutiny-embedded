//    scrutiny_datalogging_types.hpp
//        Types used across datalogging feature
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGING_TYPES_H___
#define ___SCRUTINY_DATALOGGING_TYPES_H___

#include <string.h>

#include "scrutiny_setup.hpp"
#include "scrutiny_types.hpp"

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

static_assert(SCRUTINY_DATALOGGING_MAX_SIGNAL <= 254, "SCRUTINY_DATALOGGING_MAX_SIGNAL is too big");

namespace scrutiny
{
    namespace datalogging
    {
        constexpr unsigned int MAX_OPERANDS = 4;
        static_assert(MAX_OPERANDS <= 254, "Too many operands. uint8 must be enough for iteration.");

        enum class EncodingType : uint8_t
        {
            RAW
        };

        union AnyTypeCompare
        {
            uint_biggest_t _uint;
            int_biggest_t _sint;
            float _float;
        };

        enum class VariableTypeCompare : uint8_t
        {
            _float = static_cast<int>(scrutiny::VariableType::float32),
            _uint = static_cast<int>(scrutiny::BiggestUint),
            _sint = static_cast<int>(scrutiny::BiggestSint)
        };

        enum class OperandType : uint8_t
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

        enum class SupportedTriggerConditions : uint8_t
        {
            AlwaysTrue = 0,         // Always true
            Equal = 1,              // Operand1 == Operand2
            NotEqual = 2,           // Operand1 != Operand2
            LessThan = 3,           // Operand1 < Operand2
            LessOrEqualThan = 4,    // Operand1 <= Operand2
            GreaterThan = 5,        // Operand1 > Operand2
            GreaterOrEqualThan = 6, // Operand1 >= Operand2
            ChangeMoreThan = 7,     // |Operand1[n] - Operand1[n-1]| > |Operand2| && sign(Operand1[n] - Operand1[n-1]) == sign(Operand2)
            IsWithin = 8            // |Operand1 - Operand2| < |Operand3|
        };

        struct TriggerConfig
        {
            /// @brief Reads a configuration and make a copy of it
            /// @param other The configuration to copy
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

            SupportedTriggerConditions condition; // Selected condition
            uint8_t operand_count;                // Number of given operands
            uint32_t hold_time_100ns;             // Amount of time that the condition must be true for trigger to trig
            Operand operands[MAX_OPERANDS];       // The operand definitions
        };

        enum class LoggableType : uint8_t
        {
            MEMORY = 0,
            RPV = 1,
            TIME = 2
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
            /// @brief Reads a configuration and make a copy of it
            /// @param other The configuration to copy
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

            LoggableItem items_to_log[SCRUTINY_DATALOGGING_MAX_SIGNAL]; // Definitions of the items to log

            uint8_t items_count;    // Number of items to logs
            uint16_t decimation;    // Decimation of the acquisition. Effectively reduce the sampling rate
            uint8_t probe_location; // A value indicating where the trigger should be located in the acquisition window. 0 means left, 255 means right. 128 = middle
            uint32_t timeout_100ns; // Time after which an acquisition is considered complete even if the buffer is not full
            TriggerConfig trigger;  // The trigger configuration
        };

        /// @brief Datalogging Trigger callback
        typedef void (*trigger_callback_t)();

    }
}

#endif // ___SCRUTINY_DATALOGGING_TYPES_H___