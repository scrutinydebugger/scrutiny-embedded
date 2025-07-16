//    scrutiny_datalogging_trigger.hpp
//        The definition of the datalogging trigger conditions operators
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

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

            class BaseCondition
            {
              public:
                virtual void reset(ConditionSharedData *const data) { static_cast<void>(data); };
                virtual bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]) = 0;
                virtual unsigned int get_operand_count(void) const { return 0; };
            };

            class EqualCondition : public BaseCondition
            {
              public:
                bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]) SCRUTINY_OVERRIDE;
                inline unsigned int get_operand_count(void) const SCRUTINY_OVERRIDE { return 2; }
            };

            class NotEqualCondition : public BaseCondition
            {
              public:
                bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]) SCRUTINY_OVERRIDE;
                inline unsigned int get_operand_count(void) const SCRUTINY_OVERRIDE { return 2; }
            };

            class GreaterThanCondition : public BaseCondition
            {
              public:
                bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]) SCRUTINY_OVERRIDE;
                inline unsigned int get_operand_count(void) const SCRUTINY_OVERRIDE { return 2; }
            };

            class GreaterOrEqualThanCondition : public BaseCondition
            {
              public:
                bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]) SCRUTINY_OVERRIDE;
                inline unsigned int get_operand_count(void) const SCRUTINY_OVERRIDE { return 2; }
            };

            class LessThanCondition : public BaseCondition
            {
              public:
                bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]) SCRUTINY_OVERRIDE;
                inline unsigned int get_operand_count(void) const SCRUTINY_OVERRIDE { return 2; }
            };

            class LessOrEqualThanCondition : public BaseCondition
            {
              public:
                bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]) SCRUTINY_OVERRIDE;
                inline unsigned int get_operand_count(void) const SCRUTINY_OVERRIDE { return 2; }
            };

            class ChangeMoreThanCondition : public BaseCondition
            {
              public:
                void reset(ConditionSharedData *const data) SCRUTINY_OVERRIDE SCRUTINY_FINAL
                {
                    memset(&data->cmt.previous_val, 0, sizeof(data->cmt.previous_val));
                    data->cmt.initialized = false;
                };
                bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]) SCRUTINY_OVERRIDE;
                inline unsigned int get_operand_count(void) const SCRUTINY_OVERRIDE { return 2; }
            };

            class IsWithinCondition : public BaseCondition
            {
              public:
                bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]) SCRUTINY_OVERRIDE;
                inline unsigned int get_operand_count(void) const SCRUTINY_OVERRIDE { return 3; }
            };

            class AlwaysTrueCondition : public BaseCondition
            {
              public:
                bool evaluate(
                    ConditionSharedData *const data,
                    VariableTypeCompare::eVariableTypeCompare const operand_types[],
                    AnyTypeCompare const operand_vals[]) SCRUTINY_OVERRIDE;
            };

            // cppcheck-suppress[noConstructor]
            class ConditionSet
            {
              public:
                EqualCondition eq;
                NotEqualCondition neq;
                GreaterThanCondition gt;
                GreaterOrEqualThanCondition get;
                LessThanCondition lt;
                LessOrEqualThanCondition let;
                ChangeMoreThanCondition cmt;
                IsWithinCondition within;
                AlwaysTrueCondition always_true;

                inline ConditionSharedData *data() { return &m_data; };

              private:
                ConditionSharedData m_data; // Data shared between each condition.
            };

        } // namespace trigger
    }     // namespace datalogging
} // namespace scrutiny

#endif // ___SCRUTINY_DATALOGGING_CONDITIONS_H___
