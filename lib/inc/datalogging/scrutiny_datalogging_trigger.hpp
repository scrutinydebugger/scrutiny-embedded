//    scrutiny_datalogging_trigger.hpp
//        The definition of the datalogging trigger conditions operators
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGING_TRIGGER_H___
#define ___SCRUTINY_DATALOGGING_TRIGGER_H___

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

#include <stdarg.h>
#include "datalogging/scrutiny_datalogging_types.hpp"

namespace scrutiny
{
    namespace datalogging
    {
        namespace trigger
        {
            class BaseCondition
            {
            public:
                virtual bool evaluate(const VariableType operand_types[], const AnyTypeCompare operand_vals[]) const = 0;
                virtual inline unsigned int get_operand_count(void) const = 0;
            };

            class EqualCondition : BaseCondition
            {
            public:
                bool evaluate(const VariableType operand_types[], const AnyTypeCompare operand_vals[]) const;
                inline unsigned int get_operand_count(void) const { return 2; }
            };

            class NotEqualCondition : BaseCondition
            {
            public:
                bool evaluate(const VariableType operand_types[], const AnyTypeCompare operand_vals[]) const;
                inline unsigned int get_operand_count(void) const { return 2; }
            };

            class GreaterThanCondition : BaseCondition
            {
            public:
                bool evaluate(const VariableType operand_types[], const AnyTypeCompare operand_vals[]) const;
                inline unsigned int get_operand_count(void) const { return 2; }
            };

            class GreaterOrEqualThanCondition : BaseCondition
            {
            public:
                bool evaluate(const VariableType operand_types[], const AnyTypeCompare operand_vals[]) const;
                inline unsigned int get_operand_count(void) const { return 2; }
            };

            class LessThanCondition : BaseCondition
            {
            public:
                bool evaluate(const VariableType operand_types[], const AnyTypeCompare operand_vals[]) const;
                inline unsigned int get_operand_count(void) const { return 2; }
            };

            class LessOrEqualThanCondition : BaseCondition
            {
            public:
                bool evaluate(const VariableType operand_types[], const AnyTypeCompare operand_vals[]) const;
                inline unsigned int get_operand_count(void) const { return 2; }
            };

        }
    }
}

#endif // ___SCRUTINY_DATALOGGING_CONDITIONS_H___