#include <gtest/gtest.h>
#include <cstring>
#include <string>

#include "scrutiny.hpp"
#include "scrutiny_test.hpp"

using namespace scrutiny;

static bool rpv_read_callback(RuntimePublishedValue rpv, AnyType *outval)
{
    if (rpv.id == 0x8888 && rpv.type == VariableType::float32)
    {
        outval->float32 = 1.7f;
    }
    else
    {
        return false;
    }

    return true;
}

class TestDatalogControl : public ScrutinyTest
{
protected:
    static constexpr uint32_t FIXED_FREQ_LOOP_TIMESTEP_US = 100u;
    Timebase tb;
    MainHandler scrutiny_handler;
    Config config;

    uint8_t _rx_buffer[1024];
    uint8_t _tx_buffer[1024];
    uint8_t dlbuffer[256];
    LoopHandler *loops[2];

    FixedFrequencyLoopHandler fixed_freq_loop;
    VariableFrequencyLoopHandler variable_freq_loop;

    RuntimePublishedValue rpvs[1] = {
        {0x8888, VariableType::float32}};

    TestDatalogControl() : ScrutinyTest(), fixed_freq_loop(FIXED_FREQ_LOOP_TIMESTEP_US, "Loop1"),
                           variable_freq_loop("Loop2") {}
#if SCRUTINY_ENABLE_DATALOGGING

    uint16_t encode_datalogger_config(uint8_t loop_id, const datalogging::Configuration *dlconfig, uint8_t *buffer, uint32_t max_size);
    datalogging::Configuration get_valid_reference_configuration();
    void test_configure(uint8_t loop_id, datalogging::Configuration refconfig, protocol::ResponseCode expected_code, bool check_response = true, std::string error_msg = "");

    float m_some_var_operand1 = 0;
    float m_some_var_logged1 = 0;
#endif
    virtual void SetUp()
    {
        loops[0] = &fixed_freq_loop;
        loops[1] = &variable_freq_loop;

        config.set_buffers(_rx_buffer, sizeof(_rx_buffer), _tx_buffer, sizeof(_tx_buffer));
        config.set_loops(loops, sizeof(loops) / sizeof(loops[0]));

        config.set_published_values(rpvs, sizeof(rpvs) / sizeof(rpvs[0]), rpv_read_callback);

#if SCRUTINY_ENABLE_DATALOGGING
        config.set_datalogging_buffers(dlbuffer, sizeof(dlbuffer));
#endif
        scrutiny_handler.init(&config);
        scrutiny_handler.comm()->connect();
    }
};

#if !SCRUTINY_ENABLE_DATALOGGING

// Expect all subfunctions to return Unsupported feature if not supported
TEST_F(TestDatalogControl, TestUnsupported)
{
    const protocol::CommandId cmd = protocol::CommandId::DataLogControl;
    const protocol::ResponseCode failure = protocol::ResponseCode::UnsupportedFeature;

    uint8_t tx_buffer[32];

    for (uint16_t i = 0; i <= 255u; i++)
    {
        const uint8_t subfn = static_cast<uint8_t>(i);
        uint8_t request_data[8] = {5, subfn, 0, 0};
        add_crc(request_data, sizeof(request_data) - 4);

        scrutiny_handler.comm()->receive_data(request_data, sizeof(request_data));
        scrutiny_handler.process(0);

        uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
        ASSERT_LT(n_to_read, sizeof(tx_buffer));
        ASSERT_GT(n_to_read, 0);

        scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
        ASSERT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, failure));
        scrutiny_handler.process(0);
    }
}
#else

uint16_t TestDatalogControl::encode_datalogger_config(uint8_t loop_id, const datalogging::Configuration *dlconfig, uint8_t *buffer, uint32_t max_size)
{
    uint32_t cursor = 0;
    if (max_size < 1 + 2 + 1 + 4 + 1 + 4 + 1)
    {
        return 0;
    }
    cursor += codecs::encode_8_bits(static_cast<uint8_t>(loop_id), &buffer[cursor]);
    cursor += codecs::encode_16_bits_big_endian(dlconfig->decimation, &buffer[cursor]);
    cursor += codecs::encode_8_bits(dlconfig->probe_location, &buffer[cursor]);
    cursor += codecs::encode_32_bits_big_endian(dlconfig->timeout_us, &buffer[cursor]);
    cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->trigger.condition), &buffer[cursor]);
    cursor += codecs::encode_32_bits_big_endian(dlconfig->trigger.hold_time_us, &buffer[cursor]);
    cursor += codecs::encode_8_bits(dlconfig->trigger.operand_count, &buffer[cursor]);

    for (uint32_t i = 0; i < dlconfig->trigger.operand_count; i++)
    {
        if (cursor + 1 > max_size)
        {
            return 0;
        }
        cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->trigger.operands[i].type), &buffer[cursor]);
        switch (dlconfig->trigger.operands[i].type)
        {
        case datalogging::OperandType::LITERAL:
            if (cursor + 4 > max_size)
            {
                return 0;
            }
            codecs::encode_float_big_endian(dlconfig->trigger.operands[i].data.literal.val, &buffer[cursor]);
            cursor += sizeof(float);
            break;
        case datalogging::OperandType::RPV:
            if (cursor + 2 > max_size)
            {
                return 0;
            }
            codecs::encode_16_bits_big_endian(dlconfig->trigger.operands[i].data.rpv.id, &buffer[cursor]);
            cursor += sizeof(uint16_t);
            break;
        case datalogging::OperandType::VAR:
            if (cursor + 1 + sizeof(void *) > max_size)
            {
                return 0;
            }
            cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->trigger.operands[i].data.var.datatype), &buffer[cursor]);
            cursor += codecs::encode_address_big_endian(dlconfig->trigger.operands[i].data.var.addr, &buffer[cursor]);
            break;

        case datalogging::OperandType::VARBIT:
            if (cursor + 1 + 1 + 1 + sizeof(void *) > max_size)
            {
                return 0;
            }
            cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->trigger.operands[i].data.varbit.datatype), &buffer[cursor]);
            cursor += codecs::encode_address_big_endian(dlconfig->trigger.operands[i].data.varbit.addr, &buffer[cursor]);
            cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->trigger.operands[i].data.varbit.bitoffset), &buffer[cursor]);
            cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->trigger.operands[i].data.varbit.bitsize), &buffer[cursor]);
            break;
        }
    }
    if (cursor + 1 > max_size)
    {
        return 0;
    }

    cursor += codecs::encode_8_bits(dlconfig->items_count, &buffer[cursor]);
    for (uint32_t i = 0; i < dlconfig->items_count; i++)
    {
        if (cursor + 1 > max_size)
        {
            return 0;
        }
        cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->items_to_log[i].type), &buffer[cursor]);
        switch (dlconfig->items_to_log[i].type)
        {
        case datalogging::LoggableType::TIME:
            break;
        case datalogging::LoggableType::MEMORY:
            if (cursor + 1 + sizeof(void *) > max_size)
            {
                return 0;
            }
            cursor += codecs::encode_address_big_endian(dlconfig->items_to_log[i].data.memory.address, &buffer[cursor]);
            cursor += codecs::encode_8_bits(dlconfig->items_to_log[i].data.memory.size, &buffer[cursor]);
            break;
        case datalogging::LoggableType::RPV:
            if (cursor + 2 > max_size)
            {
                return 0;
            }
            cursor += codecs::encode_16_bits_big_endian(dlconfig->items_to_log[i].data.rpv.id, &buffer[cursor]);
            break;
        }
    }

    return cursor;
}

datalogging::Configuration TestDatalogControl::get_valid_reference_configuration()
{
    datalogging::Configuration refconfig;

    refconfig.decimation = 0x1234;
    refconfig.probe_location = 123;
    refconfig.timeout_us = 0x11223344;
    refconfig.items_count = 3;

    refconfig.items_to_log[0].type = datalogging::LoggableType::TIME;
    refconfig.items_to_log[1].type = datalogging::LoggableType::MEMORY;
    refconfig.items_to_log[1].data.memory.address = &m_some_var_logged1;
    refconfig.items_to_log[1].data.memory.size = sizeof(m_some_var_logged1);
    refconfig.items_to_log[2].type = datalogging::LoggableType::RPV;
    refconfig.items_to_log[2].data.rpv.id = 0x8888;

    refconfig.trigger.condition = datalogging::SupportedTriggerConditions::Equal;
    refconfig.trigger.hold_time_us = 0xaabbccdd;
    refconfig.trigger.operand_count = 2;
    refconfig.trigger.operands[0].type = datalogging::OperandType::LITERAL;
    refconfig.trigger.operands[0].data.literal.val = 3.1415926f;
    refconfig.trigger.operands[1].type = datalogging::OperandType::VAR;
    refconfig.trigger.operands[1].data.var.addr = &m_some_var_operand1;
    refconfig.trigger.operands[1].data.var.datatype = VariableType::float32;

    return refconfig;
}

void TestDatalogControl::test_configure(uint8_t loop_id, datalogging::Configuration refconfig, protocol::ResponseCode expected_code, bool check_response, std::string error_msg)
{
    uint8_t request_data[1024] = {5, 2};
    uint16_t payload_size = encode_datalogger_config(loop_id, &refconfig, &request_data[4], sizeof(request_data));
    ASSERT_GT(sizeof(request_data), payload_size + 8) << error_msg;
    ASSERT_NE(payload_size, 0) << error_msg;
    request_data[2] = (payload_size >> 8) & 0xFF;
    request_data[3] = payload_size & 0xFF;
    add_crc(request_data, 4 + payload_size);

    scrutiny_handler.comm()->receive_data(request_data, payload_size + 8);
    scrutiny_handler.process(0);

    uint8_t tx_buffer[32];
    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();

    if (check_response)
    {
        EXPECT_EQ(n_to_read, 9) << error_msg;

        scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
        scrutiny_handler.process(0);
        EXPECT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, protocol::CommandId::DataLogControl, 2, expected_code)) << error_msg;
    }
    if (expected_code == protocol::ResponseCode::OK)
    {
        EXPECT_TRUE(scrutiny_handler.datalogger()->config_valid()) << error_msg;
    }
    else
    {
        EXPECT_FALSE(scrutiny_handler.datalogger()->config_valid()) << error_msg;
    }
}

TEST_F(TestDatalogControl, TestGetBufferSize)
{
    uint8_t tx_buffer[32];
    uint32_t buffer_size = sizeof(dlbuffer);

    uint8_t request_data[8] = {5, 1, 0, 0};
    add_crc(request_data, sizeof(request_data) - 4);

    // Make expected response
    uint8_t expected_response[9 + 4] = {0x85, 1, 0, 0, 4};
    expected_response[5] = static_cast<uint8_t>((buffer_size >> 24) & 0xFF);
    expected_response[6] = static_cast<uint8_t>((buffer_size >> 16) & 0xFF);
    expected_response[7] = static_cast<uint8_t>((buffer_size >> 8) & 0xFF);
    expected_response[8] = static_cast<uint8_t>((buffer_size >> 0) & 0xFF);
    add_crc(expected_response, sizeof(expected_response) - 4);

    scrutiny_handler.comm()->receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    ASSERT_GT(n_to_read, 0);

    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    EXPECT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

TEST_F(TestDatalogControl, TestConfigureValid1)
{
    constexpr uint8_t loop_id = 1;
    datalogging::Configuration refconfig = get_valid_reference_configuration();

    test_configure(loop_id, refconfig, protocol::ResponseCode::OK);

    const datalogging::Configuration *dlconfig = scrutiny_handler.datalogger()->config();

    EXPECT_EQ(dlconfig->decimation, refconfig.decimation);
    EXPECT_EQ(dlconfig->probe_location, refconfig.probe_location);
    EXPECT_EQ(dlconfig->timeout_us, refconfig.timeout_us);
    ASSERT_EQ(dlconfig->items_count, refconfig.items_count);
    EXPECT_EQ(dlconfig->items_to_log[0].type, refconfig.items_to_log[0].type);
    EXPECT_EQ(dlconfig->items_to_log[1].type, refconfig.items_to_log[1].type);
    EXPECT_EQ(dlconfig->items_to_log[1].data.memory.address, refconfig.items_to_log[1].data.memory.address);
    EXPECT_EQ(dlconfig->items_to_log[1].data.memory.size, refconfig.items_to_log[1].data.memory.size);
    EXPECT_EQ(dlconfig->items_to_log[2].type, refconfig.items_to_log[2].type);
    EXPECT_EQ(dlconfig->items_to_log[2].data.rpv.id, refconfig.items_to_log[2].data.rpv.id);
    EXPECT_EQ(dlconfig->trigger.condition, refconfig.trigger.condition);
    EXPECT_EQ(dlconfig->trigger.hold_time_us, refconfig.trigger.hold_time_us);
    EXPECT_EQ(dlconfig->trigger.operand_count, refconfig.trigger.operand_count);
    EXPECT_EQ(dlconfig->trigger.operands[0].type, refconfig.trigger.operands[0].type);
    EXPECT_EQ(dlconfig->trigger.operands[0].data.literal.val, refconfig.trigger.operands[0].data.literal.val);
    EXPECT_EQ(dlconfig->trigger.operands[1].type, refconfig.trigger.operands[1].type);
    EXPECT_EQ(dlconfig->trigger.operands[1].data.var.addr, refconfig.trigger.operands[1].data.var.addr);
    EXPECT_EQ(dlconfig->trigger.operands[1].data.var.datatype, refconfig.trigger.operands[1].data.var.datatype);

    EXPECT_TRUE(scrutiny_handler.datalogger()->config_valid());
}

TEST_F(TestDatalogControl, TestConfigureBadLoopID)
{
    constexpr uint8_t loop_id = 2; // This loop does not exists
    datalogging::Configuration refconfig = get_valid_reference_configuration();
    test_configure(loop_id, refconfig, protocol::ResponseCode::FailureToProceed);
}

TEST_F(TestDatalogControl, TestConfigureItemCountOverflow)
{
    constexpr uint8_t loop_id = 1;
    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.items_count = SCRUTINY_DATALOGGING_MAX_SIGNAL + 1;
    test_configure(loop_id, refconfig, protocol::ResponseCode::Overflow);
}

TEST_F(TestDatalogControl, TestConfigureOperandCountOverflow)
{
    constexpr uint8_t loop_id = 1;
    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.trigger.operand_count = datalogging::MAX_OPERANDS + 1;
    test_configure(loop_id, refconfig, protocol::ResponseCode::Overflow);
}

TEST_F(TestDatalogControl, TestConfigureOperandCountMismatch)
{
    constexpr uint8_t loop_id = 1;
    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.trigger.operand_count = 0; // Doesn'T match the condition set
    test_configure(loop_id, refconfig, protocol::ResponseCode::InvalidRequest);
}

TEST_F(TestDatalogControl, TestConfigureBadOperands)
{
    float bad_values[] = {
        std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::quiet_NaN(),
        std::numeric_limits<float>::signaling_NaN(),
        0.0f / 0.0f};

    constexpr uint8_t loop_id = 1;
    for (unsigned int i = 0; i < sizeof(bad_values) / sizeof(float); i++)
    {
        std::string errpr_msg = std::string("i=") + std::to_string(i);
        datalogging::Configuration refconfig = get_valid_reference_configuration();
        refconfig.trigger.operands[0].type = datalogging::OperandType::LITERAL;
        refconfig.trigger.operands[0].data.literal.val = bad_values[i];
        test_configure(loop_id, refconfig, protocol::ResponseCode::InvalidRequest, true, errpr_msg);
    }
}

TEST_F(TestDatalogControl, TestConfigureOperandBadRPV)
{
    constexpr uint8_t loop_id = 1;
    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.trigger.operands[0].type = datalogging::OperandType::RPV;
    refconfig.trigger.operands[0].data.rpv.id = 0x9999; // doesn't exist

    test_configure(loop_id, refconfig, protocol::ResponseCode::FailureToProceed);
}

TEST_F(TestDatalogControl, TestConfigureLoggableBadRPV)
{
    constexpr uint8_t loop_id = 1;
    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.items_to_log[0].type = datalogging::LoggableType::RPV;
    refconfig.items_to_log[0].data.rpv.id = 0x9999; // doesn't exist

    test_configure(loop_id, refconfig, protocol::ResponseCode::FailureToProceed);
}

TEST_F(TestDatalogControl, TestOwnerMechanism)
{
    ASSERT_FALSE(fixed_freq_loop.owns_datalogger());
    ASSERT_FALSE(variable_freq_loop.owns_datalogger());

    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.decimation = 1;
    refconfig.probe_location = 128;
    refconfig.timeout_us = 0;
    refconfig.trigger.hold_time_us = 0;

    test_configure(0, refconfig, protocol::ResponseCode::OK); // Assign to Loop 0 (Fixed freq)
    scrutiny_handler.process(0);
    scrutiny_handler.process(0);
    EXPECT_FALSE(fixed_freq_loop.owns_datalogger());
    fixed_freq_loop.process();
    EXPECT_TRUE(fixed_freq_loop.owns_datalogger());
    scrutiny_handler.process(0);
    scrutiny_handler.datalogger()->arm_trigger();
    scrutiny_handler.datalogger()->force_trigger();
    for (uint32_t i = 0; i < sizeof(dlbuffer) / 4; i++)
    {
        fixed_freq_loop.process();
        if (scrutiny_handler.datalogger()->data_acquired())
        {
            break;
        }
    }
    EXPECT_TRUE(scrutiny_handler.datalogger()->data_acquired());
    EXPECT_FALSE(scrutiny_handler.datalogging_data_available());
    scrutiny_handler.process(1); // Receive the IPC message here
    EXPECT_TRUE(scrutiny_handler.datalogging_data_available());

    // Switch loop.
    test_configure(1, refconfig, protocol::ResponseCode::OK, false); // Assign to Loop 0 (Fixed freq)

    EXPECT_TRUE(fixed_freq_loop.owns_datalogger());     // I'm the owner!
    EXPECT_FALSE(variable_freq_loop.owns_datalogger()); // Not me :(

    scrutiny_handler.process(0);   // Don't process because not released yet
    scrutiny_handler.process(0);   // Don't process because not released yet
    scrutiny_handler.process(0);   // Don't process because not released yet
    fixed_freq_loop.process();     // Finally release the datalogger
    scrutiny_handler.process(1);   // Process the request
    scrutiny_handler.process(1);   // Request for new ownership
    variable_freq_loop.process(1); // Take ownership

    EXPECT_FALSE(fixed_freq_loop.owns_datalogger());   // I used to be cool, but they changed what cool was.
    EXPECT_TRUE(variable_freq_loop.owns_datalogger()); // Look at me, i'm the owner now.

    scrutiny_handler.process(0);
    scrutiny_handler.datalogger()->arm_trigger();
    scrutiny_handler.datalogger()->force_trigger();
    for (uint32_t i = 0; i < sizeof(dlbuffer) / 4; i++)
    {
        variable_freq_loop.process(1); // Process a different loop
        if (scrutiny_handler.datalogger()->data_acquired())
        {
            break;
        }
    }
    EXPECT_TRUE(scrutiny_handler.datalogger()->data_acquired());
    EXPECT_FALSE(scrutiny_handler.datalogging_data_available());
    scrutiny_handler.process(1); // Receive the IPC message here
    EXPECT_TRUE(scrutiny_handler.datalogging_data_available());
}

TEST_F(TestDatalogControl, TestArmTriggerNotConfigured)
{
    uint8_t tx_buffer[32];

    uint8_t request_data[8] = {5, 3, 0, 0};
    add_crc(request_data, sizeof(request_data) - 4);

    scrutiny_handler.comm()->receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    ASSERT_GT(n_to_read, 0);

    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);

    ASSERT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, protocol::CommandId::DataLogControl, 3, protocol::ResponseCode::FailureToProceed));
    EXPECT_FALSE(scrutiny_handler.datalogger()->armed());
    fixed_freq_loop.process();
    EXPECT_FALSE(scrutiny_handler.datalogger()->armed());
}

TEST_F(TestDatalogControl, TestArmDisarmTriggerOK)
{
    uint8_t tx_buffer[32];
    uint16_t n_to_read;
    datalogging::Configuration refconfig = get_valid_reference_configuration();
    test_configure(0, refconfig, protocol::ResponseCode::OK); // Assign to Loop 0 (Fixed freq)
    fixed_freq_loop.process();                                // Accept ownership
    scrutiny_handler.process(0);

    // Arm trigger
    uint8_t arm_request_data[8] = {5, 3, 0, 0};
    add_crc(arm_request_data, sizeof(arm_request_data) - 4);

    uint8_t arm_expected_response[9] = {0x85, 3, 0, 0, 0};
    add_crc(arm_expected_response, sizeof(arm_expected_response) - 4);

    scrutiny_handler.comm()->receive_data(arm_request_data, sizeof(arm_request_data));
    scrutiny_handler.process(0);

    n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    ASSERT_GT(n_to_read, 0);

    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    scrutiny_handler.process(0);
    EXPECT_BUF_EQ(tx_buffer, arm_expected_response, sizeof(arm_expected_response));
    EXPECT_FALSE(scrutiny_handler.datalogger()->armed());
    fixed_freq_loop.process();
    EXPECT_TRUE(scrutiny_handler.datalogger()->armed());

    // Disarm trigger
    uint8_t disarm_request_data[8] = {5, 4, 0, 0};
    add_crc(disarm_request_data, sizeof(disarm_request_data) - 4);

    uint8_t diarm_expected_response[9] = {0x85, 4, 0, 0, 0};
    add_crc(diarm_expected_response, sizeof(diarm_expected_response) - 4);

    scrutiny_handler.comm()->receive_data(disarm_request_data, sizeof(disarm_request_data));
    scrutiny_handler.process(0);

    n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    ASSERT_GT(n_to_read, 0);

    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    scrutiny_handler.process(0);
    EXPECT_BUF_EQ(tx_buffer, diarm_expected_response, sizeof(diarm_expected_response));
    EXPECT_TRUE(scrutiny_handler.datalogger()->armed()); // Still true
    fixed_freq_loop.process();
    EXPECT_FALSE(scrutiny_handler.datalogger()->armed());
}

#endif
