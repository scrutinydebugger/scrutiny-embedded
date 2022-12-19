//    scrutiny_datalogging_trigger.cpp
//        The implementation of the datalogging trigger conditions operators
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include "scrutiny_tools.hpp"
#include "scrutiny_main_handler.hpp"
#include "datalogging/scrutiny_datalogging.hpp"
#include "datalogging/scrutiny_datalogging_trigger.hpp"
#include "datalogging/scrutiny_datalogging_types.hpp"

#include "string.h"

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
                template <class T1, class T2>
                class eq
                {
                public:
                    static inline bool eval(const T1 v1, const T2 v2)
                    {
                        return v1 == v2;
                    }
                };

                template <class T1, class T2>
                class neq
                {
                public:
                    static inline bool eval(const T1 v1, const T2 v2)
                    {
                        return v1 != v2;
                    }
                };

                template <class T1, class T2>
                class gt
                {
                public:
                    static inline bool eval(const T1 v1, const T2 v2)
                    {
                        return v1 > v2;
                    }
                };

                template <class T1, class T2>
                class get
                {
                public:
                    static inline bool eval(const T1 v1, const T2 v2)
                    {
                        return v1 >= v2;
                    }
                };

                template <class T1, class T2>
                class lt
                {
                public:
                    static inline bool eval(const T1 v1, const T2 v2)
                    {
                        return v1 < v2;
                    }
                };

                template <class T1, class T2>
                class let
                {
                public:
                    static inline bool eval(const T1 v1, const T2 v2)
                    {
                        return v1 <= v2;
                    }
                };

            }

            template <template <class, class> class OPERATOR>
            bool RelationalCompare(const VariableType operand_types[], const AnyTypeCompare operand_vals[])
            {
                // Now our values are stored either in a float32 or an integer of the biggest supported types.
                // Number of type comparison will greatly be reduced
                if (operand_types[0] == VariableType::float32)
                {
                    if (operand_types[1] == VariableType::float32)
                        return OPERATOR<float, float>::eval(operand_vals[0]._float, operand_vals[1]._float);
                    else if (operand_types[1] == BiggestSint)
                        return OPERATOR<float, float>::eval(operand_vals[0]._float, static_cast<float>(operand_vals[1]._sint));
                    else if (operand_types[1] == BiggestUint)
                        return OPERATOR<float, float>::eval(operand_vals[0]._float, static_cast<float>(operand_vals[1]._uint));
                }
                else if (operand_types[0] == BiggestSint)
                {
                    if (operand_types[1] == VariableType::float32)
                        return OPERATOR<float, float>::eval(static_cast<float>(operand_vals[0]._sint), operand_vals[1]._float);
                    else if (operand_types[1] == BiggestSint)
                        return OPERATOR<int_biggest_t, int_biggest_t>::eval(operand_vals[0]._sint, operand_vals[1]._sint);
                    else if (operand_types[1] == BiggestUint)
                        return OPERATOR<int_biggest_t, int_biggest_t>::eval(operand_vals[0]._sint, static_cast<int_biggest_t>(operand_vals[1]._uint));
                }
                else if (operand_types[0] == BiggestUint)
                {
                    if (operand_types[1] == VariableType::float32)
                        return OPERATOR<float, float>::eval(static_cast<float>(operand_vals[0]._uint), operand_vals[1]._float);
                    else if (operand_types[1] == BiggestSint)
                        return OPERATOR<int_biggest_t, int_biggest_t>::eval(static_cast<int_biggest_t>(operand_vals[0]._uint), operand_vals[1]._sint);
                    else if (operand_types[1] == BiggestUint)
                        return OPERATOR<uint_biggest_t, uint_biggest_t>::eval(operand_vals[0]._uint, operand_vals[1]._uint);
                }

                return false;
            }

            bool EqualCondition::evaluate(const VariableType operand_types[], const AnyTypeCompare operand_vals[]) const
            {
                return RelationalCompare<relational_operators::eq>(operand_types, operand_vals);
            }

            bool NotEqualCondition::evaluate(const VariableType operand_types[], const AnyTypeCompare operand_vals[]) const
            {
                return RelationalCompare<relational_operators::neq>(operand_types, operand_vals);
            }

            bool GreaterThanCondition::evaluate(const VariableType operand_types[], const AnyTypeCompare operand_vals[]) const
            {
                return RelationalCompare<relational_operators::gt>(operand_types, operand_vals);
            }

            bool GreaterOrEqualThanCondition::evaluate(const VariableType operand_types[], const AnyTypeCompare operand_vals[]) const
            {
                return RelationalCompare<relational_operators::get>(operand_types, operand_vals);
            }

            bool LessThanCondition::evaluate(const VariableType operand_types[], const AnyTypeCompare operand_vals[]) const
            {
                return RelationalCompare<relational_operators::lt>(operand_types, operand_vals);
            }

            bool LessOrEqualThanCondition::evaluate(const VariableType operand_types[], const AnyTypeCompare operand_vals[]) const
            {
                return RelationalCompare<relational_operators::let>(operand_types, operand_vals);
            }

        }
    }
}
