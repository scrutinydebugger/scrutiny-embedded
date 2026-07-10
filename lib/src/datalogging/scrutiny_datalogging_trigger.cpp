//    scrutiny_datalogging_trigger.cpp
//        The implementation of the datalogging trigger conditions operators
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "datalogging/scrutiny_datalogging_trigger.hpp"
#include "datalogging/scrutiny_datalogging.hpp"
#include "datalogging/scrutiny_datalogging_types.hpp"
#include "scrutiny_main_handler.hpp"
#include "scrutiny_setup.hpp"
#include "scrutiny_tools.hpp"
#include <string.h>

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

namespace scrutiny
{
    namespace datalogging
    {
        namespace trigger
        {
            template <class T> bool apply_relational_op(RelationalOperator::eRelationalOperator cond, T arg1, T arg2)
            {
                switch (cond)
                {
                case RelationalOperator::Equal:
                    return (arg1 == arg2);
                case RelationalOperator::NotEqual:
                    return (arg1 != arg2);
                case RelationalOperator::GreaterThan:
                    return (arg1 > arg2);
                case RelationalOperator::GreaterOrEqualThan:
                    return (arg1 >= arg2);
                case RelationalOperator::LessThan:
                    return (arg1 < arg2);
                case RelationalOperator::LessOrEqualThan:
                    return (arg1 <= arg2);
                default:
                    return false;
                }
            }

            bool RelationalCompare(RelationalOperator::eRelationalOperator const op, AnyValAndTypeComparePair const operands[])
            {
                SCRUTINY_CONSTEXPR uint_biggest_t UINT_2_INT_MAX = static_cast<uint_biggest_t>(-1) >> 1;

                // Now our values are stored either in a float32 or an integer of the biggest supported types.
                // Number of type comparison will greatly be reduced
                if (operands[0].valtype == VariableTypeCompare::_float)
                {
                    if (operands[1].valtype == VariableTypeCompare::_float)
                    {
                        return apply_relational_op<float>(op, operands[0].val._float, operands[1].val._float);
                    }
                    else if (operands[1].valtype == VariableTypeCompare::_sint)
                    {
                        return apply_relational_op<float>(op, operands[0].val._float, static_cast<float>(operands[1].val._sint));
                    }
                    else if (operands[1].valtype == VariableTypeCompare::_uint)
                    {
                        return apply_relational_op<float>(op, operands[0].val._float, static_cast<float>(operands[1].val._uint));
                    }
                }
                else if (operands[0].valtype == VariableTypeCompare::_sint)
                {
                    if (operands[1].valtype == VariableTypeCompare::_float)
                    {
                        return apply_relational_op<float>(op, static_cast<float>(operands[0].val._sint), operands[1].val._float);
                    }
                    else if (operands[1].valtype == VariableTypeCompare::_sint)
                    {
                        return apply_relational_op<int_biggest_t>(op, operands[0].val._sint, operands[1].val._sint);
                    }
                    else if (operands[1].valtype == VariableTypeCompare::_uint)
                    {
                        return apply_relational_op<int_biggest_t>(
                            op,
                            operands[0].val._sint,
                            static_cast<int_biggest_t>((operands[1].val._uint > UINT_2_INT_MAX) ? UINT_2_INT_MAX : operands[1].val._uint));
                    }
                }
                else if (operands[0].valtype == VariableTypeCompare::_uint)
                {
                    if (operands[1].valtype == VariableTypeCompare::_float)
                    {
                        return apply_relational_op<float>(op, static_cast<float>(operands[0].val._uint), operands[1].val._float);
                    }
                    else if (operands[1].valtype == VariableTypeCompare::_sint)
                    {
                        return apply_relational_op<int_biggest_t>(
                            op,
                            static_cast<int_biggest_t>((operands[0].val._uint > UINT_2_INT_MAX) ? UINT_2_INT_MAX : operands[0].val._uint),
                            operands[1].val._sint);
                    }
                    else if (operands[1].valtype == VariableTypeCompare::_uint)
                    {
                        return apply_relational_op<uint_biggest_t>(op, operands[0].val._uint, operands[1].val._uint);
                    }
                }

                return false;
            } // namespace trigger

            bool EqualCondition::evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[])
            {
                static_cast<void>(data);
                return RelationalCompare(RelationalOperator::Equal, operands);
            }

            bool NotEqualCondition::evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[])
            {
                static_cast<void>(data);
                return RelationalCompare(RelationalOperator::NotEqual, operands);
            }

            bool GreaterThanCondition::evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[])
            {
                static_cast<void>(data);
                return RelationalCompare(RelationalOperator::GreaterThan, operands);
            }

            bool GreaterOrEqualThanCondition::evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[])
            {
                static_cast<void>(data);
                return RelationalCompare(RelationalOperator::GreaterOrEqualThan, operands);
            }

            bool LessThanCondition::evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[])
            {
                static_cast<void>(data);
                return RelationalCompare(RelationalOperator::LessThan, operands);
            }

            bool LessOrEqualThanCondition::evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[])
            {
                static_cast<void>(data);
                return RelationalCompare(RelationalOperator::LessOrEqualThan, operands);
            }

            bool ChangeMoreThanCondition::evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[])
            {
                // We can reasonably make the assumption that the delta will be a human-sized value.
                // Therefore, a float is adequate for it. Will avoid bloating this code for no reason
                float delta = 0;
                bool outval = false;
                if (operands[1].valtype == VariableTypeCompare::_float)
                {
                    delta = operands[1].val._float;
                }
                else if (operands[1].valtype == VariableTypeCompare::_uint)
                {
                    delta = static_cast<float>(operands[1].val._uint);
                }
                else if (operands[1].valtype == VariableTypeCompare::_sint)
                {
                    delta = static_cast<float>(operands[1].val._sint);
                }
                else
                {
                    return false;
                }

                if (data->cmt.initialized)
                {
                    if (operands[0].valtype == VariableTypeCompare::_uint)
                    {
                        if (delta >= 0)
                        {
                            outval = (operands[0].val._uint > data->cmt.previous_val._uint + static_cast<int_biggest_t>(delta));
                        }
                        else
                        {
                            outval = (operands[0].val._uint < data->cmt.previous_val._uint + static_cast<int_biggest_t>(delta));
                        }
                    }
                    else if (operands[0].valtype == VariableTypeCompare::_sint)
                    {
                        if (delta >= 0)
                        {
                            outval = (operands[0].val._sint > data->cmt.previous_val._sint + static_cast<int_biggest_t>(delta));
                        }
                        else
                        {
                            outval = (operands[0].val._sint < data->cmt.previous_val._sint + static_cast<int_biggest_t>(delta));
                        }
                    }
                    else if (operands[0].valtype == VariableTypeCompare::_float)
                    {
                        if (delta >= 0)
                        {
                            outval = (operands[0].val._float > data->cmt.previous_val._float + delta);
                        }
                        else
                        {
                            outval = (operands[0].val._float < data->cmt.previous_val._float + delta);
                        }
                    }
                }
                else
                {
                    data->cmt.initialized = true;
                }

                memcpy(&data->cmt.previous_val, &operands[0].val, sizeof(data->cmt.previous_val));

                return outval;
            }

            bool IsWithinCondition::evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[])
            {
                static_cast<void>(data);
                float operands_vals_float[3];

                for (uint_fast8_t i = 0; i < 3; i++)
                {
                    if (operands[i].valtype == VariableTypeCompare::_uint)
                    {
                        operands_vals_float[i] = static_cast<float>(operands[i].val._uint);
                    }
                    else if (operands[i].valtype == VariableTypeCompare::_sint)
                    {
                        operands_vals_float[i] = static_cast<float>(operands[i].val._sint);
                    }
                    else if (operands[i].valtype == VariableTypeCompare::_float)
                    {
                        operands_vals_float[i] = operands[i].val._float;
                    }
                    else
                    {
                        return false;
                    }
                }
                float const diffabs = SCRUTINY_FABS(operands_vals_float[0] - operands_vals_float[1]);
                float const margin = SCRUTINY_FABS(operands_vals_float[2]);
                return diffabs <= margin;
            }

            bool AlwaysTrueCondition::evaluate(ConditionSharedData *const data, AnyValAndTypeComparePair const operands[])
            {
                static_cast<void>(data);
                static_cast<void>(operands);
                return true;
            }
        } // namespace trigger
    }     // namespace datalogging
} // namespace scrutiny
