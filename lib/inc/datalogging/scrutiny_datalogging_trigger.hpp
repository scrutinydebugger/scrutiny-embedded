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

            typedef bool (*EvalFn)(ConditionSharedData *, AnyValAndTypeComparePair const[]);
            typedef void (*ResetFn)(ConditionSharedData *);

            struct ActiveCondition
            {
                EvalFn eval_fn;
                ResetFn reset_fn;
                unsigned int operand_count;
            };

            class RelationalOperator
            {
              public:
                // clang-format off
                SCRUTINY_ENUM(eRelationalOperator, uint_least8_t)
                {
                    Equal,
                    NotEqual,
                    GreaterThan,
                    GreaterOrEqualThan,
                    LessThan,
                    LessOrEqualThan
                };
                // clang-format on
            };

            struct EqualCondition
            {
                static bool evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[]);
                static inline unsigned int get_operand_count(void) { return 2; }
            };

            struct NotEqualCondition
            {
                static bool evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[]);
                static inline unsigned int get_operand_count(void) { return 2; }
            };

            struct GreaterThanCondition
            {
                static bool evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[]);
                static inline unsigned int get_operand_count(void) { return 2; }
            };

            struct GreaterOrEqualThanCondition
            {
                static bool evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[]);
                static inline unsigned int get_operand_count(void) { return 2; }
            };

            struct LessThanCondition
            {
                static bool evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[]);
                static inline unsigned int get_operand_count(void) { return 2; }
            };

            struct LessOrEqualThanCondition
            {
                static bool evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[]);
                static inline unsigned int get_operand_count(void) { return 2; }
            };

            struct ChangeMoreThanCondition
            {
                static void reset(ConditionSharedData *const data)
                {
                    memset(&data->cmt.previous_val, 0, sizeof(data->cmt.previous_val));
                    data->cmt.initialized = false;
                };
                static bool evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[]);
                static inline unsigned int get_operand_count(void) { return 2; }
            };

            struct IsWithinCondition
            {
                static bool evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[]);
                static inline unsigned int get_operand_count(void) { return 3; }
            };

            struct AlwaysTrueCondition
            {
                static bool evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[]);
                static inline unsigned int get_operand_count(void) { return 0; }
            }; // namespace AlwaysTrueCondition

        } // namespace trigger
    }     // namespace datalogging
} // namespace scrutiny

#endif // ___SCRUTINY_DATALOGGING_CONDITIONS_H___
