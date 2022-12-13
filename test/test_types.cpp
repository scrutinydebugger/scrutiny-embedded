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
