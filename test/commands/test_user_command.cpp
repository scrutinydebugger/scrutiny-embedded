//    test_user_command.cpp
//        Test the behaviour of the embedded module when UserCommand commands are received
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "scrutinytest/scrutinytest.hpp"
#include <cstring>

#include "scrutiny.hpp"
#include "scrutiny_test.hpp"

#define TX_BUFFER_SIZE 128

struct CallbackData
{

    uint16_t subfunction;
    unsigned char request_data[16];
    uint16_t request_data_length;
    uint16_t response_max_data_length;
};

class TestUserCommand : public ScrutinyBaseTest
{
  protected:
    scrutiny::Timebase tb;
    scrutiny::MainHandler scrutiny_handler;
    scrutiny::Config config;

    uint8_t _rx_buffer[128];
    uint8_t _tx_buffer[TX_BUFFER_SIZE];

    TestUserCommand() :
        ScrutinyBaseTest(),
        tb(),
        scrutiny_handler(),
        config(),
        _rx_buffer(),
        _tx_buffer()
    {
    }

    virtual void SetUp() { config.set_buffers(_rx_buffer, sizeof(_rx_buffer), _tx_buffer, sizeof(_tx_buffer)); }
};

static CallbackData callback1_data;
void my_callback1(
    uint_least8_t const subfunction,
    unsigned char const *request_data,
    uint16_t const request_data_length,
    unsigned char *response_data,
    uint16_t *response_data_length,
    uint16_t const response_max_data_length)
{

    callback1_data.subfunction = subfunction;
    callback1_data.request_data_length = request_data_length;
    if (request_data_length <= sizeof(callback1_data.request_data))
    {
        std::memcpy(callback1_data.request_data, request_data, request_data_length);
    }
    callback1_data.response_max_data_length = response_max_data_length;

    response_data[0] = 0x11;
    response_data[1] = 0x22;
    response_data[2] = 0x33;
    response_data[3] = 0x44;

    *response_data_length = 4;
}

void my_callback2(
    uint8_t const subfunction,
    uint8_t const *request_data,
    uint16_t const request_data_length,
    uint8_t *response_data,
    uint16_t *response_data_length,
    uint16_t const response_max_data_length)
{
    (void)subfunction;         // Silence unused parameters warning
    (void)request_data;        // Silence unused parameters warning
    (void)request_data_length; // Silence unused parameters warning
    (void)response_data;       // Silence unused parameters warning
    *response_data_length = response_max_data_length + 1;
}

TEST_F(TestUserCommand, TestCommandCalled)
{
    uint8_t tx_buffer[32];
    config.set_user_command_callback(my_callback1);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    uint8_t request_data[8 + 3] = { 4, 0xAA, 0, 3, 0x12, 0x34, 0x56 };
    add_crc(request_data, sizeof(request_data) - 4);

    uint8_t expected_response[9 + 4] = { 0x84, 0xAA, 0, 0, 4, 0x11, 0x22, 0x33, 0x44 };
    add_crc(expected_response, sizeof(expected_response) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    EXPECT_EQ(callback1_data.subfunction, 0xAA);
    EXPECT_EQ(callback1_data.request_data_length, 0x3);
    EXPECT_EQ(callback1_data.request_data[0], 0x12);
    EXPECT_EQ(callback1_data.request_data[1], 0x34);
    EXPECT_EQ(callback1_data.request_data[2], 0x56);

    EXPECT_EQ(callback1_data.response_max_data_length, TX_BUFFER_SIZE);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_EQ(n_to_read, sizeof(expected_response));

    scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

TEST_F(TestUserCommand, TestResponseOverflow)
{
    const scrutiny::protocol::CommandId::E cmd = scrutiny::protocol::CommandId::UserCommand;
    const scrutiny::protocol::ResponseCode::E code = scrutiny::protocol::ResponseCode::Overflow;

    uint8_t tx_buffer[32];
    config.set_user_command_callback(my_callback2);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    uint8_t request_data[8] = { 4, 0, 0, 0 };
    add_crc(request_data, sizeof(request_data) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();

    scrutiny_handler.pop_data(tx_buffer, n_to_read);
    ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, 0, code);
}

TEST_F(TestUserCommand, TestNoCallback)
{
    const scrutiny::protocol::CommandId::E cmd = scrutiny::protocol::CommandId::UserCommand;
    const scrutiny::protocol::ResponseCode::E code = scrutiny::protocol::ResponseCode::UnsupportedFeature;

    uint8_t tx_buffer[32];
    // No callback set on purpose
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    uint8_t request_data[8] = { 4, 0, 0, 0 };
    add_crc(request_data, sizeof(request_data) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();

    scrutiny_handler.pop_data(tx_buffer, n_to_read);
    ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, 0, code);
}
