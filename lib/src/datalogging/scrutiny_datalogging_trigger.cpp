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
            namespace relational_operators
            {
                template <class T1, class T2> class eq
                {
                  public:
                    static inline bool eval(T1 const v1, T2 const v2) { return v1 == v2; }
                };

                template <class T1, class T2> class neq
                {
                  public:
                    static inline bool eval(T1 const v1, T2 const v2) { return v1 != v2; }
                };

                template <class T1, class T2> class gt
                {
                  public:
                    static inline bool eval(T1 const v1, T2 const v2) { return v1 > v2; }
                };

                template <class T1, class T2> class get
                {
                  public:
                    static inline bool eval(T1 const v1, T2 const v2) { return v1 >= v2; }
                };

                template <class T1, class T2> class lt
                {
                  public:
                    static inline bool eval(T1 const v1, T2 const v2) { return v1 < v2; }
                };

                template <class T1, class T2> class let
                {
                  public:
                    static inline bool eval(T1 const v1, T2 const v2) { return v1 <= v2; }
                };

            } // namespace relational_operators

            template <template <class, class> class OPERATOR>
            bool RelationalCompare(VariableTypeCompare::eVariableTypeCompare const operand_types[], AnyTypeCompare const operand_vals[])
            {
                SCRUTINY_CONSTEXPR uint_biggest_t UINT_2_INT_MAX = static_cast<uint_biggest_t>(-1) >> 1;

                // Now our values are stored either in a float32 or an integer of the biggest supported types.
                // Number of type comparison will greatly be reduced
                if (operand_types[0] == VariableTypeCompare::_float)
                {
                    if (operand_types[1] == VariableTypeCompare::_float)
                    {
                        return OPERATOR<float, float>::eval(operand_vals[0]._float, operand_vals[1]._float);
                    }
                    else if (operand_types[1] == VariableTypeCompare::_sint)
                    {
                        return OPERATOR<float, float>::eval(operand_vals[0]._float, static_cast<float>(operand_vals[1]._sint));
                    }
                    else if (operand_types[1] == VariableTypeCompare::_uint)
                    {
                        return OPERATOR<float, float>::eval(operand_vals[0]._float, static_cast<float>(operand_vals[1]._uint));
                    }
                }
                else if (operand_types[0] == VariableTypeCompare::_sint)
                {
                    if (operand_types[1] == VariableTypeCompare::_float)
                    {
                        return OPERATOR<float, float>::eval(static_cast<float>(operand_vals[0]._sint), operand_vals[1]._float);
                    }
                    else if (operand_types[1] == VariableTypeCompare::_sint)
                    {
                        return OPERATOR<int_biggest_t, int_biggest_t>::eval(operand_vals[0]._sint, operand_vals[1]._sint);
                    }
                    else if (operand_types[1] == VariableTypeCompare::_uint)
                    {
                        return OPERATOR<int_biggest_t, int_biggest_t>::eval(
                            operand_vals[0]._sint,
                            static_cast<int_biggest_t>((operand_vals[1]._uint > UINT_2_INT_MAX) ? UINT_2_INT_MAX : operand_vals[1]._uint));
                    }
                }
                else if (operand_types[0] == VariableTypeCompare::_uint)
                {
                    if (operand_types[1] == VariableTypeCompare::_float)
                    {
                        return OPERATOR<float, float>::eval(static_cast<float>(operand_vals[0]._uint), operand_vals[1]._float);
                    }
                    else if (operand_types[1] == VariableTypeCompare::_sint)
                    {
                        return OPERATOR<int_biggest_t, int_biggest_t>::eval(
                            static_cast<int_biggest_t>((operand_vals[0]._uint > UINT_2_INT_MAX) ? UINT_2_INT_MAX : operand_vals[0]._uint),
                            operand_vals[1]._sint);
                    }
                    else if (operand_types[1] == VariableTypeCompare::_uint)
                    {
                        return OPERATOR<uint_biggest_t, uint_biggest_t>::eval(operand_vals[0]._uint, operand_vals[1]._uint);
                    }
                }

                return false;
            }

            bool EqualCondition::evaluate(
                ConditionSharedData *const data,
                VariableTypeCompare::eVariableTypeCompare const operand_types[],
                AnyTypeCompare const operand_vals[])
            {
                static_cast<void>(data);
                return RelationalCompare<relational_operators::eq>(operand_types, operand_vals);
            }

            bool NotEqualCondition::evaluate(
                ConditionSharedData *const data,
                VariableTypeCompare::eVariableTypeCompare const operand_types[],
                AnyTypeCompare const operand_vals[])
            {
                static_cast<void>(data);
                return RelationalCompare<relational_operators::neq>(operand_types, operand_vals);
            }

            bool GreaterThanCondition::evaluate(
                ConditionSharedData *const data,
                VariableTypeCompare::eVariableTypeCompare const operand_types[],
                AnyTypeCompare const operand_vals[])
            {
                static_cast<void>(data);
                return RelationalCompare<relational_operators::gt>(operand_types, operand_vals);
            }

            bool GreaterOrEqualThanCondition::evaluate(
                ConditionSharedData *const data,
                VariableTypeCompare::eVariableTypeCompare const operand_types[],
                AnyTypeCompare const operand_vals[])
            {
                static_cast<void>(data);
                return RelationalCompare<relational_operators::get>(operand_types, operand_vals);
            }

            bool LessThanCondition::evaluate(
                ConditionSharedData *const data,
                VariableTypeCompare::eVariableTypeCompare const operand_types[],
                AnyTypeCompare const operand_vals[])
            {
                static_cast<void>(data);
                return RelationalCompare<relational_operators::lt>(operand_types, operand_vals);
            }

            bool LessOrEqualThanCondition::evaluate(
                ConditionSharedData *const data,
                VariableTypeCompare::eVariableTypeCompare const operand_types[],
                AnyTypeCompare const operand_vals[])
            {
                static_cast<void>(data);
                return RelationalCompare<relational_operators::let>(operand_types, operand_vals);
            }

            bool ChangeMoreThanCondition::evaluate(
                ConditionSharedData *const data,
                VariableTypeCompare::eVariableTypeCompare const operand_types[],
                AnyTypeCompare const operand_vals[])
            {
                // We can reasonably make the assumption that the delta will be a human-sized value.
                // Therefore, a float is adequate for it. Will avoid bloating this code for no reason
                float delta = 0;
                bool outval = false;
                if (operand_types[1] == VariableTypeCompare::_float)
                {
                    delta = operand_vals[1]._float;
                }
                else if (operand_types[1] == VariableTypeCompare::_uint)
                {
                    delta = static_cast<float>(operand_vals[1]._uint);
                }
                else if (operand_types[1] == VariableTypeCompare::_sint)
                {
                    delta = static_cast<float>(operand_vals[1]._sint);
                }
                else
                {
                    return false;
                }

                if (data->cmt.initialized)
                {
                    if (operand_types[0] == VariableTypeCompare::_uint)
                    {
                        if (delta >= 0)
                        {
                            outval = (operand_vals[0]._uint > data->cmt.previous_val._uint + static_cast<int_biggest_t>(delta));
                        }
                        else
                        {
                            outval = (operand_vals[0]._uint < data->cmt.previous_val._uint + static_cast<int_biggest_t>(delta));
                        }
                    }
                    else if (operand_types[0] == VariableTypeCompare::_sint)
                    {
                        if (delta >= 0)
                        {
                            outval = (operand_vals[0]._sint > data->cmt.previous_val._sint + static_cast<int_biggest_t>(delta));
                        }
                        else
                        {
                            outval = (operand_vals[0]._sint < data->cmt.previous_val._sint + static_cast<int_biggest_t>(delta));
                        }
                    }
                    else if (operand_types[0] == VariableTypeCompare::_float)
                    {
                        if (delta >= 0)
                        {
                            outval = (operand_vals[0]._float > data->cmt.previous_val._float + delta);
                        }
                        else
                        {
                            outval = (operand_vals[0]._float < data->cmt.previous_val._float + delta);
                        }
                    }
                }
                else
                {
                    data->cmt.initialized = true;
                }

                memcpy(&data->cmt.previous_val, &operand_vals[0], sizeof(data->cmt.previous_val));

                return outval;
            }

            bool IsWithinCondition::evaluate(
                ConditionSharedData *const data,
                VariableTypeCompare::eVariableTypeCompare const operand_types[],
                AnyTypeCompare const operand_vals[])
            {
                static_cast<void>(data);
                float operands_vals_float[3];

                for (uint_fast8_t i = 0; i < 3; i++)
                {
                    if (operand_types[i] == VariableTypeCompare::_uint)
                    {
                        operands_vals_float[i] = static_cast<float>(operand_vals[i]._uint);
                    }
                    else if (operand_types[i] == VariableTypeCompare::_sint)
                    {
                        operands_vals_float[i] = static_cast<float>(operand_vals[i]._sint);
                    }
                    else if (operand_types[i] == VariableTypeCompare::_float)
                    {
                        operands_vals_float[i] = operand_vals[i]._float;
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

            bool AlwaysTrueCondition::evaluate(
                ConditionSharedData *const data,
                VariableTypeCompare::eVariableTypeCompare const operand_types[],
                AnyTypeCompare const operand_vals[])
            {
                static_cast<void>(data);
                static_cast<void>(operand_types);
                static_cast<void>(operand_vals);
                return true;
            }
        } // namespace trigger
    }     // namespace datalogging
} // namespace scrutiny
