//    scrutiny_datalogging_conditions.cpp
//        The implementation of the datalogging trigger conditions operators
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include "scrutiny_tools.hpp"
#include "scrutiny_main_handler.hpp"
#include "datalogging/scrutiny_datalogging.hpp"
#include "datalogging/scrutiny_datalogging_conditions.hpp"
#include "datalogging/scrutiny_datalogging_types.hpp"

#include "string.h"

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif
namespace scrutiny
{
    namespace datalogging
    {
        namespace conditions
        {
            bool EqualCondition::evaluate(const MainHandler *const mh, const Operand *const op1, const Operand *const op2) const
            {
                scrutiny::VariableType op1_type;
                scrutiny::AnyType op1_val;
                scrutiny::VariableType op2_type;
                scrutiny::AnyType op2_val;

                scrutiny::datalogging::fetch_operand(mh, op1, &op1_val, &op1_type);
                scrutiny::datalogging::fetch_operand(mh, op2, &op2_val, &op2_type);

                return true;
            }
        }
    }
}
