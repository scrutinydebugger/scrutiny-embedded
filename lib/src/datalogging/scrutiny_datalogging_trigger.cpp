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

                datalogging::fetch_operand(mh, op1, &opvals[0], &optypes[0]);
                datalogging::fetch_operand(mh, op2, &opvals[1], &optypes[1]);

                // Remove some possibilities for the template below.
                // If the union layout allows it, the compiler should be able to optimize this pretty well.
                for (int i = 0; i < 2; i++)
                {
                    switch (optypes[i])
                    {
                    case VariableType::float64:
                        optypes[i] = VariableType::float32;
                        opvals[i].float32 = static_cast<float>(opvals[i].float64);
                        break;

                    case VariableType::boolean:
                        optypes[i] = VariableType::uint64;
                        opvals[i].uint64 = static_cast<uint64_t>(opvals[i].boolean);
                        break;

                    case VariableType::sint8:
                        optypes[i] = VariableType::sint64;
                        opvals[i].sint64 = static_cast<int64_t>(opvals[i].sint8);
                        break;

                    case VariableType::sint16:
                        optypes[i] = VariableType::sint64;
                        opvals[i].sint64 = static_cast<int64_t>(opvals[i].sint16);
                        break;

                    case VariableType::sint32:
                        optypes[i] = VariableType::sint64;
                        opvals[i].sint64 = static_cast<int64_t>(opvals[i].sint32);
                        break;

                    case VariableType::uint8:
                        optypes[i] = VariableType::uint64;
                        opvals[i].uint64 = static_cast<uint64_t>(opvals[i].uint8);
                        break;

                    case VariableType::uint16:
                        optypes[i] = VariableType::uint64;
                        opvals[i].uint64 = static_cast<uint64_t>(opvals[i].uint16);
                        break;

                    case VariableType::uint32:
                        optypes[i] = VariableType::uint64;
                        opvals[i].uint64 = static_cast<uint64_t>(opvals[i].uint32);
                        break;

                    default:
                        break;
                    }
                }

                // We only have to check float32, uint64 and sint64
                if (optypes[0] == VariableType::float32)
                {
                    if (optypes[1] == VariableType::float32)
                        return OPERATOR<float, float>::eval(opvals[0].float32, opvals[1].float32);
                    else if (optypes[1] == VariableType::sint64)
                        return OPERATOR<float, float>::eval(opvals[0].float32, static_cast<float>(opvals[1].sint64));
                    else if (optypes[1] == VariableType::uint64)
                        return OPERATOR<float, float>::eval(opvals[0].float32, static_cast<float>(opvals[1].uint64));
                }
                else if (optypes[0] == VariableType::sint64)
                {
                    if (optypes[1] == VariableType::float32)
                        return OPERATOR<float, float>::eval(static_cast<float>(opvals[0].sint64), opvals[1].float32);
                    else if (optypes[1] == VariableType::sint64)
                        return OPERATOR<int64_t, int64_t>::eval(opvals[0].sint64, opvals[1].sint64);
                    else if (optypes[1] == VariableType::uint64)
                        return OPERATOR<int64_t, int64_t>::eval(opvals[0].sint64, static_cast<int64_t>(opvals[1].uint64));
                }
                else if (optypes[0] == VariableType::uint64)
                {
                    if (optypes[1] == VariableType::float32)
                        return OPERATOR<float, float>::eval(static_cast<float>(opvals[0].uint64), opvals[1].float32);
                    else if (optypes[1] == VariableType::sint64)
                        return OPERATOR<int64_t, int64_t>::eval(static_cast<int64_t>(opvals[0].uint64), opvals[1].sint64);
                    else if (optypes[1] == VariableType::uint64)
                        return OPERATOR<uint64_t, uint64_t>::eval(opvals[0].uint64, opvals[1].uint64);
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
