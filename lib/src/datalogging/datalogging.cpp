//    datalogging.cpp
//        Implementation of some datalogging features
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#include "datalogging/scrutiny_datalogging.hpp"
#include "scrutiny_tools.hpp"
#include "scrutiny_main_handler.hpp"
#include "string.h"

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif
namespace scrutiny
{
    namespace datalogging
    {
        void convert_to_compare_type(VariableType *vtype, AnyType *val)
        {
            switch (*vtype)
            {
#if SCRUTINY_SUPPORT_64BITS
            case VariableType::float64:
                *vtype = VariableType::float32;
                val->float32 = static_cast<float>(val->float64);
                break;
#endif
            case VariableType::boolean:
                *vtype = BiggestUint;
                tools::set_biggest_uint(*val, static_cast<uint_biggest_t>(val->boolean));
                break;

            case VariableType::sint8:
                *vtype = BiggestSint;
                tools::set_biggest_sint(*val, static_cast<uint_biggest_t>(val->sint8));
                break;

            case VariableType::sint16:
                *vtype = BiggestSint;
                tools::set_biggest_sint(*val, static_cast<uint_biggest_t>(val->sint16));
                break;

            case VariableType::sint32:
                *vtype = BiggestSint;
                tools::set_biggest_sint(*val, static_cast<uint_biggest_t>(val->sint32));
                break;

            case VariableType::uint8:
                *vtype = BiggestUint;
                tools::set_biggest_uint(*val, static_cast<uint_biggest_t>(val->uint8));
                break;

            case VariableType::uint16:
                *vtype = BiggestUint;
                tools::set_biggest_uint(*val, static_cast<uint_biggest_t>(val->uint16));
                break;

            case VariableType::uint32:
                *vtype = BiggestUint;
                tools::set_biggest_uint(*val, static_cast<uint_biggest_t>(val->uint32));
                break;

            default:
                break;
            }
        }

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