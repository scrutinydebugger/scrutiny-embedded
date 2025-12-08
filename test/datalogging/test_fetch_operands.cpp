//    test_fetch_operands.cpp
//        Test the capacity to decode an operand for log trigger
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "scrutinytest/scrutinytest.hpp"
#include <cstring>

#include "scrutiny.hpp"
#include "scrutiny_test.hpp"

static bool rpv_read_callback(scrutiny::RuntimePublishedValue rpv, scrutiny::AnyType *outval, scrutiny::LoopHandler *const caller)
{
    static_cast<void>(caller);
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

    scrutiny::AddressRange readonly_ranges[2];
    scrutiny::AddressRange forbidden_ranges[2];
    scrutiny::RuntimePublishedValue rpvs[2];

    TestFetchOperands() :
        ScrutinyTest(),
        tb(),
        scrutiny_handler(),
        config(),
        _rx_buffer(),
        _tx_buffer(),
        forbidden_buffer(),
        forbidden_buffer2(),
        readonly_buffer(),
        readonly_buffer2()
    {
        readonly_ranges[0] = scrutiny::tools::make_address_range(readonly_buffer, sizeof(readonly_buffer)),
        readonly_ranges[1] = scrutiny::tools::make_address_range(readonly_buffer2, sizeof(readonly_buffer2));

        forbidden_ranges[0] = scrutiny::tools::make_address_range(forbidden_buffer, sizeof(forbidden_buffer));
        forbidden_ranges[1] = scrutiny::tools::make_address_range(forbidden_buffer, sizeof(forbidden_buffer2));

        rpvs[0].id = 0x1234;
        rpvs[0].type = scrutiny::VariableType::uint32;
        rpvs[1].id = 0x5678;
        rpvs[1].type = scrutiny::VariableType::float32;
    }

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
    scrutiny::VariableType::eVariableType vartype = scrutiny::VariableType::unknown;

    Operand operand;
    operand.type = OperandType::LITERAL;
    operand.data.literal.val = 0.1234f;
    bool success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
    EXPECT_TRUE(success);
    EXPECT_EQ(vartype, scrutiny::VariableType::float32);
    EXPECT_EQ(val.float32, 0.1234f);
}

TEST_F(TestFetchOperands, TestFetchVar)
{
    float my_var = 3.1415926f;

    scrutiny::AnyType val;
    scrutiny::VariableType::eVariableType vartype = scrutiny::VariableType::unknown;

    Operand operand;
    operand.type = OperandType::VAR;
    operand.data.var.addr = &my_var;
    operand.data.var.datatype = scrutiny::VariableType::float32;

    bool success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
    EXPECT_TRUE(success);

    EXPECT_EQ(vartype, scrutiny::VariableType::float32);
    EXPECT_EQ(val.float32, my_var);
}

TEST_F(TestFetchOperands, TestFetchRPV)
{
    scrutiny::AnyType val;
    scrutiny::VariableType::eVariableType vartype = scrutiny::VariableType::unknown;

    Operand operand;
    operand.type = OperandType::RPV;

    operand.data.rpv.id = 0x1234;
    bool success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
    EXPECT_TRUE(success);
    EXPECT_EQ(vartype, scrutiny::VariableType::uint32);
    EXPECT_EQ(val.uint32, 0xaabbccdd);

    operand.data.rpv.id = 0x5678;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
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
    scrutiny::VariableType::eVariableType vartype = scrutiny::VariableType::unknown;

    Operand operand;
    operand.type = OperandType::VARBIT;

    operand.data.varbit.addr = &my_struct;
    operand.data.varbit.datatype = scrutiny::VariableType::sint16;
    operand.data.varbit.bitoffset = 3;
    operand.data.varbit.bitsize = 11;

    bool success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
    EXPECT_TRUE(success);
    EXPECT_EQ(vartype, scrutiny::VariableType::sint16);
    EXPECT_EQ(val.sint16, my_struct.val);
}

TEST_F(TestFetchOperands, TestBadOperandType)
{
    scrutiny::AnyType val;
    scrutiny::VariableType::eVariableType vartype = scrutiny::VariableType::unknown;

    Operand operand;
    operand.type = static_cast<OperandType::eOperandType>(0xff);

    bool success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
    EXPECT_FALSE(success);
}

TEST_F(TestFetchOperands, TestFetchForbiddenMemory)
{
    scrutiny::AnyType val;
    scrutiny::VariableType::eVariableType vartype = scrutiny::VariableType::unknown;

    Operand operand;
    operand.type = OperandType::VAR;
    operand.data.var.addr = forbidden_buffer;
    operand.data.var.datatype = scrutiny::VariableType::uint8;

    bool success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
    EXPECT_FALSE(success);
}

TEST_F(TestFetchOperands, TestFetchInexistandRPV)
{
    scrutiny::AnyType val;
    scrutiny::VariableType::eVariableType vartype = scrutiny::VariableType::unknown;

    Operand operand;
    operand.type = OperandType::RPV;
    operand.data.rpv.id = 0xAAAA;

    bool success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
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

    my_struct.a = 0;
    my_struct.b = 0;
    my_struct.c = 0;
    my_struct.d = 0;

    scrutiny::AnyType val;
    scrutiny::VariableType::eVariableType vartype = scrutiny::VariableType::unknown;
    bool success = false;

    Operand operand;
    operand.type = OperandType::VARBIT;
    operand.data.varbit.addr = &my_struct;
    operand.data.varbit.datatype = scrutiny::VariableType::sint8;

    operand.data.varbit.bitoffset = 31;
    operand.data.varbit.bitsize = 1;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
    EXPECT_TRUE(success);

    operand.data.varbit.bitoffset = 32;
    operand.data.varbit.bitsize = 1;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);

#if SCRUTINY_SUPPORT_64BITS
    EXPECT_TRUE(success);
#else
    EXPECT_FALSE(success);
#endif

    operand.data.varbit.bitoffset = 0;
    operand.data.varbit.bitsize = 32;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
    EXPECT_TRUE(success);

    operand.data.varbit.bitoffset = 0;
    operand.data.varbit.bitsize = 33;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
#if SCRUTINY_SUPPORT_64BITS
    EXPECT_TRUE(success);
#else
    EXPECT_FALSE(success);
#endif

    operand.data.varbit.bitoffset = 63;
    operand.data.varbit.bitsize = 1;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
#if SCRUTINY_SUPPORT_64BITS
    EXPECT_TRUE(success);
#else
    EXPECT_FALSE(success);
#endif

    operand.data.varbit.bitoffset = 64;
    operand.data.varbit.bitsize = 1;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
    EXPECT_FALSE(success);

    operand.data.varbit.bitoffset = 0;
    operand.data.varbit.bitsize = 64;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
#if SCRUTINY_SUPPORT_64BITS
    EXPECT_TRUE(success);
#else
    EXPECT_FALSE(success);
#endif

    operand.data.varbit.bitoffset = 0;
    operand.data.varbit.bitsize = 65;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
    EXPECT_FALSE(success);

    operand.data.varbit.bitoffset = 0;
    operand.data.varbit.bitsize = 0;
    success = fetch_operand(&scrutiny_handler, &operand, &val, &vartype, SCRUTINY_NULL);
    EXPECT_FALSE(success);
}
