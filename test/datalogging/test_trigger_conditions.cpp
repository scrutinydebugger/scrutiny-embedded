//    test_trigger_conditions.cpp
//        Test datalogging trigger conditions
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

TEST_F(TestTriggerConditions, Equality_AllTypes)
{
    scrutiny::datalogging::trigger::EqualCondition cond;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::VariableType valtypes[2];

    for (int i = 0; i < 2; i++)
    {
        const int op0 = (i == 0) ? 0 : 1;
        const int op1 = (i == 0) ? 1 : 0;

        vals[op0]._float = 0.5f;
        vals[op1]._float = 0.6f;
        valtypes[op0] = scrutiny::VariableType::float32;
        valtypes[op1] = scrutiny::VariableType::float32;
        EXPECT_FALSE(cond.evaluate(valtypes, vals));
        vals[op1]._float = 0.5f;
        EXPECT_TRUE(cond.evaluate(valtypes, vals));

        vals[op0]._uint = 2;
        vals[op1]._float = 2.1f;
        valtypes[op0] = scrutiny::BiggestUint;
        valtypes[op1] = scrutiny::VariableType::float32;
        EXPECT_FALSE(cond.evaluate(valtypes, vals));
        vals[op1]._float = 2.0f;
        EXPECT_TRUE(cond.evaluate(valtypes, vals));

        vals[op0]._sint = 2;
        vals[op1]._float = 2.1f;
        valtypes[op0] = scrutiny::BiggestSint;
        valtypes[op1] = scrutiny::VariableType::float32;
        EXPECT_FALSE(cond.evaluate(valtypes, vals));
        vals[op1]._float = 2.0f;
        EXPECT_TRUE(cond.evaluate(valtypes, vals));

        vals[op0]._sint = 2;
        vals[op1]._uint = 3;
        valtypes[op0] = scrutiny::BiggestSint;
        valtypes[op1] = scrutiny::BiggestUint;
        EXPECT_FALSE(cond.evaluate(valtypes, vals));
        vals[op1]._uint = 2;
        EXPECT_TRUE(cond.evaluate(valtypes, vals));

        vals[op0]._uint = 2;
        vals[op1]._uint = 3;
        valtypes[op0] = scrutiny::BiggestUint;
        valtypes[op1] = scrutiny::BiggestUint;
        EXPECT_FALSE(cond.evaluate(valtypes, vals));
        vals[op1]._uint = 2;
        EXPECT_TRUE(cond.evaluate(valtypes, vals));

        vals[op0]._sint = 2;
        vals[op1]._sint = 3;
        valtypes[op0] = scrutiny::BiggestSint;
        valtypes[op1] = scrutiny::BiggestSint;
        EXPECT_FALSE(cond.evaluate(valtypes, vals));
        vals[op1]._sint = 2;
        EXPECT_TRUE(cond.evaluate(valtypes, vals));
    }
}
