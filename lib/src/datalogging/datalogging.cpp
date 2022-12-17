//    datalogging.cpp
//        Implementation of some datalogging features
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include "datalogging/scrutiny_datalogging.hpp"
#include "string.h"

namespace scrutiny
{
    namespace datalogging
    {
        bool fetch_operand(const MainHandler *const main_handler, const Operand *const operand, AnyType *const val, VariableType *const variable_type)
        {
            bool success = true;
            if (operand->type == OperandType::LITERAL)
            {
                val->float32 = operand->data.literal.val;
                *variable_type = VariableType::float32;
            }
            else if (operand->type == OperandType::RPV)
            {
                RuntimePublishedValue rpv;
                main_handler->get_rpv(operand->data.rpv.id, &rpv);
                success = main_handler->get_rpv_read_callback()(rpv, val);
                *variable_type = rpv.type;
            }
            else if (operand->type == OperandType::VAR)
            {
                success = main_handler->fetch_variable(operand->data.var.addr, operand->data.var.datatype, val);
                *variable_type = operand->data.var.datatype;
            }
            else if (operand->type == OperandType::VARBIT)
            {
                success = main_handler->fetch_variable_bitfield(
                    operand->data.var.addr,
                    tools::get_var_type_type(operand->data.var.datatype),
                    operand->data.varbit.bitoffset,
                    operand->data.varbit.bitsize,
                    val,
                    variable_type);
            }
            else
            {
                success = false;
            }

            if (!success)
            {
                memset(val, 0, sizeof(AnyType));
                *variable_type = VariableType::float32;
            }

            return success;
        }
    }
}