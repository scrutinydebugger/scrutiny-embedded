//    test_trigger_conditions.cpp
//        Test datalogging trigger conditions
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

static unsigned char _rx_buffer[128];
static unsigned char _tx_buffer[128];

static unsigned char forbidden_buffer[32];
static unsigned char forbidden_buffer2[32];
static unsigned char readonly_buffer[32];
static unsigned char readonly_buffer2[32];

class TestTriggerConditions : public ScrutinyTest
{
  protected:
    scrutiny::Timebase tb;
    scrutiny::MainHandler scrutiny_handler;
    scrutiny::Config config;

    scrutiny::AddressRange readonly_ranges[2];
    scrutiny::AddressRange forbidden_ranges[2];
    scrutiny::RuntimePublishedValue rpvs[2];

    TestTriggerConditions() :
        ScrutinyTest(),
        tb(),
        scrutiny_handler(),
        config()
    {
        readonly_ranges[0] = scrutiny::tools::make_address_range(readonly_buffer, sizeof(readonly_buffer)),
        readonly_ranges[1] = scrutiny::tools::make_address_range(readonly_buffer2, sizeof(readonly_buffer2));

        forbidden_ranges[0] = scrutiny::tools::make_address_range(forbidden_buffer, sizeof(forbidden_buffer));
        forbidden_ranges[1] = scrutiny::tools::make_address_range(forbidden_buffer2, sizeof(forbidden_buffer2));

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

TEST_F(TestTriggerConditions, AlwaysTrue)
{
    using always_true = scrutiny::datalogging::trigger::AlwaysTrueCondition;
    scrutiny::datalogging::trigger::ConditionSharedData cond_data;
    EXPECT_TRUE(always_true::evaluate(&cond_data, SCRUTINY_NULL, SCRUTINY_NULL));
}

TEST_F(TestTriggerConditions, OperatorEQ)
{
    scrutiny::datalogging::trigger::ConditionSharedData cond_data;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare::eVariableTypeCompare valtypes[2];
    using eq = scrutiny::datalogging::trigger::EqualCondition;

    vals[0]._float = 0.4f;
    vals[1]._float = 0.5f;
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;
    EXPECT_FALSE(eq::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 0.5f;
    EXPECT_TRUE(eq::evaluate(&cond_data, valtypes, vals));
}

TEST_F(TestTriggerConditions, OperatorNEQ)
{
    scrutiny::datalogging::trigger::ConditionSharedData cond_data;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare::eVariableTypeCompare valtypes[2];
    using neq = scrutiny::datalogging::trigger::NotEqualCondition;

    vals[0]._float = 0.5f;
    vals[1]._float = 0.5f;
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;
    EXPECT_FALSE(neq::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 0.4f;
    EXPECT_TRUE(neq::evaluate(&cond_data, valtypes, vals));
}

TEST_F(TestTriggerConditions, OperatorGT)
{
    scrutiny::datalogging::trigger::ConditionSharedData cond_data;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare::eVariableTypeCompare valtypes[2];
    using gt = scrutiny::datalogging::trigger::GreaterThanCondition;

    vals[0]._float = 0.4f;
    vals[1]._float = 0.5f;
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;
    EXPECT_FALSE(gt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 0.5f;
    EXPECT_FALSE(gt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 0.6f;
    EXPECT_TRUE(gt::evaluate(&cond_data, valtypes, vals));
}

TEST_F(TestTriggerConditions, OperatorGET)
{
    scrutiny::datalogging::trigger::ConditionSharedData cond_data;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare::eVariableTypeCompare valtypes[2];
    using get = scrutiny::datalogging::trigger::GreaterOrEqualThanCondition;

    vals[0]._float = 0.4f;
    vals[1]._float = 0.5f;
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;
    EXPECT_FALSE(get::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 0.5f;
    EXPECT_TRUE(get::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 0.6f;
    EXPECT_TRUE(get::evaluate(&cond_data, valtypes, vals));
}

TEST_F(TestTriggerConditions, OperatorLT)
{
    scrutiny::datalogging::trigger::ConditionSharedData cond_data;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare::eVariableTypeCompare valtypes[2];
    using lt = scrutiny::datalogging::trigger::LessThanCondition;

    vals[0]._float = 0.6f;
    vals[1]._float = 0.5f;
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;
    EXPECT_FALSE(lt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 0.5f;
    EXPECT_FALSE(lt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 0.4f;
    EXPECT_TRUE(lt::evaluate(&cond_data, valtypes, vals));
}

TEST_F(TestTriggerConditions, OperatorLET)
{
    scrutiny::datalogging::trigger::ConditionSharedData cond_data;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare::eVariableTypeCompare valtypes[2];
    using let = scrutiny::datalogging::trigger::LessOrEqualThanCondition;

    vals[0]._float = 0.6f;
    vals[1]._float = 0.5f;
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;
    EXPECT_FALSE(let::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 0.5f;
    EXPECT_TRUE(let::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 0.4f;
    EXPECT_TRUE(let::evaluate(&cond_data, valtypes, vals));
}

TEST_F(TestTriggerConditions, Equality_AllTypes)
{
    scrutiny::datalogging::trigger::ConditionSharedData cond_data;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare::eVariableTypeCompare valtypes[2];
    using eq = scrutiny::datalogging::trigger::EqualCondition;

    for (int i = 0; i < 2; i++)
    {
        const int op0 = (i == 0) ? 0 : 1;
        const int op1 = (i == 0) ? 1 : 0;

        vals[op0]._float = 0.5f;
        vals[op1]._float = 0.6f;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_float;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_float;
        EXPECT_FALSE(eq::evaluate(&cond_data, valtypes, vals));
        vals[op1]._float = 0.5f;
        EXPECT_TRUE(eq::evaluate(&cond_data, valtypes, vals));

        vals[op0]._uint = 2;
        vals[op1]._float = 2.1f;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_uint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_float;
        EXPECT_FALSE(eq::evaluate(&cond_data, valtypes, vals));
        vals[op1]._float = 2.0f;
        EXPECT_TRUE(eq::evaluate(&cond_data, valtypes, vals));

        vals[op0]._sint = 2;
        vals[op1]._float = 2.1f;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_sint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_float;
        EXPECT_FALSE(eq::evaluate(&cond_data, valtypes, vals));
        vals[op1]._float = 2.0f;
        EXPECT_TRUE(eq::evaluate(&cond_data, valtypes, vals));

        vals[op0]._sint = 2;
        vals[op1]._uint = 3;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_sint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_uint;
        EXPECT_FALSE(eq::evaluate(&cond_data, valtypes, vals));
        vals[op1]._uint = 2;
        EXPECT_TRUE(eq::evaluate(&cond_data, valtypes, vals));

        vals[op0]._uint = 2;
        vals[op1]._uint = 3;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_uint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_uint;
        EXPECT_FALSE(eq::evaluate(&cond_data, valtypes, vals));
        vals[op1]._uint = 2;
        EXPECT_TRUE(eq::evaluate(&cond_data, valtypes, vals));

        vals[op0]._sint = 2;
        vals[op1]._sint = 3;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_sint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_sint;
        EXPECT_FALSE(eq::evaluate(&cond_data, valtypes, vals));
        vals[op1]._sint = 2;
        EXPECT_TRUE(eq::evaluate(&cond_data, valtypes, vals));

        vals[op0]._sint = -1;
        vals[op1]._uint = static_cast<scrutiny::uint_biggest_t>(-1);
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_sint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_uint;
        EXPECT_FALSE(eq::evaluate(&cond_data, valtypes, vals));
    }
}

TEST_F(TestTriggerConditions, LT_MixedSignAndTypes)
{
    scrutiny::datalogging::trigger::ConditionSharedData cond_data;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare::eVariableTypeCompare valtypes[2];
    using lt = scrutiny::datalogging::trigger::LessThanCondition;

    for (int i = 0; i < 2; i++)
    {
        const int op0 = (i == 0) ? 0 : 1;
        const int op1 = (i == 0) ? 1 : 0;

        bool truefalse = (i == 0) ? false : true;

        vals[op0]._float = 1.0f;
        vals[op1]._sint = -5;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_float;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_sint;
        EXPECT_EQ(lt::evaluate(&cond_data, valtypes, vals), truefalse);
        vals[op0]._float = -6;
        EXPECT_EQ(lt::evaluate(&cond_data, valtypes, vals), !truefalse);

        vals[op0]._float = -1.0f;
        vals[op1]._uint = 0;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_float;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_uint;
        EXPECT_EQ(lt::evaluate(&cond_data, valtypes, vals), !truefalse);

        vals[op0]._sint = -6;
        vals[op1]._sint = -5;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_sint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_sint;
        EXPECT_EQ(lt::evaluate(&cond_data, valtypes, vals), !truefalse);

        vals[op0]._uint = static_cast<scrutiny::uint_biggest_t>(-2);
        vals[op1]._sint = -1;
        valtypes[op0] = scrutiny::datalogging::VariableTypeCompare::_uint;
        valtypes[op1] = scrutiny::datalogging::VariableTypeCompare::_sint;
        EXPECT_EQ(lt::evaluate(&cond_data, valtypes, vals), truefalse);
    }
}

TEST_F(TestTriggerConditions, ChangeMoreThan_Basic)
{
    scrutiny::datalogging::trigger::ConditionSharedData cond_data;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare::eVariableTypeCompare valtypes[2];
    using cmt = scrutiny::datalogging::trigger::ChangeMoreThanCondition;
    cmt::reset(&cond_data);

    // Test positive change
    vals[0]._float = 10.0f;
    vals[1]._float = 5.0f; // Positive change of 5.0
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;

    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._float = 11.0f;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._float = 5.0f; // Negative change has no effect
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._float = 10.1f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 11.0f;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 20.0f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 30.0f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));

    // Test negative change
    cmt::reset(&cond_data);
    vals[0]._float = 10.0f;
    vals[1]._float = -5.0f; // NEgative change of 5.0
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._float = 20.0f; // Positive change has no effect
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 14.9f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 14.0f;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 5.0f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = -5.0f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
}

TEST_F(TestTriggerConditions, ChangeMoreThan_AllTypes)
{
    scrutiny::datalogging::trigger::ConditionSharedData cond_data;
    scrutiny::datalogging::AnyTypeCompare vals[2];
    scrutiny::datalogging::VariableTypeCompare::eVariableTypeCompare valtypes[2];

    using cmt = scrutiny::datalogging::trigger::ChangeMoreThanCondition;
    // ===== Float - Sint
    cmt::reset(&cond_data);
    // Test positive change
    vals[0]._float = 10.0f;
    vals[1]._sint = 5; // Positive change of 5.0
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_sint;

    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._float = 11.0f;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._float = 5.0f; // Negative change has no effect
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._float = 10.1f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 11.0f;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 20.0f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 30.0f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));

    // Test negative change
    cmt::reset(&cond_data);
    vals[0]._float = 10.0f;
    vals[1]._sint = -5; // NEgative change of 5.0
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._float = 20.0f; // Positive change has no effect
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 14.9f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 14.0f;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 5.0f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = -5.0f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));

    // ===== Float - Uint
    cmt::reset(&cond_data);
    // Test positive change
    vals[0]._float = 10.0f;
    vals[1]._uint = 5; // Positive change of 5.0
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_uint;

    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._float = 11.0f;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._float = 5.0f; // Negative change has no effect
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._float = 10.1f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 11.0f;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 20.0f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._float = 30.0f;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));

    // ===== Uint - Uint
    cmt::reset(&cond_data);
    // Test positive change
    vals[0]._uint = static_cast<scrutiny::uint_biggest_t>(-1000);
    vals[1]._uint = 5; // Positive change of 5.0
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_uint;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_uint;

    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._uint += 1;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._uint -= 10; // Negative change has no effect
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._uint += 6;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._uint += 2;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._uint += 6;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._uint += 10;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));

    // ===== Uint - Sint
    cmt::reset(&cond_data);
    // Test positive change
    vals[0]._uint = static_cast<scrutiny::uint_biggest_t>(-1000);
    ;
    vals[1]._sint = 5; // Positive change of 5.0
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_uint;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_sint;

    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._uint += 1;
    ;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._uint -= 10; // Negative change has no effect
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._uint += 6;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._uint += 4;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._uint += 10;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._uint += 6;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));

    // Test negative change
    cmt::reset(&cond_data);
    vals[0]._uint = static_cast<scrutiny::uint_biggest_t>(-1000);
    vals[1]._sint = -5; // NEgative change of 5.0
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._uint += 20; // Positive change has no effect
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._uint -= 6;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._uint -= 4;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._uint -= 100;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._uint -= 10;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));

    // ===== Sint - Sint
    cmt::reset(&cond_data);
    // Test positive change
    vals[0]._sint = -100;
    ;
    vals[1]._sint = 5; // Positive change of 5.0
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_sint;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_sint;

    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._sint += 1;
    ;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._sint -= 10; // Negative change has no effect
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._sint += 6;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._sint += 4;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._sint += 10;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._sint += 6;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));

    // Test negative change
    cmt::reset(&cond_data);
    vals[0]._sint = -100;
    vals[1]._sint = -5; // NEgative change of 5.0
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._sint += 20; // Positive change has no effect
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._sint += -6;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._sint += -4;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._sint += -100;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._sint += -10;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));

    // ===== Sint - Uint
    cmt::reset(&cond_data);
    // Test positive change
    vals[0]._sint = -100;
    ;
    vals[1]._uint = 5; // Positive change of 5.0
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_sint;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_sint;

    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._uint += 1;
    ;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._uint -= 10; // Negative change has no effect
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));

    vals[0]._uint += 6;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._uint += 4;
    EXPECT_FALSE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._uint += 10;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
    vals[0]._uint += 6;
    EXPECT_TRUE(cmt::evaluate(&cond_data, valtypes, vals));
}

TEST_F(TestTriggerConditions, IsWithin)
{
    scrutiny::datalogging::trigger::ConditionSharedData cond_data;
    scrutiny::datalogging::AnyTypeCompare vals[3];
    scrutiny::datalogging::VariableTypeCompare::eVariableTypeCompare valtypes[3];
    using is_within = scrutiny::datalogging::trigger::IsWithinCondition;

    is_within::reset(&cond_data);
    // Test positive change
    vals[0]._float = 10.0f;
    vals[1]._float = 5.0f;
    vals[2]._float = 1.0f;
    valtypes[0] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[1] = scrutiny::datalogging::VariableTypeCompare::_float;
    valtypes[2] = scrutiny::datalogging::VariableTypeCompare::_float;

    EXPECT_FALSE(is_within::evaluate(&cond_data, valtypes, vals));

    vals[1]._float = 11.01f;
    EXPECT_FALSE(is_within::evaluate(&cond_data, valtypes, vals));

    vals[1]._float = 11.00f;
    EXPECT_TRUE(is_within::evaluate(&cond_data, valtypes, vals));

    vals[1]._float = 8.99f;
    EXPECT_FALSE(is_within::evaluate(&cond_data, valtypes, vals));

    vals[1]._float = 9.0f;
    EXPECT_TRUE(is_within::evaluate(&cond_data, valtypes, vals));
}
