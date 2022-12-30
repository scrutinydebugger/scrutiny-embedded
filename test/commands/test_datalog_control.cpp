#include <gtest/gtest.h>
#include <cstring>

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

    uint8_t _rx_buffer[128];
    uint8_t _tx_buffer[128];
    uint8_t dlbuffer[256];
    LoopHandler *loops[2];

    FixedFrequencyLoopHandler fixed_freq_loop;
    VariableFrequencyLoopHandler variable_freq_loop;

    RuntimePublishedValue rpvs[1] = {
        {0x8888, VariableType::float32}};

    TestDatalogControl() : ScrutinyTest(), fixed_freq_loop(FIXED_FREQ_LOOP_TIMESTEP_US) {}

    uint16_t encode_datalogger_config(loop_id_t loop_id, const datalogging::Configuration *dlconfig, uint8_t *buffer);

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

uint16_t TestDatalogControl::encode_datalogger_config(loop_id_t loop_id, const datalogging::Configuration *dlconfig, uint8_t *buffer)
{
    uint32_t cursor = 0;

    cursor += codecs::encode_8_bits(static_cast<uint8_t>(loop_id), &buffer[cursor]);
    cursor += codecs::encode_16_bits_big_endian(dlconfig->decimation, &buffer[cursor]);
    cursor += codecs::encode_8_bits(dlconfig->probe_location, &buffer[cursor]);
    cursor += codecs::encode_32_bits_big_endian(dlconfig->timeout_us, &buffer[cursor]);
    cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->trigger.condition), &buffer[cursor]);
    cursor += codecs::encode_32_bits_big_endian(dlconfig->trigger.hold_time_us, &buffer[cursor]);
    cursor += codecs::encode_8_bits(dlconfig->trigger.operand_count, &buffer[cursor]);

    for (uint32_t i = 0; i < dlconfig->trigger.operand_count; i++)
    {
        cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->trigger.operands[i].type), &buffer[cursor]);
        switch (dlconfig->trigger.operands[i].type)
        {
        case datalogging::OperandType::LITERAL:
            codecs::encode_float_big_endian(dlconfig->trigger.operands[i].data.literal.val, &buffer[cursor]);
            cursor += sizeof(float);
            break;
        case datalogging::OperandType::RPV:
            codecs::encode_16_bits_big_endian(dlconfig->trigger.operands[i].data.rpv.id, &buffer[cursor]);
            cursor += sizeof(uint16_t);
            break;
        case datalogging::OperandType::VAR:
            cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->trigger.operands[i].data.var.datatype), &buffer[cursor]);
            cursor += codecs::encode_address_big_endian(dlconfig->trigger.operands[i].data.var.addr, &buffer[cursor]);
            break;

        case datalogging::OperandType::VARBIT:
            cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->trigger.operands[i].data.varbit.datatype), &buffer[cursor]);
            cursor += codecs::encode_address_big_endian(dlconfig->trigger.operands[i].data.varbit.addr, &buffer[cursor]);
            cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->trigger.operands[i].data.varbit.bitoffset), &buffer[cursor]);
            cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->trigger.operands[i].data.varbit.bitsize), &buffer[cursor]);
            break;
        }
    }

    cursor += codecs::encode_8_bits(dlconfig->items_count, &buffer[cursor]);
    for (uint32_t i = 0; i < dlconfig->items_count; i++)
    {
        cursor += codecs::encode_8_bits(static_cast<uint8_t>(dlconfig->items_to_log[i].type), &buffer[cursor]);
        switch (dlconfig->items_to_log[i].type)
        {
        case datalogging::LoggableType::TIME:
            break;
        case datalogging::LoggableType::MEMORY:
            cursor += codecs::encode_address_big_endian(dlconfig->items_to_log[i].data.memory.address, &buffer[cursor]);
            cursor += codecs::encode_8_bits(dlconfig->items_to_log[i].data.memory.size, &buffer[cursor]);
            break;
        case datalogging::LoggableType::RPV:
            cursor += codecs::encode_16_bits_big_endian(dlconfig->items_to_log[i].data.rpv.id, &buffer[cursor]);
            break;
        }
    }

    return cursor;
}

TEST_F(TestDatalogControl, TestGetBufferSize)
{
    uint8_t tx_buffer[32];
    uint32_t buffer_size = sizeof(dlbuffer);

    uint8_t request_data[8] = {5, 2, 0, 0};
    add_crc(request_data, sizeof(request_data) - 4);

    // Make expected response
    uint8_t expected_response[9 + 4] = {0x85, 2, 0, 0, 4};
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
    float some_var_operand = 0;
    float some_var_logged = 0;

    constexpr loop_id_t loop_id = 1;
    datalogging::Configuration refconfig;

    refconfig.decimation = 0x1234;
    refconfig.probe_location = 123;
    refconfig.timeout_us = 0x11223344;
    refconfig.items_count = 3;

    refconfig.items_to_log[0].type = datalogging::LoggableType::TIME;
    refconfig.items_to_log[1].type = datalogging::LoggableType::MEMORY;
    refconfig.items_to_log[1].data.memory.address = &some_var_logged;
    refconfig.items_to_log[1].data.memory.size = sizeof(some_var_logged);
    refconfig.items_to_log[2].type = datalogging::LoggableType::RPV;
    refconfig.items_to_log[2].data.rpv.id = 0x8888;

    refconfig.trigger.condition = datalogging::SupportedTriggerConditions::Equal;
    refconfig.trigger.hold_time_us = 0xaabbccdd;
    refconfig.trigger.operand_count = 2;
    refconfig.trigger.operands[0].type = datalogging::OperandType::LITERAL;
    refconfig.trigger.operands[0].data.literal.val = 3.1415926f;
    refconfig.trigger.operands[1].type = datalogging::OperandType::VAR;
    refconfig.trigger.operands[1].data.var.addr = &some_var_operand;
    refconfig.trigger.operands[1].data.var.datatype = VariableType::float32;

    uint8_t request_data[128] = {5, 4};
    uint16_t payload_size = encode_datalogger_config(loop_id, &refconfig, &request_data[4]);
    request_data[2] = (payload_size >> 8) & 0xFF;
    request_data[3] = payload_size & 0xFF;

    add_crc(request_data, 4 + payload_size);

    // Make expected response
    uint8_t expected_response[9] = {0x85, 4, 0, 0, 0};
    add_crc(expected_response, sizeof(expected_response) - 4);

    scrutiny_handler.comm()->receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint8_t tx_buffer[32];
    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    ASSERT_GT(n_to_read, 0);

    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    EXPECT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, protocol::CommandId::DataLogControl, 4, protocol::ResponseCode::OK));

    EXPECT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));

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
}

#endif
