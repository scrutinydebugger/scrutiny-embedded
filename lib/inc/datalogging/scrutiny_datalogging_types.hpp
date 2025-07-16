//    scrutiny_datalogging_types.hpp
//        Types used across datalogging feature
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGING_TYPES_H___
#define ___SCRUTINY_DATALOGGING_TYPES_H___

#include <string.h>

#include "scrutiny_setup.hpp"
#include "scrutiny_types.hpp"

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

namespace scrutiny
{
    namespace datalogging
    {
        static SCRUTINY_CONSTEXPR unsigned int MAX_OPERANDS = 4;
#if SCRUTINY_HAS_CPP11
        static_assert(SCRUTINY_DATALOGGING_MAX_SIGNAL <= 254, "SCRUTINY_DATALOGGING_MAX_SIGNAL is too big");
        static_assert(MAX_OPERANDS <= 254, "Too many operands. uint8 must be enough for iteration.");
#endif

        typedef ctypes::scrutiny_c_datalogging_buffer_size_t buffer_size_t;

        class EncodingType
        {
          public:
            // clang-format off
            SCRUTINY_ENUM(eEncodingType, uint_least8_t)
            {
                RAW
            };
            // clang-format on
        };

        union AnyTypeCompare
        {
            uint_biggest_t _uint;
            int_biggest_t _sint;
            float _float;
        };

        class VariableTypeCompare
        {
          public:
            // clang-format off
            SCRUTINY_ENUM(eVariableTypeCompare, uint_least8_t)
            {
                _float = static_cast<int>(scrutiny::VariableType::float32),
                _uint = static_cast<int>(scrutiny::BiggestUint),
                _sint = static_cast<int>(scrutiny::BiggestSint)
            };
            // clang-format on
        };

        class OperandType
        {
          public:
            // clang-format off
            SCRUTINY_ENUM(eOperandType, uint_least8_t)
            {
                LITERAL = 0,
                VAR = 1,
                VARBIT = 2,
                RPV = 3
            };
            // clang-format on
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
                VariableType::eVariableType datatype;
            } var;
            struct
            {
                void *addr;
                VariableType::eVariableType datatype;
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
            OperandType::eOperandType type;
            OperandData data;
        };

        class SupportedTriggerConditions
        {
          public:
            // clang-format off
            SCRUTINY_ENUM(eSupportedTriggerConditions, uint_least8_t)
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
            // clang-format on
        };

        struct TriggerConfig
        {
            /// @brief Reads a configuration and make a copy of it
            /// @param other The configuration to copy
            void copy_from(const TriggerConfig * other)
            {
                for (unsigned int i = 0; i < MAX_OPERANDS; i++)
                {
                    memcpy(&operands[i], &other->operands[i], sizeof(Operand));
                }

                condition = other->condition;
                operand_count = other->operand_count;
                hold_time_100ns = other->hold_time_100ns;
            }

            SupportedTriggerConditions::eSupportedTriggerConditions condition; // Selected condition
            uint8_t operand_count;                                             // Number of given operands
            uint32_t hold_time_100ns;                                          // Amount of time that the condition must be true for trigger to trig
            Operand operands[MAX_OPERANDS];                                    // The operand definitions
        };

        class LoggableType
        {
          public:
            // clang-format off
            SCRUTINY_ENUM(eLoggableType, uint_least8_t)
            {
                MEMORY = 0,
                RPV = 1,
                TIME = 2
            };
            // clang-format on
        };
        struct LoggableItem
        {
            LoggableType::eLoggableType type;
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

            uint8_t items_count; // Number of items to logs
            uint16_t decimation; // Decimation of the acquisition. Effectively reduce the sampling rate
            // A value indicating where the trigger should be located in the acquisition window. 0 means left, 255 means right. 128 = middle
            uint8_t probe_location;
            uint32_t timeout_100ns; // Time after which an acquisition is considered complete even if the buffer is not full
            TriggerConfig trigger;  // The trigger configuration
        };

        /// @brief Datalogging Trigger callback
        typedef ctypes::scrutiny_c_datalogging_trigger_callback_t trigger_callback_t;

    } // namespace datalogging
} // namespace scrutiny

#endif // ___SCRUTINY_DATALOGGING_TYPES_H___
