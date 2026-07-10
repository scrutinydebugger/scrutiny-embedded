//    test_datalogging_types.cpp
//        Run some tests on datalogging types
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

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
    scrutiny::AnyValAndTypePair vpair;

#if CHAR_BIT == 8
    vpair.valtype = scrutiny::VariableType::sint8;
    vpair.val.sint8 = -50;
    scrutiny::datalogging::convert_to_compare_type(&vpair);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyValAndTypeComparePair *>(&vpair)->val._sint, -50);
#endif

    vpair.valtype = scrutiny::VariableType::sint16;
    vpair.val.sint16 = -1000;
    scrutiny::datalogging::convert_to_compare_type(&vpair);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyValAndTypeComparePair *>(&vpair)->val._sint, -1000);

    vpair.valtype = scrutiny::VariableType::sint32;
    vpair.val.sint32 = -100000;
    scrutiny::datalogging::convert_to_compare_type(&vpair);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyValAndTypeComparePair *>(&vpair)->val._sint, -100000);

#if CHAR_BIT == 8
    vpair.valtype = scrutiny::VariableType::uint8;
    vpair.val.uint8 = 50;
    scrutiny::datalogging::convert_to_compare_type(&vpair);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyValAndTypeComparePair *>(&vpair)->val._uint, 50u);
#endif

    vpair.valtype = scrutiny::VariableType::uint16;
    vpair.val.uint16 = 1000;
    scrutiny::datalogging::convert_to_compare_type(&vpair);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyValAndTypeComparePair *>(&vpair)->val._uint, 1000u);

    vpair.valtype = scrutiny::VariableType::uint32;
    vpair.val.uint32 = 100000;
    scrutiny::datalogging::convert_to_compare_type(&vpair);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyValAndTypeComparePair *>(&vpair)->val._uint, 100000u);

#if SCRUTINY_SUPPORT_64BITS
    vpair.valtype = scrutiny::VariableType::uint64;
    vpair.val.uint64 = 0x123456789ABC;
    scrutiny::datalogging::convert_to_compare_type(&vpair);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyValAndTypeComparePair *>(&vpair)->val._uint, 0x123456789ABCu);

    vpair.valtype = scrutiny::VariableType::sint64;
    vpair.val.sint64 = -0x123456789ABC;
    scrutiny::datalogging::convert_to_compare_type(&vpair);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyValAndTypeComparePair *>(&vpair)->val._sint, -0x123456789ABC);

    vpair.valtype = scrutiny::VariableType::float64;
    vpair.val.float64 = 3.1415926;
    scrutiny::datalogging::convert_to_compare_type(&vpair);
    EXPECT_EQ(reinterpret_cast<scrutiny::datalogging::AnyValAndTypeComparePair *>(&vpair)->val._float, 3.1415926f);
#endif
}
