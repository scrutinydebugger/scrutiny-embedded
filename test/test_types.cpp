//    test_types.cpp
//        Some tests dones on scrutiny types
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny.hpp"
#include "scrutinytest/scrutinytest.hpp"

TEST(TestTypes, TestSize)
{
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::sint8), 1);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::sint16), 2);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::sint32), 4);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::uint8), 1);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::uint16), 2);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::uint32), 4);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::float8), 1);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::float16), 2);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::float32), 4);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::cfloat8), 1);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::cfloat16), 2);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::cfloat32), 4);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::boolean), 1);
#if SCRUTINY_SUPPORT_64BITS
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::sint64), 8);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::uint64), 8);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::float64), 8);
    ASSERT_EQ(scrutiny::tools::get_type_size_u8(scrutiny::VariableType::cfloat64), 8);
#endif

    if (CHAR_BIT == 1)
    {
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::sint8), 1);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::sint16), 2);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::sint32), 4);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::uint8), 1);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::uint16), 2);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::uint32), 4);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::float8), 1);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::float16), 2);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::float32), 4);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::cfloat8), 1);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::cfloat16), 2);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::cfloat32), 4);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::boolean), 1);
#if SCRUTINY_SUPPORT_64BITS
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::sint64), 8);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::uint64), 8);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::float64), 8);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::cfloat64), 8);
#endif
    }
    else if (CHAR_BIT == 2)
    {
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::sint8), 1);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::sint16), 1);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::sint32), 2);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::uint8), 1);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::uint16), 1);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::uint32), 2);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::float8), 1);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::float16), 1);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::float32), 2);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::cfloat8), 1);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::cfloat16), 1);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::cfloat32), 2);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::boolean), 1);
#if SCRUTINY_SUPPORT_64BITS
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::sint64), 4);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::uint64), 4);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::float64), 4);
        ASSERT_EQ(scrutiny::tools::get_type_size_char(scrutiny::VariableType::cfloat64), 4);
#endif
    }
}

TEST(TestTypes, TestTypeType)
{
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::sint8), scrutiny::VariableTypeType::_sint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::sint16), scrutiny::VariableTypeType::_sint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::sint32), scrutiny::VariableTypeType::_sint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::uint8), scrutiny::VariableTypeType::_uint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::uint16), scrutiny::VariableTypeType::_uint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::uint32), scrutiny::VariableTypeType::_uint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::float8), scrutiny::VariableTypeType::_float);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::float16), scrutiny::VariableTypeType::_float);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::float32), scrutiny::VariableTypeType::_float);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::cfloat8), scrutiny::VariableTypeType::_cfloat);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::cfloat16), scrutiny::VariableTypeType::_cfloat);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::cfloat32), scrutiny::VariableTypeType::_cfloat);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::boolean), scrutiny::VariableTypeType::_boolean);
#if SCRUTINY_SUPPORT_64BITS
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::uint64), scrutiny::VariableTypeType::_uint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::sint64), scrutiny::VariableTypeType::_sint);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::float64), scrutiny::VariableTypeType::_float);
    ASSERT_EQ(scrutiny::tools::get_var_type_type(scrutiny::VariableType::cfloat64), scrutiny::VariableTypeType::_cfloat);
#endif
}

TEST(TestTypes, TestMakeType)
{
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_sint, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::sint8);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_sint, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::sint16);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_sint, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::sint32);
#if SCRUTINY_SUPPORT_64BITS
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_sint, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::sint64);
#endif

    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_uint, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::uint8);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_uint, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::uint16);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_uint, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::uint32);
#if SCRUTINY_SUPPORT_64BITS
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_uint, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::uint64);
#endif

    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_float, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::float8);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_float, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::float16);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_float, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::float32);
#if SCRUTINY_SUPPORT_64BITS
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_float, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::float64);
#endif

    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_cfloat, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::cfloat8);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_cfloat, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::cfloat16);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_cfloat, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::cfloat32);
#if SCRUTINY_SUPPORT_64BITS
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_cfloat, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::cfloat64);
#endif

    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_boolean, scrutiny::VariableTypeSize::_8), scrutiny::VariableType::boolean);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_boolean, scrutiny::VariableTypeSize::_16), scrutiny::VariableType::unknown);
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_boolean, scrutiny::VariableTypeSize::_32), scrutiny::VariableType::unknown);
#if SCRUTINY_SUPPORT_64BITS
    ASSERT_EQ(scrutiny::tools::make_type(scrutiny::VariableTypeType::_boolean, scrutiny::VariableTypeSize::_64), scrutiny::VariableType::unknown);
#endif
}
