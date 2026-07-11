//    scrutiny_datalogging.cpp
//        Implementation of some datalogging features
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "datalogging/scrutiny_datalogging.hpp"
#include "scrutiny_main_handler.hpp"
#include "scrutiny_setup.hpp"
#include "scrutiny_tools.hpp"
#include <limits.h>
#include <string.h>

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif
namespace scrutiny
{
    namespace datalogging
    {
        /// @brief Takes a value and make sure that the biggest supported version of it is valid. For instance if a value is a uint8, the uint32 field
        /// will have a valid value with zeros in the MSBs
        /// @param vtype Variable type of the input data
        /// @param val Input data
        void convert_to_compare_type(AnyValAndTypePair *const val_type_pair)
        {
            switch (val_type_pair->valtype)
            {
#if SCRUTINY_SUPPORT_64BITS
            case VariableType::float64:
            {
                val_type_pair->valtype = VariableType::float32;
                float const valf32 = static_cast<float>(val_type_pair->val.float64);
                val_type_pair->val.float32 = valf32;
                break;
            }
#endif
#if CHAR_BIT == 8
            case VariableType::boolean8:
#endif
            case VariableType::boolean16:
            case VariableType::boolean32:
#if SCRUTINY_SUPPORT_64BITS
            case VariableType::boolean64:
#endif
            case VariableType::boolean: // No size encoded
            {
                val_type_pair->valtype = BiggestUint;
                tools::set_biggest_uint(val_type_pair->val, static_cast<uint_biggest_t>(val_type_pair->val.boolean));
                break;
            }
#if CHAR_BIT == 8
            case VariableType::sint8:
            {
                val_type_pair->valtype = BiggestSint;
                tools::set_biggest_sint(val_type_pair->val, static_cast<uint_biggest_t>(val_type_pair->val.sint8));
                break;
            }
#endif
            case VariableType::sint16:
            {
                val_type_pair->valtype = BiggestSint;
                tools::set_biggest_sint(val_type_pair->val, static_cast<uint_biggest_t>(val_type_pair->val.sint16));
                break;
            }

            case VariableType::sint32:
            {
                val_type_pair->valtype = BiggestSint;
                tools::set_biggest_sint(val_type_pair->val, static_cast<uint_biggest_t>(val_type_pair->val.sint32));
                break;
            }
#if CHAR_BIT == 8
            case VariableType::uint8:
            {
                val_type_pair->valtype = BiggestUint;
                tools::set_biggest_uint(val_type_pair->val, static_cast<uint_biggest_t>(val_type_pair->val.uint8));
                break;
            }
#endif
            case VariableType::uint16:
            {
                val_type_pair->valtype = BiggestUint;
                tools::set_biggest_uint(val_type_pair->val, static_cast<uint_biggest_t>(val_type_pair->val.uint16));
                break;
            }

            case VariableType::uint32:
            {
                val_type_pair->valtype = BiggestUint;
                tools::set_biggest_uint(val_type_pair->val, static_cast<uint_biggest_t>(val_type_pair->val.uint32));
                break;
            }

            default:
                break;
            }
        }

        /// @brief Fetch an operand value from an operand definition
        /// @param main_handler A pointer to the MainHandler since memory access will go through it to respect forbidden regions
        /// @param operand The operand definition
        /// @param val The output value
        /// @param variable_type The output value data type
        /// @return true on success, false on failure
        bool fetch_operand(
            MainHandler const *const main_handler,
            Operand const *const operand,
            AnyValAndTypePair *const val_type_pair,
            LoopHandler *const caller)
        {
            bool success = true;
            if (operand->common.type == OperandType::Literal)
            {
                val_type_pair->val.float32 = operand->literal.val;
                val_type_pair->valtype = VariableType::float32;
            }
            else if (operand->common.type == OperandType::Rpv)
            {
                RuntimePublishedValue rpv;
                main_handler->get_rpv(operand->rpv.id, &rpv);
                success = main_handler->get_rpv_read_callback()(rpv, &val_type_pair->val, caller);
                val_type_pair->valtype = rpv.type;
            }
            else if (operand->common.type == OperandType::Var)
            {
                success = main_handler->fetch_variable(operand->var.addr, operand->var.datatype, &val_type_pair->val);
                val_type_pair->valtype = operand->var.datatype;
            }
            else if (operand->common.type == OperandType::VarBit)
            {
                success = main_handler->fetch_variable_bitfield(
                    operand->varbit.addr,
                    tools::get_var_type_type(operand->varbit.datatype),
                    operand->varbit.bitoffset,
                    operand->varbit.bitsize,
                    val_type_pair);
            }
            else
            {
                success = false;
            }

            if (!success)
            {
                memset(&val_type_pair->val, 0, sizeof(AnyType));
                val_type_pair->valtype = VariableType::float32;
            }

            return success;
        }
    } // namespace datalogging
} // namespace scrutiny
