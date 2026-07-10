//    scrutiny_datalogging_types.hpp
//        Types used across datalogging feature
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

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
        static SCRUTINY_CONSTEXPR unsigned int MAX_OPERANDS = 3;
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

        struct AnyValAndTypeComparePair
        {
            AnyTypeCompare val;
            VariableTypeCompare::eVariableTypeCompare valtype;
        };

#if SCRUTINY_HAS_CPP11
        static_assert(sizeof(AnyValAndTypeComparePair) == sizeof(AnyValAndTypePair), "Mismatch in pair struct");
#endif
        class OperandType
        {
          public:
            // clang-format off
            SCRUTINY_ENUM(eOperandType, uint_least8_t)
            {
                Literal = 0,
                Var = 1,
                VarBit = 2,
                Rpv = 3
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
                uint_least8_t bitoffset;
                uint_least8_t bitsize;
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
            inline void copy_from(TriggerConfig const *const other) { memcpy(this, other, sizeof(TriggerConfig)); }

            Operand operands[MAX_OPERANDS];                                    // The operand definitions
            uint32_t hold_time_100ns;                                          // Amount of time that the condition must be true for trigger to trig
            uint_least8_t operand_count;                                       // Number of given operands
            SupportedTriggerConditions::eSupportedTriggerConditions condition; // Selected condition
        };

        class LoggableType
        {
          public:
            // clang-format off
            SCRUTINY_ENUM(eLoggableType, uint_least8_t)
            {
                Memory = 0,
                Rpv = 1,
                Time = 2
            };
            // clang-format on
        };

        union LoggableItem
        {
            struct
            {
                void *_pad1;
                uint_least8_t _pad2;
                LoggableType::eLoggableType type;
            } common;
            struct
            {
                void *address;
                uint_least8_t size;
            } memory;
            struct
            {
                uint16_t id;
            } rpv;
            struct
            {
            } time;
        };

        struct Configuration
        {
            /// @brief Reads a configuration and makes a copy of it
            /// @param other The configuration to copy
            inline void copy_from(Configuration const *const other) { memcpy(this, other, sizeof(Configuration)); }

            LoggableItem items_to_log[SCRUTINY_DATALOGGING_MAX_SIGNAL]; // Definitions of the items to log
            TriggerConfig trigger;                                      // The trigger configuration
            uint32_t timeout_100ns;    // Time after which an acquisition is considered complete even if the buffer is not full
            uint16_t decimation;       // Decimation of the acquisition. Effectively reduce the sampling rate
            uint_least8_t items_count; // Number of items to log
            // A value indicating where the trigger should be located in the acquisition window. 0 means left, 255 means right. 128 = middle
            uint_least8_t probe_location;
        };

        /// @brief Datalogging Trigger callback
        typedef ctypes::scrutiny_c_datalogging_trigger_callback_t trigger_callback_t;

    } // namespace datalogging
} // namespace scrutiny

#endif // ___SCRUTINY_DATALOGGING_TYPES_H___
