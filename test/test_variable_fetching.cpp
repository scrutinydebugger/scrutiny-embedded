//    test_variable_fetching.cpp
//        Test the capacity to read a variable from the memory given the debugging info
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include <cstddef>
#include <gtest/gtest.h>
#include "scrutiny_test.hpp"
#include "scrutiny.hpp"

class TestVariableFetching : public ScrutinyTest
{
protected:
    scrutiny::Timebase tb;
    scrutiny::MainHandler scrutiny_handler;
    scrutiny::Config config;

    uint8_t _rx_buffer[128];
    uint8_t _tx_buffer[128];

    uint8_t forbidden_buffer[128];
    uint8_t forbidden_buffer2[128];
    uint8_t readonly_buffer[128];
    uint8_t readonly_buffer2[128];

    scrutiny::AddressRange readonly_ranges[2] = {
        scrutiny::tools::make_address_range(readonly_buffer, sizeof(readonly_buffer)),
        scrutiny::tools::make_address_range(readonly_buffer2, sizeof(readonly_buffer2))};

    scrutiny::AddressRange forbidden_ranges[2] = {
        scrutiny::tools::make_address_range(forbidden_buffer, sizeof(forbidden_buffer)),
        scrutiny::tools::make_address_range(forbidden_buffer2, sizeof(forbidden_buffer2))};

    virtual void SetUp()
    {
        config.set_buffers(_rx_buffer, sizeof(_rx_buffer), _tx_buffer, sizeof(_tx_buffer));

        config.set_readonly_address_range(readonly_ranges, sizeof(readonly_ranges) / sizeof(readonly_ranges[0]));
        config.set_forbidden_address_range(forbidden_ranges, sizeof(forbidden_ranges) / sizeof(forbidden_ranges[0]));

        scrutiny_handler.init(&config);
    }
};

TEST_F(TestVariableFetching, RandomFetch)
{
    uint8_t some_buffer[32];

    float f32 = 1.2345;
    double f64 = 3.1415926;
    uint16_t u16 = 0x1234;
    bool b = true;

    memcpy(&some_buffer[0], &f32, sizeof(f32));
    memcpy(&some_buffer[4], &f64, sizeof(f64));
    memcpy(&some_buffer[4 + 8], &u16, sizeof(u16));
    memcpy(&some_buffer[4 + 8 + 2], &b, sizeof(b));

    scrutiny::AnyType outval;
    bool success;
    success = scrutiny_handler.fetch_variable(&some_buffer[0], scrutiny::VariableType::float32, &outval);
    EXPECT_EQ(outval.float32, f32);
    EXPECT_TRUE(success);

#if SCRUTINY_SUPPORT_64BITS
    success = scrutiny_handler.fetch_variable(&some_buffer[4], scrutiny::VariableType::float64, &outval);
    EXPECT_EQ(outval.float64, f64);
    EXPECT_TRUE(success);
#endif

    success = scrutiny_handler.fetch_variable(&some_buffer[4 + 8], scrutiny::VariableType::uint16, &outval);
    EXPECT_EQ(outval.uint16, u16);
    EXPECT_TRUE(success);

    success = scrutiny_handler.fetch_variable(&some_buffer[4 + 8 + 2], scrutiny::VariableType::boolean, &outval);
    EXPECT_EQ(outval.boolean, b);
    EXPECT_TRUE(success);
}

TEST_F(TestVariableFetching, NoAccess)
{
    scrutiny::AnyType outval;
    bool success;
    success = scrutiny_handler.fetch_variable(forbidden_buffer, scrutiny::VariableType::float32, &outval);
    EXPECT_FALSE(success);

    success = scrutiny_handler.fetch_variable(forbidden_buffer2, scrutiny::VariableType::float32, &outval);
    EXPECT_FALSE(success);

    success = scrutiny_handler.fetch_variable(readonly_buffer, scrutiny::VariableType::float32, &outval);
    EXPECT_TRUE(success);

    success = scrutiny_handler.fetch_variable(readonly_buffer2, scrutiny::VariableType::float32, &outval);
    EXPECT_TRUE(success);
}

TEST_F(TestVariableFetching, Bitfield)
{
    uint8_t some_buffer[32];
    memset(some_buffer, 0xFF, sizeof(some_buffer));

#pragma pack(push, 1)
    struct
    {
        uint8_t a : 3;
        int8_t b : 5;
        uint16_t c : 10;
        int16_t d : 10;
        uint32_t e : 17;
        int32_t f : 19;
        // break to avoid bitoffset >= 64
        uint8_t : 2;
        bool b1 : 1;
        bool b2 : 7;
#if SCRUTINY_SUPPORT_64BITS
        uint64_t : 54;
        // break to avoid bitoffset >= 64
        uint64_t : 12;
        uint64_t g : 35;
        uint64_t : 17;
        // break to avoid bitoffset >= 64
        uint64_t : 20;
        int64_t h : 35;
        uint64_t : 9;
#endif

    } data1;
#pragma pack(pop)

    data1.a = 6;
    data1.b = -15;
    data1.c = 300;
    data1.d = -400;
    data1.e = 100000;
    data1.f = -100000;
#if SCRUTINY_SUPPORT_64BITS
    data1.g = 0x123456789;
    data1.h = -0x123456789;
#endif
    data1.b1 = true;
    data1.b2 = static_cast<bool>(123);

    memcpy(some_buffer, &data1, sizeof(data1));

    scrutiny::AnyType outval;
    scrutiny::VariableType outtype;
    bool success;
    success = scrutiny_handler.fetch_variable_bitfield(some_buffer, scrutiny::VariableTypeType::_uint, 0, 3, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.uint8, data1.a);
    EXPECT_EQ(outtype, scrutiny::VariableType::uint8);

    success = scrutiny_handler.fetch_variable_bitfield(some_buffer, scrutiny::VariableTypeType::_sint, 3, 5, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.sint8, data1.b);
    EXPECT_EQ(outtype, scrutiny::VariableType::sint8);

    success = scrutiny_handler.fetch_variable_bitfield(some_buffer, scrutiny::VariableTypeType::_uint, 8, 10, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.uint16, data1.c);
    EXPECT_EQ(outtype, scrutiny::VariableType::uint16);

    success = scrutiny_handler.fetch_variable_bitfield(some_buffer, scrutiny::VariableTypeType::_sint, 18, 10, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.sint16, data1.d);
    EXPECT_EQ(outtype, scrutiny::VariableType::sint16);

    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[2], scrutiny::VariableTypeType::_uint, 28 - 16, 17, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.uint32, data1.e);
    EXPECT_EQ(outtype, scrutiny::VariableType::uint32);

    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[4], scrutiny::VariableTypeType::_sint, 45 - 32, 19, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.sint32, data1.f);
    EXPECT_EQ(outtype, scrutiny::VariableType::sint32);

#if SCRUTINY_SUPPORT_64BITS
    // Same tests as above, but read from a different starting point that a 64bit machine could do
    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer, scrutiny::VariableTypeType::_uint, 28, 17, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.uint32, data1.e);
    EXPECT_EQ(outtype, scrutiny::VariableType::uint32);

    success = scrutiny_handler.fetch_variable_bitfield(some_buffer, scrutiny::VariableTypeType::_sint, 45, 19, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.sint32, data1.f);
    EXPECT_EQ(outtype, scrutiny::VariableType::sint32);
#endif

    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[8], scrutiny::VariableTypeType::_boolean, 2, 1, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.boolean, data1.b1);
    EXPECT_EQ(outtype, scrutiny::VariableType::boolean);

    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[8], scrutiny::VariableTypeType::_boolean, 3, 7, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.boolean, data1.b2);
    EXPECT_EQ(outtype, scrutiny::VariableType::boolean);

#if SCRUTINY_SUPPORT_64BITS
    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[16], scrutiny::VariableTypeType::_uint, 12, 35, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.uint64, data1.g);
    EXPECT_EQ(outtype, scrutiny::VariableType::uint64);

    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[24], scrutiny::VariableTypeType::_sint, 20, 35, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.sint64, data1.h);
    EXPECT_EQ(outtype, scrutiny::VariableType::sint64);
#endif
}
