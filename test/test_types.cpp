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
