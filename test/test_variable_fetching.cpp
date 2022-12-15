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

    success = scrutiny_handler.fetch_variable(&some_buffer[4], scrutiny::VariableType::float64, &outval);
    EXPECT_EQ(outval.float64, f64);
    EXPECT_TRUE(success);

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
        uint16_t b : 10;
        int8_t c : 5;
    } data;
#pragma pack(pop)

    data.a = 6;
    data.b = 300;
    data.c = -15;

    memcpy(some_buffer, &data, sizeof(data));

    scrutiny::AnyType outval;
    scrutiny::VariableType outtype;
    bool success;
    success = scrutiny_handler.fetch_variable_bitfield(some_buffer, scrutiny::VariableTypeType::_uint, 0, 3, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.uint8, data.a);
    EXPECT_EQ(outtype, scrutiny::VariableType::uint8);

    success = scrutiny_handler.fetch_variable_bitfield(some_buffer, scrutiny::VariableTypeType::_uint, 3, 10, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.uint16, data.b);
    EXPECT_EQ(outtype, scrutiny::VariableType::uint16);

    success = scrutiny_handler.fetch_variable_bitfield(some_buffer, scrutiny::VariableTypeType::_sint, 13, 5, &outval, &outtype);
    EXPECT_TRUE(success);
    EXPECT_EQ(outval.sint8, data.c);
    EXPECT_EQ(outtype, scrutiny::VariableType::sint8);
}
