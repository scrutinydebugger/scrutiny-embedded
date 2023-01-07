//    test_datalog_control.cpp
//        Test the DataLogControl command used to configure, control and reads the datalogger
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

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

    uint16_t encode_datalogger_config(uint8_t loop_id, uint16_t config_id, const datalogging::Configuration *dlconfig, uint8_t *buffer, uint16_t max_size);
    datalogging::Configuration get_valid_reference_configuration();
    void test_configure(uint8_t loop_id, uint16_t config_id, datalogging::Configuration refconfig, protocol::ResponseCode expected_code, bool check_response = true, std::string error_msg = "");
    void check_get_status(datalogging::DataLogger::State expected_state);

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

/// @brief Encodes a datalogging config to be received by Configure subfunction
/// @param loop_id The loop ID to which runs the datalogger
/// @param config_id A Configuration ID that will be associated with the config
/// @param dlconfig The configuration to encode
/// @param buffer The destination buffer
/// @param max_size The buffer max size.
/// @return  Number of bytes written. Will be 0 in case of overflow
uint16_t TestDatalogControl::encode_datalogger_config(uint8_t loop_id, uint16_t config_id, const datalogging::Configuration *dlconfig, uint8_t *buffer, uint16_t max_size)
{
    uint16_t cursor = 0;
    if (max_size < 1 + 2 + 2 + 1 + 4 + 1 + 4 + 1)
    {
        return 0;
    }
    cursor += codecs::encode_8_bits(loop_id, &buffer[cursor]);
    cursor += codecs::encode_16_bits_big_endian(config_id, &buffer[cursor]);
    cursor += codecs::encode_16_bits_big_endian(dlconfig->decimation, &buffer[cursor]);
    cursor += codecs::encode_8_bits(dlconfig->probe_location, &buffer[cursor]);
    cursor += codecs::encode_32_bits_big_endian(dlconfig->timeout_100ns, &buffer[cursor]);
    cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->trigger.condition), &buffer[cursor]);
    cursor += codecs::encode_32_bits_big_endian(dlconfig->trigger.hold_time_100ns, &buffer[cursor]);
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

/// @brief Return a valid configuration used across the whole test suite
datalogging::Configuration TestDatalogControl::get_valid_reference_configuration()
{
    datalogging::Configuration refconfig;

    refconfig.decimation = 0x1234;
    refconfig.probe_location = 123;
    refconfig.timeout_100ns = 0x11223344;
    refconfig.items_count = 3;

    refconfig.items_to_log[0].type = datalogging::LoggableType::TIME;
    refconfig.items_to_log[1].type = datalogging::LoggableType::MEMORY;
    refconfig.items_to_log[1].data.memory.address = &m_some_var_logged1;
    refconfig.items_to_log[1].data.memory.size = sizeof(m_some_var_logged1);
    refconfig.items_to_log[2].type = datalogging::LoggableType::RPV;
    refconfig.items_to_log[2].data.rpv.id = 0x8888;

    refconfig.trigger.condition = datalogging::SupportedTriggerConditions::Equal;
    refconfig.trigger.hold_time_100ns = 0xaabbccdd;
    refconfig.trigger.operand_count = 2;
    refconfig.trigger.operands[0].type = datalogging::OperandType::LITERAL;
    refconfig.trigger.operands[0].data.literal.val = 3.1415926f;
    refconfig.trigger.operands[1].type = datalogging::OperandType::VAR;
    refconfig.trigger.operands[1].data.var.addr = &m_some_var_operand1;
    refconfig.trigger.operands[1].data.var.datatype = VariableType::float32;

    return refconfig;
}

/// @brief Runs a test for DataLogControl-Configure command.
/// @param loop_id Loop ID to configure
/// @param config_id  The configuraiton ID
/// @param refconfig The datalog coniguration
/// @param expected_code Expected response code returned through CommHandler
/// @param check_response When true, make sure the respons eis valid.
/// @param error_msg Error message to log in case of failure
void TestDatalogControl::test_configure(uint8_t loop_id, uint16_t config_id, datalogging::Configuration refconfig, protocol::ResponseCode expected_code, bool check_response, std::string error_msg)
{
    uint8_t request_data[1024] = {5, 2};
    uint16_t payload_size = encode_datalogger_config(loop_id, config_id, &refconfig, &request_data[4], sizeof(request_data));
    ASSERT_GT(sizeof(request_data), payload_size + 8) << error_msg;
    ASSERT_NE(payload_size, 0) << error_msg;
    request_data[2] = (payload_size >> 8) & 0xFF;
    request_data[3] = payload_size & 0xFF;
    add_crc(request_data, 4 + payload_size);

    scrutiny_handler.comm()->receive_data(request_data, payload_size + 8);
    scrutiny_handler.process(0);

    uint8_t tx_buffer[32];
    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);

    if (check_response)
    {
        ASSERT_EQ(n_to_read, 9) << error_msg;

        scrutiny_handler.process(0);
        EXPECT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, protocol::CommandId::DataLogControl, 2, expected_code)) << error_msg;

        if (expected_code == protocol::ResponseCode::OK)
        {
            EXPECT_TRUE(scrutiny_handler.datalogger()->config_valid()) << error_msg;
        }
        else
        {
            EXPECT_FALSE(scrutiny_handler.datalogger()->config_valid()) << error_msg;
        }
    }
}

TEST_F(TestDatalogControl, TestGetSetup)
{
    uint8_t tx_buffer[32];
    uint32_t buffer_size = sizeof(dlbuffer);

    uint8_t request_data[8] = {5, 1, 0, 0};
    add_crc(request_data, sizeof(request_data) - 4);

    // Make expected response
    uint8_t expected_response[9 + 4 + 1] = {0x85, 1, 0, 0, 5};
    codecs::encode_32_bits_big_endian(buffer_size, &expected_response[5]);
#if SCRUTINY_DATALOGGING_ENCODING == SCRUTINY_DATALOGGING_ENCODING_RAW
    expected_response[9] = static_cast<uint8_t>(datalogging::EncodingType::RAW);
#else
#error Unkown encoding
#endif
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
    constexpr uint16_t config_id = 0xaa55;
    datalogging::Configuration refconfig = get_valid_reference_configuration();

    test_configure(loop_id, config_id, refconfig, protocol::ResponseCode::OK);

    const datalogging::Configuration *dlconfig = scrutiny_handler.datalogger()->config();
    EXPECT_EQ(scrutiny_handler.datalogger()->get_config_id(), config_id);

    EXPECT_EQ(dlconfig->decimation, refconfig.decimation);
    EXPECT_EQ(dlconfig->probe_location, refconfig.probe_location);
    EXPECT_EQ(dlconfig->timeout_100ns, refconfig.timeout_100ns);
    ASSERT_EQ(dlconfig->items_count, refconfig.items_count);
    EXPECT_EQ(dlconfig->items_to_log[0].type, refconfig.items_to_log[0].type);
    EXPECT_EQ(dlconfig->items_to_log[1].type, refconfig.items_to_log[1].type);
    EXPECT_EQ(dlconfig->items_to_log[1].data.memory.address, refconfig.items_to_log[1].data.memory.address);
    EXPECT_EQ(dlconfig->items_to_log[1].data.memory.size, refconfig.items_to_log[1].data.memory.size);
    EXPECT_EQ(dlconfig->items_to_log[2].type, refconfig.items_to_log[2].type);
    EXPECT_EQ(dlconfig->items_to_log[2].data.rpv.id, refconfig.items_to_log[2].data.rpv.id);
    EXPECT_EQ(dlconfig->trigger.condition, refconfig.trigger.condition);
    EXPECT_EQ(dlconfig->trigger.hold_time_100ns, refconfig.trigger.hold_time_100ns);
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
    test_configure(loop_id, 0, refconfig, protocol::ResponseCode::FailureToProceed);
}

TEST_F(TestDatalogControl, TestConfigureItemCountOverflow)
{
    constexpr uint8_t loop_id = 1;
    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.items_count = SCRUTINY_DATALOGGING_MAX_SIGNAL + 1;
    test_configure(loop_id, 0, refconfig, protocol::ResponseCode::Overflow);
}

TEST_F(TestDatalogControl, TestConfigureOperandCountOverflow)
{
    constexpr uint8_t loop_id = 1;
    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.trigger.operand_count = datalogging::MAX_OPERANDS + 1;
    test_configure(loop_id, 0, refconfig, protocol::ResponseCode::Overflow);
}

TEST_F(TestDatalogControl, TestConfigureOperandCountMismatch)
{
    constexpr uint8_t loop_id = 1;
    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.trigger.operand_count = 0; // Doesn'T match the condition set
    test_configure(loop_id, 0, refconfig, protocol::ResponseCode::InvalidRequest);
}

TEST_F(TestDatalogControl, TestConfigureBadOperands)
{
    float bad_values[] = {
        std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::quiet_NaN(),
        std::numeric_limits<float>::signaling_NaN()};

    constexpr uint8_t loop_id = 1;
    for (unsigned int i = 0; i < sizeof(bad_values) / sizeof(float); i++)
    {
        std::string error_msg = std::string("i=") + std::to_string(i);
        datalogging::Configuration refconfig = get_valid_reference_configuration();
        refconfig.trigger.operands[0].type = datalogging::OperandType::LITERAL;
        refconfig.trigger.operands[0].data.literal.val = bad_values[i];
        test_configure(loop_id, 0, refconfig, protocol::ResponseCode::InvalidRequest, true, error_msg);
    }
}

TEST_F(TestDatalogControl, TestConfigureOperandBadRPV)
{
    constexpr uint8_t loop_id = 1;
    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.trigger.operands[0].type = datalogging::OperandType::RPV;
    refconfig.trigger.operands[0].data.rpv.id = 0x9999; // doesn't exist

    test_configure(loop_id, 0, refconfig, protocol::ResponseCode::FailureToProceed);
}

TEST_F(TestDatalogControl, TestConfigureUnknownCondition)
{
    constexpr uint8_t loop_id = 1;
    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.trigger.condition = static_cast<datalogging::SupportedTriggerConditions>(0xAA);

    test_configure(loop_id, 0, refconfig, protocol::ResponseCode::InvalidRequest);
}

TEST_F(TestDatalogControl, TestConfigureLoggableBadRPV)
{
    constexpr uint8_t loop_id = 1;
    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.items_to_log[0].type = datalogging::LoggableType::RPV;
    refconfig.items_to_log[0].data.rpv.id = 0x9999; // doesn't exist

    test_configure(loop_id, 0, refconfig, protocol::ResponseCode::FailureToProceed);
}

TEST_F(TestDatalogControl, TestOwnerMechanism)
{
    ASSERT_FALSE(fixed_freq_loop.owns_datalogger());
    ASSERT_FALSE(variable_freq_loop.owns_datalogger());

    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.decimation = 1;
    refconfig.probe_location = 128;
    refconfig.timeout_100ns = 0;
    refconfig.trigger.hold_time_100ns = 0;

    test_configure(0, 0, refconfig, protocol::ResponseCode::OK); // Assign to Loop 0 (Fixed freq)
    scrutiny_handler.process(0);
    scrutiny_handler.process(0);
    EXPECT_FALSE(fixed_freq_loop.owns_datalogger());
    fixed_freq_loop.process();
    EXPECT_TRUE(fixed_freq_loop.owns_datalogger());
    scrutiny_handler.process(0);
    scrutiny_handler.datalogger()->arm_trigger();
    scrutiny_handler.datalogger()->force_trigger();
    EXPECT_FALSE(scrutiny_handler.datalogging_data_available());
    for (uint32_t i = 0; i < sizeof(dlbuffer) / 4; i++)
    {
        fixed_freq_loop.process();
        scrutiny_handler.process(1);
        if (scrutiny_handler.datalogger()->data_acquired())
        {
            break;
        }
    }
    EXPECT_TRUE(scrutiny_handler.datalogger()->data_acquired());
    fixed_freq_loop.process();
    scrutiny_handler.process(1); // Receive the IPC message here
    EXPECT_TRUE(scrutiny_handler.datalogging_data_available());

    // Switch loop.
    test_configure(1, 0, refconfig, protocol::ResponseCode::OK, false); // Assign to Loop 0 (Fixed freq)

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
    EXPECT_FALSE(scrutiny_handler.datalogging_data_available());
    for (uint32_t i = 0; i < sizeof(dlbuffer) / 4; i++)
    {
        variable_freq_loop.process(1); // Process a different loop
        scrutiny_handler.process(1);
        if (scrutiny_handler.datalogger()->data_acquired())
        {
            break;
        }
    }
    EXPECT_TRUE(scrutiny_handler.datalogger()->data_acquired());
    variable_freq_loop.process(1);
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
    test_configure(0, 0, refconfig, protocol::ResponseCode::OK); // Assign to Loop 0 (Fixed freq)
    fixed_freq_loop.process();                                   // Accept ownership
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

void TestDatalogControl::check_get_status(datalogging::DataLogger::State expected_state)
{
    uint8_t tx_buffer[32];
    uint16_t n_to_read;

    uint8_t request_data[8] = {5, 5, 0, 0};
    add_crc(request_data, sizeof(request_data) - 4);
    uint8_t expected_response[9 + 1] = {0x85, 5, 0, 0, 1};
    expected_response[5] = static_cast<uint8_t>(expected_state);

    add_crc(expected_response, sizeof(expected_response) - 4);
    scrutiny_handler.comm()->receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);
    n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    ASSERT_GT(n_to_read, 0);
    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    scrutiny_handler.process(0);
    EXPECT_TRUE( // More verbose than raw data check in case of failure
        IS_PROTOCOL_RESPONSE(
            tx_buffer,
            protocol::CommandId::DataLogControl,
            static_cast<uint8_t>(protocol::DataLogControl::Subfunction::GetStatus),
            protocol::ResponseCode::OK));

    datalogging::DataLogger::State gotten_state = static_cast<datalogging::DataLogger::State>(tx_buffer[5]);
    EXPECT_EQ(gotten_state, expected_state);
    EXPECT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response)); // Last check. Redundant, but it's ok.
}

TEST_F(TestDatalogControl, TestGetStatus)
{

    // Get Status
    check_get_status(datalogging::DataLogger::State::IDLE);

    datalogging::Configuration refconfig = get_valid_reference_configuration();
    test_configure(0, 0, refconfig, protocol::ResponseCode::OK); // Assign to Loop 0 (Fixed freq)
    fixed_freq_loop.process();                                   // Accept ownership
    scrutiny_handler.process(0);

    // From this point, the datalogger is owned by a loop. We need to wait on the loop to broadcast the status through an IPC message.
    fixed_freq_loop.process();   // Send status
    scrutiny_handler.process(0); // Receive status

    check_get_status(datalogging::DataLogger::State::CONFIGURED);
    scrutiny_handler.datalogger()->arm_trigger();
    fixed_freq_loop.process();   // Send status
    scrutiny_handler.process(0); // Receive status
    check_get_status(datalogging::DataLogger::State::ARMED);

    refconfig.items_count = SCRUTINY_DATALOGGING_MAX_SIGNAL + 1;
    test_configure(0, 0, refconfig, protocol::ResponseCode::Overflow, false);
    fixed_freq_loop.process();
    scrutiny_handler.process(0);

    // Empty transmit buffer
    uint8_t dummy_buffer[32];
    scrutiny_handler.comm()->pop_data(dummy_buffer, sizeof(dummy_buffer));
    EXPECT_TRUE(IS_PROTOCOL_RESPONSE(dummy_buffer, protocol::CommandId::DataLogControl, 2, protocol::ResponseCode::Overflow));
    scrutiny_handler.process(0);

    check_get_status(datalogging::DataLogger::State::IDLE); // Gets reset if in error.

    refconfig = get_valid_reference_configuration();
    refconfig.decimation = 1;
    test_configure(0, 0, refconfig, protocol::ResponseCode::OK);
    scrutiny_handler.process(0);
    fixed_freq_loop.process();

    check_get_status(datalogging::DataLogger::State::CONFIGURED);
    scrutiny_handler.datalogger()->arm_trigger();
    fixed_freq_loop.process();   // Send status
    scrutiny_handler.process(0); // Receive status
    check_get_status(datalogging::DataLogger::State::ARMED);

    // We are rmed. We will force a full acquisition and make sure it is correctly reported.
    scrutiny_handler.datalogger()->force_trigger();
    for (uint32_t i = 0; i < sizeof(dlbuffer) / 4; i++)
    {
        fixed_freq_loop.process();
        scrutiny_handler.process(1);
        if (scrutiny_handler.datalogger()->data_acquired())
        {
            break;
        }
    }
    // Acquisition complete. Process for IPC message to be transferred from loop to main
    fixed_freq_loop.process();
    scrutiny_handler.process(1);
    ASSERT_TRUE(scrutiny_handler.datalogger()->data_acquired());
    check_get_status(datalogging::DataLogger::State::ACQUISITION_COMPLETED);
}

TEST_F(TestDatalogControl, TestGetAcquisitionMetadata)
{
    uint8_t tx_buffer[32];
    uint16_t n_to_read;

    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.decimation = 1;
    test_configure(0, 0xabcd, refconfig, protocol::ResponseCode::OK); // Assign to Loop 0 (Fixed freq)
    fixed_freq_loop.process();                                        // Accept ownership
    scrutiny_handler.process(0);

    // Send a request and expect a FailureToProceed because no acquisition is ready.
    uint8_t request_data_before[8] = {5, 6, 0, 0};
    add_crc(request_data_before, sizeof(request_data_before) - 4);

    scrutiny_handler.comm()->receive_data(request_data_before, sizeof(request_data_before));
    scrutiny_handler.process(0);
    n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_GT(n_to_read, 0);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    scrutiny_handler.process(0);
    // Expect a failure because there is no daa available.
    EXPECT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, protocol::CommandId::DataLogControl, 6, protocol::ResponseCode::FailureToProceed));

    // Force an acquisition to happen
    scrutiny_handler.datalogger()->arm_trigger();
    scrutiny_handler.datalogger()->force_trigger();
    EXPECT_FALSE(scrutiny_handler.datalogging_data_available());
    for (uint32_t i = 0; i < sizeof(dlbuffer) / 4; i++)
    {
        fixed_freq_loop.process();
        scrutiny_handler.process(1);
        if (scrutiny_handler.datalogger()->data_acquired())
        {
            break;
        }
    }
    EXPECT_TRUE(scrutiny_handler.datalogger()->data_acquired());
    // Make sure that the loop informs the main handler that data is available.
    fixed_freq_loop.process();
    scrutiny_handler.process(1);

    // Send a 2nd request. Expect a OK response because data is available now.
    uint8_t request_data_after[8] = {5, 6, 0, 0};
    add_crc(request_data_after, sizeof(request_data_after) - 4);

    scrutiny_handler.comm()->receive_data(request_data_after, sizeof(request_data_after));
    scrutiny_handler.process(0);
    n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_GT(n_to_read, 0);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);

    ASSERT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, protocol::CommandId::DataLogControl, 6, protocol::ResponseCode::OK));
    datalogging::DataReader *reader = scrutiny_handler.datalogger()->get_reader();
    reader->reset();

    uint8_t expected_response[9 + 2 + 2 + 4 + 4 + 4] = {0x85, 6, 0, 0, 16};
    uint16_t cursor = 5;
    cursor += codecs::encode_16_bits_big_endian(scrutiny_handler.datalogger()->get_acquisition_id(), &expected_response[cursor]);
    cursor += codecs::encode_16_bits_big_endian(0xabcd, &expected_response[cursor]);
    cursor += codecs::encode_32_bits_big_endian(reader->get_entry_count(), &expected_response[cursor]);
    cursor += codecs::encode_32_bits_big_endian(reader->get_total_size(), &expected_response[cursor]);
    cursor += codecs::encode_32_bits_big_endian(scrutiny_handler.datalogger()->log_points_after_trigger(), &expected_response[cursor]);
    add_crc(expected_response, sizeof(expected_response) - 4);

    EXPECT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

TEST_F(TestDatalogControl, TestReadAcquisitionNoDataAvailable)
{
    uint8_t tx_buffer[32];
    uint16_t n_to_read;

    // Send a request and expect a FailureToProceed because no acquisition is ready.
    uint8_t request_data_before[8] = {5, 7, 0, 0};
    add_crc(request_data_before, sizeof(request_data_before) - 4);

    scrutiny_handler.comm()->receive_data(request_data_before, sizeof(request_data_before));
    scrutiny_handler.process(0);
    n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_GT(n_to_read, 0);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    scrutiny_handler.process(0);
    // Expect a failure because there is no daa available.
    EXPECT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, protocol::CommandId::DataLogControl, 7, protocol::ResponseCode::FailureToProceed));
}

TEST_F(TestDatalogControl, TestReadAcquisitionOneTransfer)
{
    uint8_t tx_buffer[1024];
    uint16_t n_to_read;

    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.decimation = 1;
    test_configure(0, 0xabcd, refconfig, protocol::ResponseCode::OK); // Assign to Loop 0 (Fixed freq)
    fixed_freq_loop.process();                                        // Accept ownership
    scrutiny_handler.process(0);

    // Force an acquisition to happen
    scrutiny_handler.datalogger()->arm_trigger();
    scrutiny_handler.datalogger()->force_trigger();
    EXPECT_FALSE(scrutiny_handler.datalogging_data_available());
    for (uint32_t i = 0; i < sizeof(dlbuffer) / 4; i++)
    {
        fixed_freq_loop.process();
        scrutiny_handler.process(1);
        if (scrutiny_handler.datalogger()->data_acquired())
        {
            break;
        }
    }
    EXPECT_TRUE(scrutiny_handler.datalogger()->data_acquired());
    // Make sure that the loop informs the main handler that data is available.
    fixed_freq_loop.process();
    scrutiny_handler.process(1);

    uint8_t request_data_after[8] = {5, 7, 0, 0};
    add_crc(request_data_after, sizeof(request_data_after) - 4);

    scrutiny_handler.comm()->receive_data(request_data_after, sizeof(request_data_after));
    scrutiny_handler.process(0);
    n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_GT(n_to_read, 0);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);

    ASSERT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, protocol::CommandId::DataLogControl, 7, protocol::ResponseCode::OK));

    datalogging::DataReader *reader = scrutiny_handler.datalogger()->get_reader();
    EXPECT_EQ(tx_buffer[5], 1); // finished
    EXPECT_EQ(tx_buffer[6], 0); // Rolling counter;
    EXPECT_EQ(codecs::decode_16_bits_big_endian(&tx_buffer[7]), scrutiny_handler.datalogger()->get_acquisition_id());
    uint32_t payload_length = codecs::decode_16_bits_big_endian(&tx_buffer[3]);
    EXPECT_TRUE(reader->finished());
    reader->reset();

    uint8_t raw_data[sizeof(dlbuffer)];
    uint32_t data_count = reader->read(raw_data, sizeof(raw_data));
    EXPECT_GT(data_count, static_cast<float>(sizeof(dlbuffer)) * 0.9f);
    ASSERT_EQ(data_count, reader->get_total_size());
    ASSERT_EQ(data_count, payload_length - 8); // header=4. Crc=4

    EXPECT_BUF_EQ(&tx_buffer[9], raw_data, data_count);

    uint32_t expected_crc = tools::crc32(raw_data, data_count);
    uint32_t gotten_crc = codecs::decode_32_bits_big_endian(&tx_buffer[n_to_read - 8]);
    EXPECT_EQ(gotten_crc, expected_crc);
}

TEST_F(TestDatalogControl, TestReadAcquisitionMultipleTransfer)
{
    uint8_t small_tx_buffer[32];
    uint8_t big_dlbuffer[10000];

    config.set_buffers(_rx_buffer, sizeof(_rx_buffer), small_tx_buffer, sizeof(small_tx_buffer));
    config.set_datalogging_buffers(big_dlbuffer, sizeof(big_dlbuffer));
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    datalogging::Configuration refconfig = get_valid_reference_configuration();
    refconfig.decimation = 1;
    test_configure(0, 0xabcd, refconfig, protocol::ResponseCode::OK); // Assign to Loop 0 (Fixed freq)
    fixed_freq_loop.process();                                        // Accept ownership
    scrutiny_handler.process(0);

    // Force an acquisition to happen
    scrutiny_handler.datalogger()->arm_trigger();
    scrutiny_handler.datalogger()->force_trigger();
    EXPECT_FALSE(scrutiny_handler.datalogging_data_available());
    for (uint32_t i = 0; i < sizeof(big_dlbuffer) / 4; i++)
    {
        fixed_freq_loop.process();
        scrutiny_handler.process(1);
        if (scrutiny_handler.datalogger()->data_acquired())
        {
            break;
        }
    }
    EXPECT_TRUE(scrutiny_handler.datalogger()->data_acquired());
    // Make sure that the loop informs the main handler that data is available.
    fixed_freq_loop.process();
    scrutiny_handler.process(1);
    constexpr uint16_t transfer_overhead = 9 + 4; // protocol overhead =9.  Payload overhead = 4
    constexpr uint16_t crc_size = 4;
    ASSERT_GT(sizeof(small_tx_buffer), transfer_overhead);
    constexpr uint16_t maximum_transfer_count = (sizeof(big_dlbuffer) + crc_size) / (sizeof(small_tx_buffer) - transfer_overhead);
    uint8_t read_data[sizeof(big_dlbuffer)];
    uint8_t reference_data[sizeof(big_dlbuffer)];
    uint16_t expected_acquisition_id = scrutiny_handler.datalogger()->get_acquisition_id();
    for (uint8_t iteration = 0; iteration < 3; iteration++)
    {
        uint32_t read_cursor = 0;
        // We can loop as many time as we want.  The datalogger keeps the data for as long as there is no command to change its state (reconfigure or rearm)
        datalogging::DataReader *reader = scrutiny_handler.datalogger()->get_reader();
        reader->reset();
        const uint32_t total_data_length = reader->read(reference_data, sizeof(reference_data));
        ASSERT_GT(total_data_length, 0.95f * sizeof(big_dlbuffer));
        uint32_t expected_crc = tools::crc32(reference_data, total_data_length);
        ASSERT_TRUE(reader->finished()) << "iteration=" << iteration;

        bool finished = false;
        for (uint16_t i = 0; i < maximum_transfer_count && !finished; i++)
        {
            std::string error_msg = std::string("iteration=") + std::to_string(iteration) + std::string(", i=") + std::to_string(i);
            uint8_t validation_txbuffer[128];

            uint8_t request_data_after[8] = {5, 7, 0, 0};
            add_crc(request_data_after, sizeof(request_data_after) - 4);

            scrutiny_handler.comm()->receive_data(request_data_after, sizeof(request_data_after));
            scrutiny_handler.process(0);
            uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
            ASSERT_GT(n_to_read, 0) << error_msg;
            ASSERT_LT(n_to_read, sizeof(validation_txbuffer)) << error_msg;
            scrutiny_handler.comm()->pop_data(validation_txbuffer, n_to_read);
            scrutiny_handler.process(0);

            ASSERT_TRUE(IS_PROTOCOL_RESPONSE(validation_txbuffer, protocol::CommandId::DataLogControl, 7, protocol::ResponseCode::OK)) << error_msg;
            finished = static_cast<bool>(validation_txbuffer[5]);
            EXPECT_EQ(validation_txbuffer[6], i % 0x100) << error_msg; // Rolling counter;
            EXPECT_EQ(codecs::decode_16_bits_big_endian(&validation_txbuffer[7]), expected_acquisition_id) << error_msg;

            uint16_t payload_length = codecs::decode_16_bits_big_endian(&validation_txbuffer[3]);
            ASSERT_GE(payload_length, 8);

            uint16_t qty_to_read;
            if (finished)
            {
                qty_to_read = payload_length - 4 - 4; // Last block has CRC;
                uint32_t read_crc = codecs::decode_32_bits_big_endian(&validation_txbuffer[9 + qty_to_read]);
                EXPECT_EQ(read_crc, expected_crc) << error_msg;
            }
            else
            {
                qty_to_read = payload_length - 4;
            }
            std::memcpy(&read_data[read_cursor], &validation_txbuffer[9], qty_to_read);
            read_cursor += qty_to_read;
        }
        EXPECT_TRUE(finished);

        EXPECT_BUF_EQ(read_data, reference_data, total_data_length) << "iteration=" << iteration;
    }
}

#endif
