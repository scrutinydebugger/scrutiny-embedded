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

class TestTriggerConditions : public ScrutinyTest
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

TEST_F(TestTriggerConditions, Equality)
{
    scrutiny::AnyType val;
    scrutiny::VariableType vartype;

    float thevar = 0;

    Operand op1;
    op1.type = OperandType::LITERAL;
    op1.data.literal.val = 10;

    Operand op2;
    op1.type = OperandType::VAR;
    op1.data.var.addr = &thevar;
    op1.data.var.datatype = scrutiny::VariableType::float32;

    conditions::EqualCondition cond;
    EXPECT_FALSE(cond.evaluate(op1, op2));
    thevar = 10;
    EXPECT_TRUE(cond.evaluate(op1, op2));
}
