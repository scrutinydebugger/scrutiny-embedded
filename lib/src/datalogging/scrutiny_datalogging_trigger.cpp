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
            bool RelationalCompare(const MainHandler *const mh, const Operand *const op1, const Operand *const op2)
            {
                VariableType optypes[2];
                AnyType opvals[2];

                if (datalogging::fetch_operand(mh, op1, &opvals[0], &optypes[0]) == false)
                {
                    return false;
                }

                if (datalogging::fetch_operand(mh, op2, &opvals[1], &optypes[1]) == false)
                {
                    return false;
                }

                // Remove some possibilities for the template below.
                for (int i = 0; i < 2; i++)
                {
                    tools::ConvertValueToBiggestFormat(&optypes[i], &opvals[i]);
                }

                // Now our values are stored either in a float32 or an integer of the biggest supported types.
                // Number of type comparison will greatly be reduced
                if (optypes[0] == VariableType::float32)
                {
                    if (optypes[1] == VariableType::float32)
                        return OPERATOR<float, float>::eval(
                            opvals[0].float32,
                            opvals[1].float32);
                    else if (optypes[1] == BiggestSint)
                        return OPERATOR<float, float>::eval(
                            opvals[0].float32,
                            static_cast<float>(tools::read_biggest_sint(opvals[1])));
                    else if (optypes[1] == BiggestUint)
                        return OPERATOR<float, float>::eval(
                            opvals[0].float32,
                            static_cast<float>(tools::read_biggest_uint(opvals[1])));
                }
                else if (optypes[0] == BiggestSint)
                {
                    if (optypes[1] == VariableType::float32)
                        return OPERATOR<float, float>::eval(
                            static_cast<float>(tools::read_biggest_sint(opvals[0])),
                            opvals[1].float32);
                    else if (optypes[1] == BiggestSint)
                        return OPERATOR<int_biggest_t, int_biggest_t>::eval(
                            tools::read_biggest_sint(opvals[0]),
                            tools::read_biggest_sint(opvals[1]));
                    else if (optypes[1] == BiggestUint)
                        return OPERATOR<int_biggest_t, int_biggest_t>::eval(
                            tools::read_biggest_sint(opvals[0]),
                            static_cast<int_biggest_t>(tools::read_biggest_uint(opvals[1])));
                }
                else if (optypes[0] == BiggestUint)
                {
                    if (optypes[1] == VariableType::float32)
                        return OPERATOR<float, float>::eval(
                            static_cast<float>(tools::read_biggest_uint(opvals[0])),
                            opvals[1].float32);
                    else if (optypes[1] == BiggestSint)
                        return OPERATOR<int_biggest_t, int_biggest_t>::eval(
                            static_cast<int_biggest_t>(tools::read_biggest_uint(opvals[0])),
                            tools::read_biggest_sint(opvals[1]));
                    else if (optypes[1] == BiggestUint)
                        return OPERATOR<uint_biggest_t, uint_biggest_t>::eval(
                            tools::read_biggest_uint(opvals[0]),
                            tools::read_biggest_uint(opvals[1]));
                }

                return false;
            }

            bool EqualCondition::evaluate(const MainHandler *const mh, const Operand *const op1, const Operand *const op2) const
            {
                return RelationalCompare<relational_operators::eq>(mh, op1, op2);
            }
        }
    }
}
