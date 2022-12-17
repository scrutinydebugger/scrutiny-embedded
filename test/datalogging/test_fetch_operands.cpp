//    test_fetch_operands.cpp
//        Test the capacity to decode an operand for log trigger
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include <gtest/gtest.h>
#include <cstring>

#include "scrutiny_test.hpp"
#include "scrutiny.hpp"

static bool rpv_read_callback(scrutiny::RuntimePublishedValue rpv, scrutiny::AnyType *outval)
{
    if (rpv.id == 0x1234 && rpv.type == scrutiny::VariableType::uint32)
    {
        outval->uint32 = 0xaabbccdd;
    }
    else if (rpv.id == 0x5678 && rpv.type == scrutiny::VariableType::float32)
    {
        outval->float32 = 3.1415926f;
    }
    else
    {
        return false;
    }

    return true;
}

class TestFetchOperands : public ScrutinyTest
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

    scrutiny::RuntimePublishedValue rpvs[2] = {
        {0x1234, scrutiny::VariableType::uint32},
        {0x5678, scrutiny::VariableType::float32}};

    virtual void SetUp()
    {
        config.set_buffers(_rx_buffer, sizeof(_rx_buffer), _tx_buffer, sizeof(_tx_buffer));

        config.set_readonly_address_range(readonly_ranges, sizeof(readonly_ranges) / sizeof(readonly_ranges[0]));
        config.set_forbidden_address_range(forbidden_ranges, sizeof(forbidden_ranges) / sizeof(forbidden_ranges[0]));

        config.set_published_values(rpvs, sizeof(rpvs) / sizeof(rpvs[0]), rpv_read_callback);

        scrutiny_handler.init(&config);
    }
};

using namespace scrutiny::datalogging;

TEST_F(TestFetchOperands, TestFetchLiteral)
{
    scrutiny::AnyType val;
    scrutiny::VariableType vartype;

    Operand operand;
    operand.type = OperandType::LITERAL;
    operand.data.literal.val = 0.1234f;
    bool success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype);
    EXPECT_TRUE(success);
    EXPECT_EQ(vartype, scrutiny::VariableType::float32);
    EXPECT_EQ(val.float32, 0.1234f);
}

TEST_F(TestFetchOperands, TestFetchVar)
{
    double my_var = 3.1415926;

    scrutiny::AnyType val;
    scrutiny::VariableType vartype;

    Operand operand;
    operand.type = OperandType::VAR;
    operand.data.var.addr = &my_var;
    operand.data.var.datatype = scrutiny::VariableType::float64;

    bool success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype);
    EXPECT_TRUE(success);

    EXPECT_EQ(vartype, scrutiny::VariableType::float64);
    EXPECT_EQ(val.float64, my_var);
}

TEST_F(TestFetchOperands, TestFetchRPV)
{
    scrutiny::AnyType val;
    scrutiny::VariableType vartype;

    Operand operand;
    operand.type = OperandType::RPV;

    operand.data.rpv.id = 0x1234;
    bool success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype);
    EXPECT_TRUE(success);
    EXPECT_EQ(vartype, scrutiny::VariableType::uint32);
    EXPECT_EQ(val.uint32, 0xaabbccdd);

    operand.data.rpv.id = 0x5678;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype);
    EXPECT_TRUE(success);
    EXPECT_EQ(vartype, scrutiny::VariableType::float32);
    EXPECT_EQ(val.float32, 3.1415926f);
}

TEST_F(TestFetchOperands, TestFetchVarBit)
{
#pragma pack(push, 1)
    struct
    {
        int16_t : 3;
        int16_t val : 11;
        int16_t : 2;
    } my_struct;
#pragma pack(pop)

    my_struct.val = -1000;

    scrutiny::AnyType val;
    scrutiny::VariableType vartype;

    Operand operand;
    operand.type = OperandType::VARBIT;

    operand.data.varbit.addr = &my_struct;
    operand.data.varbit.datatype = scrutiny::VariableType::sint16;
    operand.data.varbit.bitoffset = 3;
    operand.data.varbit.bitsize = 11;

    bool success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype);
    EXPECT_TRUE(success);
    EXPECT_EQ(vartype, scrutiny::VariableType::sint16);
    EXPECT_EQ(val.sint16, my_struct.val);
}

TEST_F(TestFetchOperands, TestBadOperandType)
{
    scrutiny::AnyType val;
    scrutiny::VariableType vartype;

    Operand operand;
    operand.type = static_cast<OperandType>(0xff);

    bool success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype);
    EXPECT_FALSE(success);
}

TEST_F(TestFetchOperands, TestFetchForbiddenMemory)
{
    scrutiny::AnyType val;
    scrutiny::VariableType vartype;

    Operand operand;
    operand.type = OperandType::VAR;
    operand.data.var.addr = forbidden_buffer;
    operand.data.var.datatype = scrutiny::VariableType::uint8;

    bool success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype);
    EXPECT_FALSE(success);
}

TEST_F(TestFetchOperands, TestFetchInexistandRPV)
{
    scrutiny::AnyType val;
    scrutiny::VariableType vartype;

    Operand operand;
    operand.type = OperandType::RPV;
    operand.data.rpv.id = 0xAAAA;

    bool success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype);
    EXPECT_FALSE(success);
}

TEST_F(TestFetchOperands, TestFetchBitfieldsLimits)
{
#pragma pack(push, 1)
    struct
    {
        int32_t a;
        int32_t b;
        int32_t c;
        int32_t d;
    } my_struct;
#pragma pack(pop)

    scrutiny::AnyType val;
    scrutiny::VariableType vartype;
    bool success;

    Operand operand;
    operand.type = OperandType::VARBIT;
    operand.data.varbit.addr = &my_struct;
    operand.data.varbit.datatype = scrutiny::VariableType::sint8;

    operand.data.varbit.bitoffset = 63;
    operand.data.varbit.bitsize = 1;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype);
    EXPECT_TRUE(success);

    operand.data.varbit.bitoffset = 64;
    operand.data.varbit.bitsize = 1;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype);
    EXPECT_FALSE(success);

    operand.data.varbit.bitoffset = 0;
    operand.data.varbit.bitsize = 64;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype);
    EXPECT_TRUE(success);

    operand.data.varbit.bitoffset = 0;
    operand.data.varbit.bitsize = 65;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype);
    EXPECT_FALSE(success);

    operand.data.varbit.bitoffset = 0;
    operand.data.varbit.bitsize = 0;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype);
    EXPECT_FALSE(success);
}