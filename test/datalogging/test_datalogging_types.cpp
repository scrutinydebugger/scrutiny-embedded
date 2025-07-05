//    test_datalogging_types.cpp
//        Run some tests on datalogging types
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny.hpp"
#include "scrutinytest/scrutinytest.hpp"

TEST(TestDataLoggingTypes, AnyTypeCompareCast)
{
    scrutiny::AnyType v;
    memset(&v, 55, sizeof(scrutiny::AnyType));

    v.float32 = 3.1415926f;
    EXPECT_EQ(v.float32, reinterpret_cast<scrutiny::datalogging::AnyTypeCompare *>(&v)->_float);

#if SCRUTINY_SUPPORT_64BITS
    v.sint64 = -10000000;
    EXPECT_EQ(v.sint64, reinterpret_cast<scrutiny::datalogging::AnyTypeCompare *>(&v)->_sint);
    v.uint64 = 1000000;
    EXPECT_EQ(v.uint64, reinterpret_cast<scrutiny::datalogging::AnyTypeCompare *>(&v)->_uint);
#else
    v.sint32 = -10000000;
    EXPECT_EQ(v.sint32, reinterpret_cast<scrutiny::datalogging::AnyTypeCompare *>(&v)->_sint);
    v.uint32 = 1000000;
    EXPECT_EQ(v.uint32, reinterpret_cast<scrutiny::datalogging::AnyTypeCompare *>(&v)->_uint);
#endif
}

TEST(TestDataLoggingTypes, ConvertToCompareType)
{
    scrutiny::VariableType::eVariableType vtype;
    scrutiny::AnyType v;

    vtype = scrutiny::VariableType::sint8;
    v.sint8 = -50;
    scrutiny::datalogging::convert_to_compare_type(&vtype, &v);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyTypeCompare *>(&v)->_sint, -50);

    vtype = scrutiny::VariableType::sint16;
    v.sint16 = -1000;
    scrutiny::datalogging::convert_to_compare_type(&vtype, &v);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyTypeCompare *>(&v)->_sint, -1000);

    vtype = scrutiny::VariableType::sint32;
    v.sint32 = -100000;
    scrutiny::datalogging::convert_to_compare_type(&vtype, &v);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyTypeCompare *>(&v)->_sint, -100000);

    vtype = scrutiny::VariableType::uint8;
    v.uint8 = 50;
    scrutiny::datalogging::convert_to_compare_type(&vtype, &v);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyTypeCompare *>(&v)->_uint, 50u);

    vtype = scrutiny::VariableType::uint16;
    v.uint16 = 1000;
    scrutiny::datalogging::convert_to_compare_type(&vtype, &v);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyTypeCompare *>(&v)->_uint, 1000u);

    vtype = scrutiny::VariableType::uint32;
    v.uint32 = 100000;
    scrutiny::datalogging::convert_to_compare_type(&vtype, &v);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyTypeCompare *>(&v)->_uint, 100000u);

#if SCRUTINY_SUPPORT_64BITS
    vtype = scrutiny::VariableType::uint64;
    v.uint64 = 0x123456789ABC;
    scrutiny::datalogging::convert_to_compare_type(&vtype, &v);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyTypeCompare *>(&v)->_uint, 0x123456789ABCu);

    vtype = scrutiny::VariableType::sint64;
    v.sint64 = -0x123456789ABC;
    scrutiny::datalogging::convert_to_compare_type(&vtype, &v);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyTypeCompare *>(&v)->_sint, -0x123456789ABC);

    vtype = scrutiny::VariableType::float64;
    v.float64 = 3.1415926;
    scrutiny::datalogging::convert_to_compare_type(&vtype, &v);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyTypeCompare *>(&v)->_float, 3.1415926f);
#endif
}
