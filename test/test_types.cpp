//    test_types.cpp
//        Some tests dones on scrutiny types
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include <gtest/gtest.h>
#include "scrutiny.hpp"

TEST(TestTypes, TestSize)
{
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::sint8), 1);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::sint16), 2);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::sint32), 4);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::sint64), 8);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::sint128), 16);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::sint256), 32);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::uint8), 1);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::uint16), 2);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::uint32), 4);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::uint64), 8);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::uint128), 16);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::uint256), 32);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::float8), 1);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::float16), 2);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::float32), 4);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::float64), 8);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::float128), 16);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::float256), 32);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::cfloat8), 1);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::cfloat16), 2);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::cfloat32), 4);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::cfloat64), 8);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::cfloat128), 16);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::cfloat256), 32);
    ASSERT_EQ(scrutiny::tools::get_type_size(scrutiny::VariableType::boolean), 1);
}

TEST(TestTypes, TestTypeType)
{
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::sint8), scrutiny::VariableTypeType::_sint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::sint16), scrutiny::VariableTypeType::_sint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::sint32), scrutiny::VariableTypeType::_sint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::sint64), scrutiny::VariableTypeType::_sint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::sint128), scrutiny::VariableTypeType::_sint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::sint256), scrutiny::VariableTypeType::_sint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::uint8), scrutiny::VariableTypeType::_uint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::uint16), scrutiny::VariableTypeType::_uint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::uint32), scrutiny::VariableTypeType::_uint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::uint64), scrutiny::VariableTypeType::_uint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::uint128), scrutiny::VariableTypeType::_uint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::uint256), scrutiny::VariableTypeType::_uint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::float8), scrutiny::VariableTypeType::_float);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::float16), scrutiny::VariableTypeType::_float);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::float32), scrutiny::VariableTypeType::_float);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::float64), scrutiny::VariableTypeType::_float);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::float128), scrutiny::VariableTypeType::_float);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::float256), scrutiny::VariableTypeType::_float);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::cfloat8), scrutiny::VariableTypeType::_cfloat);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::cfloat16), scrutiny::VariableTypeType::_cfloat);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::cfloat32), scrutiny::VariableTypeType::_cfloat);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::cfloat64), scrutiny::VariableTypeType::_cfloat);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::cfloat128), scrutiny::VariableTypeType::_cfloat);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::cfloat256), scrutiny::VariableTypeType::_cfloat);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::boolean), scrutiny::VariableTypeType::_boolean);
}

TEST(TestTypes, TestMakeType)
{
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_sint, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::sint8);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_sint, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::sint16);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_sint, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::sint32);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_sint, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::sint64);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_sint, scrutiny::VariableTypeSize::_128), scrutiny::VariableType::sint128);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_sint, scrutiny::VariableTypeSize::_256), scrutiny::VariableType::sint256);

    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_uint, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::uint8);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_uint, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::uint16);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_uint, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::uint32);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_uint, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::uint64);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_uint, scrutiny::VariableTypeSize::_128), scrutiny::VariableType::uint128);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_uint, scrutiny::VariableTypeSize::_256), scrutiny::VariableType::uint256);

    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_float, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::float8);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_float, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::float16);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_float, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::float32);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_float, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::float64);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_float, scrutiny::VariableTypeSize::_128), scrutiny::VariableType::float128);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_float, scrutiny::VariableTypeSize::_256), scrutiny::VariableType::float256);

    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_cfloat, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::cfloat8);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_cfloat, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::cfloat16);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_cfloat, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::cfloat32);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_cfloat, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::cfloat64);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_cfloat, scrutiny::VariableTypeSize::_128), scrutiny::VariableType::cfloat128);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_cfloat, scrutiny::VariableTypeSize::_256), scrutiny::VariableType::cfloat256);

    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_boolean, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::boolean);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_boolean, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::unknown);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_boolean, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::unknown);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_boolean, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::unknown);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_boolean, scrutiny::VariableTypeSize::_128), scrutiny::VariableType::unknown);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_boolean, scrutiny::VariableTypeSize::_256), scrutiny::VariableType::unknown);
}
