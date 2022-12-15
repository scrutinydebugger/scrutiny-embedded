#include "scrutiny_tools.hpp"
#include "scrutiny_main_handler.hpp"
#include "datalogging/scrutiny_datalogging_conditions.hpp"
#include "datalogging/scrutiny_datalogging_types.hpp"

#include "string.h"

namespace scrutiny
{
    namespace datalogging
    {
        bool fetch_operand(MainHandler *main_handler, Operand *operand, AnyType *val, VariableType *variable_type)
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
                    operand->data.var.datatype,
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

        namespace conditions
        {
            bool equal(Operand *operand1, Operand *operand2)
            {
            }
        }
    }
}
