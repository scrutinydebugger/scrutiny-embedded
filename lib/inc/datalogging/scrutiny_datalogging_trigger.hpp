//    scrutiny_datalogging_trigger.hpp
//        The definition of the datalogging trigger conditions operators
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGING_TRIGGER_H___
#define ___SCRUTINY_DATALOGGING_TRIGGER_H___

#include <string.h>

#include "datalogging/scrutiny_datalogging_types.hpp"
#include "scrutiny_setup.hpp"

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

namespace scrutiny
{
    namespace datalogging
    {
        namespace trigger
        {
            struct ConditionSharedData
            {
                struct
                {
                    AnyTypeCompare previous_val;
                    bool initialized;
                } cmt; // Change More Than
            };

            typedef bool (*EvalFn)(ConditionSharedData *, VariableTypeCompare::eVariableTypeCompare const[], AnyTypeCompare const[]);
            typedef void (*ResetFn)(ConditionSharedData *);

            struct ActiveCondition
            {
                EvalFn eval_fn;
                ResetFn reset_fn;
                unsigned int operand_count;
            };

            struct EqualCondition
            {
                static void reset(ConditionSharedData *const data) { static_cast<void>(data); }
                static bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]);
                static inline unsigned int get_operand_count(void) { return 2; }
            };

            struct NotEqualCondition
            {
                static void reset(ConditionSharedData *const data) { static_cast<void>(data); }
                static bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]);
                static inline unsigned int get_operand_count(void) { return 2; }
            };

            struct GreaterThanCondition
            {
                static void reset(ConditionSharedData *const data) { static_cast<void>(data); }
                static bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]);
                static inline unsigned int get_operand_count(void) { return 2; }
            };

            struct GreaterOrEqualThanCondition
            {
                static void reset(ConditionSharedData *const data) { static_cast<void>(data); }
                static bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]);
                static inline unsigned int get_operand_count(void) { return 2; }
            };

            struct LessThanCondition
            {
                static void reset(ConditionSharedData *const data) { static_cast<void>(data); }
                static bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]);
                static inline unsigned int get_operand_count(void) { return 2; }
            };

            struct LessOrEqualThanCondition
            {
                static void reset(ConditionSharedData *const data) { static_cast<void>(data); }
                static bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]);
                static inline unsigned int get_operand_count(void) { return 2; }
            };

            struct ChangeMoreThanCondition
            {
                static void reset(ConditionSharedData *const data)
                {
                    memset(&data->cmt.previous_val, 0, sizeof(data->cmt.previous_val));
                    data->cmt.initialized = false;
                };
                static bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]);
                static inline unsigned int get_operand_count(void) { return 2; }
            };

            struct IsWithinCondition
            {
                static void reset(ConditionSharedData *const data) { static_cast<void>(data); }
                static bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]);
                static inline unsigned int get_operand_count(void) { return 3; }
            };

            struct AlwaysTrueCondition
            {
                static void reset(ConditionSharedData *const data) { static_cast<void>(data); }
                static bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]);
                static inline unsigned int get_operand_count(void) { return 0; }
            }; // namespace AlwaysTrueCondition

        } // namespace trigger
    }     // namespace datalogging
} // namespace scrutiny

#endif // ___SCRUTINY_DATALOGGING_CONDITIONS_H___
