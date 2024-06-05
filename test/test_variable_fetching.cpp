//    test_variable_fetching.cpp
//        Test the capacity to read a variable from the memory given the debugging info
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

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

    TestVariableFetching() : ScrutinyTest(),
                             tb{},
                             scrutiny_handler{},
                             config{},
                             _rx_buffer{},
                             _tx_buffer{},
                             forbidden_buffer{},
                             forbidden_buffer2{},
                             readonly_buffer{},
                             readonly_buffer2{}
    {
    }

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

    float f32 = 1.2345f;
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
        uint8_t pad : 2;
        uint8_t val : 3;
    } data_u8;

    struct
    {
        int8_t pad : 2;
        int8_t val : 5;
    } data_s8;

    struct
    {
        uint16_t pad : 4;
        uint16_t val : 10;
    } data_u16;

    struct
    {
        int16_t pad : 4;
        int16_t val : 10;
    } data_s16;

    struct
    {
        uint32_t pad : 7;
        uint32_t val : 18;
    } data_u32;

    struct
    {
        int32_t pad : 7;
        int32_t val : 18;
    } data_s32;

    struct
    {
        uint8_t pad : 2;
        bool b1 : 1;
        uint8_t pad2 : 2;
        bool b2 : 1;
    } data_bool;

#if SCRUTINY_SUPPORT_64BITS
    struct
    {
        uint64_t pad : 7;
        uint64_t val : 37;
    } data_u64;

    struct
    {
        int64_t pad : 7;
        int64_t val : 37;
    } data_s64;
#endif

#pragma pack(pop)

    data_u8.val = 6;
    data_s8.val = -15;
    data_u16.val = 300;
    data_s16.val = -400;
    data_u32.val = 100000;
    data_s32.val = -100000;
#if SCRUTINY_SUPPORT_64BITS
    data_u64.val = 0x123456789;
    data_s64.val = -0x123456789;
#endif
    data_bool.b1 = true;
    data_bool.b2 = static_cast<bool>(123);

    memcpy(&some_buffer[0], &data_u8, sizeof(data_u8));
    memcpy(&some_buffer[1], &data_s8, sizeof(data_s8));
    memcpy(&some_buffer[2], &data_u16, sizeof(data_u16));
    memcpy(&some_buffer[4], &data_s16, sizeof(data_s16));
    memcpy(&some_buffer[6], &data_u32, sizeof(data_u32));
    memcpy(&some_buffer[10], &data_s32, sizeof(data_s32));
    memcpy(&some_buffer[14], &data_bool, sizeof(data_bool));

#if SCRUTINY_SUPPORT_64BITS
    memcpy(&some_buffer[15], &data_u64, sizeof(data_u64));
    memcpy(&some_buffer[23], &data_s64, sizeof(data_s64));
#endif

    scrutiny::AnyType outval;
    scrutiny::VariableType outtype;
    bool success;
    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[0], scrutiny::VariableTypeType::_uint, 2, 3, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.uint8, data_u8.val);
    EXPECT_EQ(outtype, scrutiny::VariableType::uint8);

    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[1], scrutiny::VariableTypeType::_sint, 2, 5, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.sint8, data_s8.val);
    EXPECT_EQ(outtype, scrutiny::VariableType::sint8);

    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[2], scrutiny::VariableTypeType::_uint, 4, 10, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.uint16, data_u16.val);
    EXPECT_EQ(outtype, scrutiny::VariableType::uint16);

    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[4], scrutiny::VariableTypeType::_sint, 4, 10, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.sint16, data_s16.val);
    EXPECT_EQ(outtype, scrutiny::VariableType::sint16);

    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[6], scrutiny::VariableTypeType::_uint, 7, 18, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.uint32, data_u32.val);
    EXPECT_EQ(outtype, scrutiny::VariableType::uint32);

    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[10], scrutiny::VariableTypeType::_sint, 7, 18, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.sint32, data_s32.val);
    EXPECT_EQ(outtype, scrutiny::VariableType::sint32);

    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[14], scrutiny::VariableTypeType::_boolean, 2, 1, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.boolean, data_bool.b1);
    EXPECT_EQ(outtype, scrutiny::VariableType::boolean);

    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[14], scrutiny::VariableTypeType::_boolean, 2, 1, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.boolean, data_bool.b2);
    EXPECT_EQ(outtype, scrutiny::VariableType::boolean);

#if SCRUTINY_SUPPORT_64BITS
    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[15], scrutiny::VariableTypeType::_uint, 7, 37, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.uint64, data_u64.val);
    EXPECT_EQ(outtype, scrutiny::VariableType::uint64);

    success = scrutiny_handler.fetch_variable_bitfield(&some_buffer[23], scrutiny::VariableTypeType::_sint, 7, 37, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.sint64, data_s64.val);
    EXPECT_EQ(outtype, scrutiny::VariableType::sint64);
#endif
}
