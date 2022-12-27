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

TEST_F(TestTriggerConditions, OperatorEQ)
{
    // scrutiny::datalogging::trigger::EqualCondition cond;
    scrutiny::datalogging::trigger::ConditionSet cond;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare valtypes[2];

    vals[0]._float = 0.4f;
    vals[1]._float = 0.5f;
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;
    EXPECT_FALSE(cond.eq.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 0.5f;
    EXPECT_TRUE(cond.eq.evaluate(cond.data(), valtypes, vals));
}

TEST_F(TestTriggerConditions, OperatorNEQ)
{
    scrutiny::datalogging::trigger::ConditionSet cond;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare valtypes[2];

    vals[0]._float = 0.5f;
    vals[1]._float = 0.5f;
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;
    EXPECT_FALSE(cond.neq.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 0.4f;
    EXPECT_TRUE(cond.neq.evaluate(cond.data(), valtypes, vals));
}

TEST_F(TestTriggerConditions, OperatorGT)
{
    scrutiny::datalogging::trigger::ConditionSet cond;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare valtypes[2];

    vals[0]._float = 0.4f;
    vals[1]._float = 0.5f;
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;
    EXPECT_FALSE(cond.gt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 0.5f;
    EXPECT_FALSE(cond.gt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 0.6f;
    EXPECT_TRUE(cond.gt.evaluate(cond.data(), valtypes, vals));
}

TEST_F(TestTriggerConditions, OperatorGET)
{
    scrutiny::datalogging::trigger::ConditionSet cond;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare valtypes[2];

    vals[0]._float = 0.4f;
    vals[1]._float = 0.5f;
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;
    EXPECT_FALSE(cond.get.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 0.5f;
    EXPECT_TRUE(cond.get.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 0.6f;
    EXPECT_TRUE(cond.get.evaluate(cond.data(), valtypes, vals));
}

TEST_F(TestTriggerConditions, OperatorLT)
{
    scrutiny::datalogging::trigger::ConditionSet cond;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare valtypes[2];

    vals[0]._float = 0.6f;
    vals[1]._float = 0.5f;
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;
    EXPECT_FALSE(cond.lt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 0.5f;
    EXPECT_FALSE(cond.lt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 0.4f;
    EXPECT_TRUE(cond.lt.evaluate(cond.data(), valtypes, vals));
}

TEST_F(TestTriggerConditions, OperatorLET)
{
    scrutiny::datalogging::trigger::ConditionSet cond;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare valtypes[2];

    vals[0]._float = 0.6f;
    vals[1]._float = 0.5f;
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;
    EXPECT_FALSE(cond.let.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 0.5f;
    EXPECT_TRUE(cond.let.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 0.4f;
    EXPECT_TRUE(cond.let.evaluate(cond.data(), valtypes, vals));
}

TEST_F(TestTriggerConditions, Equality_AllTypes)
{
    scrutiny::datalogging::trigger::ConditionSet cond;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare valtypes[2];

    for (int i = 0; i < 2; i++)
    {
        const int op0 = (i == 0) ? 0 : 1;
        const int op1 = (i == 0) ? 1 : 0;

        vals[op0]._float = 0.5f;
        vals[op1]._float = 0.6f;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_float;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_float;
        EXPECT_FALSE(cond.eq.evaluate(cond.data(), valtypes, vals));
        vals[op1]._float = 0.5f;
        EXPECT_TRUE(cond.eq.evaluate(cond.data(), valtypes, vals));

        vals[op0]._uint = 2;
        vals[op1]._float = 2.1f;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_uint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_float;
        EXPECT_FALSE(cond.eq.evaluate(cond.data(), valtypes, vals));
        vals[op1]._float = 2.0f;
        EXPECT_TRUE(cond.eq.evaluate(cond.data(), valtypes, vals));

        vals[op0]._sint = 2;
        vals[op1]._float = 2.1f;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_sint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_float;
        EXPECT_FALSE(cond.eq.evaluate(cond.data(), valtypes, vals));
        vals[op1]._float = 2.0f;
        EXPECT_TRUE(cond.eq.evaluate(cond.data(), valtypes, vals));

        vals[op0]._sint = 2;
        vals[op1]._uint = 3;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_sint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_uint;
        EXPECT_FALSE(cond.eq.evaluate(cond.data(), valtypes, vals));
        vals[op1]._uint = 2;
        EXPECT_TRUE(cond.eq.evaluate(cond.data(), valtypes, vals));

        vals[op0]._uint = 2;
        vals[op1]._uint = 3;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_uint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_uint;
        EXPECT_FALSE(cond.eq.evaluate(cond.data(), valtypes, vals));
        vals[op1]._uint = 2;
        EXPECT_TRUE(cond.eq.evaluate(cond.data(), valtypes, vals));

        vals[op0]._sint = 2;
        vals[op1]._sint = 3;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_sint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_sint;
        EXPECT_FALSE(cond.eq.evaluate(cond.data(), valtypes, vals));
        vals[op1]._sint = 2;
        EXPECT_TRUE(cond.eq.evaluate(cond.data(), valtypes, vals));

        vals[op0]._sint = -1;
        vals[op1]._uint = static_cast<scrutiny::uint_biggest_t>(-1);
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_sint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_uint;
        EXPECT_FALSE(cond.eq.evaluate(cond.data(), valtypes, vals));
    }
}

TEST_F(TestTriggerConditions, LT_MixedSignAndTypes)
{
    scrutiny::datalogging::trigger::ConditionSet cond;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare valtypes[2];
    for (int i = 0; i < 2; i++)
    {
        const int op0 = (i == 0) ? 0 : 1;
        const int op1 = (i == 0) ? 1 : 0;

        bool truefalse = (i == 0) ? false : true;

        vals[op0]._float = 1.0f;
        vals[op1]._sint = -5;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_float;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_sint;
        EXPECT_EQ(cond.lt.evaluate(cond.data(), valtypes, vals), truefalse);
        vals[op0]._float = -6;
        EXPECT_EQ(cond.lt.evaluate(cond.data(), valtypes, vals), !truefalse);

        vals[op0]._float = -1.0f;
        vals[op1]._uint = 0;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_float;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_uint;
        EXPECT_EQ(cond.lt.evaluate(cond.data(), valtypes, vals), !truefalse);

        vals[op0]._sint = -6;
        vals[op1]._sint = -5;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_sint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_sint;
        EXPECT_EQ(cond.lt.evaluate(cond.data(), valtypes, vals), !truefalse);

        vals[op0]._uint = static_cast<scrutiny::uint_biggest_t>(-2);
        vals[op1]._sint = -1;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_uint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_sint;
        EXPECT_EQ(cond.lt.evaluate(cond.data(), valtypes, vals), truefalse);
    }
}

TEST_F(TestTriggerConditions, ChangeMoreThan_Basic)
{
    scrutiny::datalogging::trigger::ConditionSet cond;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare valtypes[2];
    cond.cmt.reset(cond.data());

    // Test positive change
    vals[0]._float = 10.0f;
    vals[1]._float = 5.0f; // Positive change of 5.0
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;

    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._float = 11.0f;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._float = 5.0f; // Negative change has no effect
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._float = 10.1f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 11.0f;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 20.0f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 30.0f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    // Test negative change
    cond.cmt.reset(cond.data());
    vals[0]._float = 10.0f;
    vals[1]._float = -5.0f; // NEgative change of 5.0
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._float = 20.0f; // Positive change has no effect
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 14.9f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 14.0f;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 5.0f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = -5.0f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
}

TEST_F(TestTriggerConditions, ChangeMoreThan_AllTypes)
{
    scrutiny::datalogging::trigger::ConditionSet cond;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare valtypes[2];

    // ===== Float - Sint
    cond.cmt.reset(cond.data());
    // Test positive change
    vals[0]._float = 10.0f;
    vals[1]._sint = 5; // Positive change of 5.0
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_sint;

    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._float = 11.0f;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._float = 5.0f; // Negative change has no effect
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._float = 10.1f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 11.0f;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 20.0f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 30.0f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    // Test negative change
    cond.cmt.reset(cond.data());
    vals[0]._float = 10.0f;
    vals[1]._sint = -5; // NEgative change of 5.0
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._float = 20.0f; // Positive change has no effect
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 14.9f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 14.0f;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 5.0f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = -5.0f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    // ===== Float - Uint
    cond.cmt.reset(cond.data());
    // Test positive change
    vals[0]._float = 10.0f;
    vals[1]._uint = 5; // Positive change of 5.0
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_uint;

    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._float = 11.0f;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._float = 5.0f; // Negative change has no effect
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._float = 10.1f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 11.0f;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 20.0f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._float = 30.0f;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    // ===== Uint - Uint
    cond.cmt.reset(cond.data());
    // Test positive change
    vals[0]._uint = static_cast<scrutiny::uint_biggest_t>(-1000);
    vals[1]._uint = 5; // Positive change of 5.0
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_uint;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_uint;

    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._uint += 1;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._uint -= 10; // Negative change has no effect
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._uint += 6;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._uint += 2;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._uint += 6;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._uint += 10;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    // ===== Uint - Sint
    cond.cmt.reset(cond.data());
    // Test positive change
    vals[0]._uint = static_cast<scrutiny::uint_biggest_t>(-1000);
    ;
    vals[1]._sint = 5; // Positive change of 5.0
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_uint;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_sint;

    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._uint += 1;
    ;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._uint -= 10; // Negative change has no effect
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._uint += 6;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._uint += 4;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._uint += 10;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._uint += 6;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    // Test negative change
    cond.cmt.reset(cond.data());
    vals[0]._uint = static_cast<scrutiny::uint_biggest_t>(-1000);
    vals[1]._sint = -5; // NEgative change of 5.0
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._uint += 20; // Positive change has no effect
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._uint -= 6;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._uint -= 4;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._uint -= 100;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._uint -= 10;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    // ===== Sint - Sint
    cond.cmt.reset(cond.data());
    // Test positive change
    vals[0]._sint = -100;
    ;
    vals[1]._sint = 5; // Positive change of 5.0
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_sint;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_sint;

    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._sint += 1;
    ;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._sint -= 10; // Negative change has no effect
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._sint += 6;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._sint += 4;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._sint += 10;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._sint += 6;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    // Test negative change
    cond.cmt.reset(cond.data());
    vals[0]._sint = -100;
    vals[1]._sint = -5; // NEgative change of 5.0
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._sint += 20; // Positive change has no effect
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._sint += -6;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._sint += -4;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._sint += -100;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._sint += -10;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    // ===== Sint - Uint
    cond.cmt.reset(cond.data());
    // Test positive change
    vals[0]._sint = -100;
    ;
    vals[1]._uint = 5; // Positive change of 5.0
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_sint;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_sint;

    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._uint += 1;
    ;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._uint -= 10; // Negative change has no effect
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));

    vals[0]._uint += 6;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._uint += 4;
    EXPECT_FALSE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._uint += 10;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
    vals[0]._uint += 6;
    EXPECT_TRUE(cond.cmt.evaluate(cond.data(), valtypes, vals));
}
