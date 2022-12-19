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
                virtual void reset() = 0;
                virtual bool evaluate(const VariableTypeCompare operand_types[], const AnyTypeCompare operand_vals[]) = 0;
                virtual inline unsigned int get_operand_count(void) const { return 0; };
            };

            class EqualCondition : BaseCondition
            {
            public:
                virtual void reset(){};
                bool evaluate(const VariableTypeCompare operand_types[], const AnyTypeCompare operand_vals[]);
                inline unsigned int get_operand_count(void) const { return 2; }
            };

            class NotEqualCondition : BaseCondition
            {
            public:
                virtual void reset(){};
                bool evaluate(const VariableTypeCompare operand_types[], const AnyTypeCompare operand_vals[]);
                inline unsigned int get_operand_count(void) const { return 2; }
            };

            class GreaterThanCondition : BaseCondition
            {
            public:
                virtual void reset(){};
                bool evaluate(const VariableTypeCompare operand_types[], const AnyTypeCompare operand_vals[]);
                inline unsigned int get_operand_count(void) const { return 2; }
            };

            class GreaterOrEqualThanCondition : BaseCondition
            {
            public:
                virtual void reset(){};
                bool evaluate(const VariableTypeCompare operand_types[], const AnyTypeCompare operand_vals[]);
                inline unsigned int get_operand_count(void) const { return 2; }
            };

            class LessThanCondition : BaseCondition
            {
            public:
                virtual void reset(){};
                bool evaluate(const VariableTypeCompare operand_types[], const AnyTypeCompare operand_vals[]);
                inline unsigned int get_operand_count(void) const { return 2; }
            };

            class LessOrEqualThanCondition : BaseCondition
            {
            public:
                virtual void reset(){};
                bool evaluate(const VariableTypeCompare operand_types[], const AnyTypeCompare operand_vals[]);
                inline unsigned int get_operand_count(void) const { return 2; }
            };

            class ChangeMoreThanCondition : BaseCondition
            {
            public:
                virtual void reset()
                {
                    m_previous_val = 0.0f;
                    m_initialized = false;
                };
                bool evaluate(const VariableTypeCompare operand_types[], const AnyTypeCompare operand_vals[]);
                inline unsigned int get_operand_count(void) const { return 2; }

            protected:
                float m_previous_val;
                bool m_initialized;
            };

        }
    }
}

#endif // ___SCRUTINY_DATALOGGING_CONDITIONS_H___