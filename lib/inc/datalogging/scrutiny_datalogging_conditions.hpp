//    scrutiny_datalogging_conditions.hpp
//        The definition of the datalogging trigger conditions operators
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGING_CONDITIONS_H___
#define ___SCRUTINY_DATALOGGING_CONDITIONS_H___

#include <stdarg.h>
#include "datalogging/scrutiny_datalogging_types.hpp"

namespace scrutiny
{
    namespace datalogging
    {
        namespace conditions
        {

            class BaseCondition
            {
            public:
                virtual bool evaluate(const MainHandler *const mh, const Operand *const op1, const Operand *const op2) const = 0;
            };

            class EqualCondition : BaseCondition
            {
            public:
                bool evaluate(const MainHandler *const mh, const Operand *const op1, const Operand *const op2) const;
            };

        }
    }
}

#endif // ___SCRUTINY_DATALOGGING_CONDITIONS_H___