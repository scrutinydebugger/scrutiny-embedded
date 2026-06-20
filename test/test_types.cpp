//    test_types.cpp
//        Some tests done on scrutiny types
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny.hpp"
#include "scrutinytest/scrutinytest.hpp"

TEST(TestTypes, TestSize)
{
#if CHAR_BIT == 8
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::sint8), 1);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::uint8), 1);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::float8), 1);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::cfloat8), 1);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::boolean8), 1);
#endif
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::sint16), 2);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::uint16), 2);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::float16), 2);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::cfloat16), 2);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::boolean16), 2);

    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::sint32), 4);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::uint32), 4);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::float32), 4);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::cfloat32), 4);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::boolean32), 4);
#if SCRUTINY_SUPPORT_64BITS
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::sint64), 8);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::uint64), 8);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::float64), 8);
    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::cfloat64), 8);
#endif

    ASSERT_EQ(scrutiny::tools::get_type_size_8bits(scrutiny::VariableType::boolean), scrutiny::tools::get_platform_boolean_size_8bits());
}

TEST(TestTypes, TestTypeType)
{
#if CHAR_BIT == 8
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::sint8), scrutiny::VariableTypeType::_sint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::uint8), scrutiny::VariableTypeType::_uint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::float8), scrutiny::VariableTypeType::_float);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::cfloat8), scrutiny::VariableTypeType::_cfloat);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::boolean8), scrutiny::VariableTypeType::_boolean);
#endif
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::sint16), scrutiny::VariableTypeType::_sint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::uint16), scrutiny::VariableTypeType::_uint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::float16), scrutiny::VariableTypeType::_float);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::cfloat16), scrutiny::VariableTypeType::_cfloat);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::boolean16), scrutiny::VariableTypeType::_boolean);

    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::sint32), scrutiny::VariableTypeType::_sint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::uint32), scrutiny::VariableTypeType::_uint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::float32), scrutiny::VariableTypeType::_float);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::cfloat32), scrutiny::VariableTypeType::_cfloat);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::boolean32), scrutiny::VariableTypeType::_boolean);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::boolean16), scrutiny::VariableTypeType::_boolean);

#if SCRUTINY_SUPPORT_64BITS
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::uint64), scrutiny::VariableTypeType::_uint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::sint64), scrutiny::VariableTypeType::_sint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::float64), scrutiny::VariableTypeType::_float);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::cfloat64), scrutiny::VariableTypeType::_cfloat);
#endif

    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::boolean), scrutiny::VariableTypeType::_boolean);
}

TEST(TestTypes, TestMakeType)
{
#if CHAR_BIT == 8
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_sint, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::sint8);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_uint, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::uint8);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_float, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::float8);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_cfloat, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::cfloat8);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_boolean, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::boolean8);
#endif

    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_sint, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::sint16);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_uint, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::uint16);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_float, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::float16);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_cfloat, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::cfloat16);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_boolean, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::boolean16);

    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_sint, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::sint32);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_uint, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::uint32);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_float, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::float32);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_cfloat, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::cfloat32);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_boolean, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::boolean32);

#if SCRUTINY_SUPPORT_64BITS
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_sint, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::sint64);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_uint, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::uint64);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_float, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::float64);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_cfloat, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::cfloat64);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_boolean, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::boolean64);
#endif
}
