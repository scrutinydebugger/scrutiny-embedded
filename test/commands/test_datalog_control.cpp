//    test_user_command.cpp
//        Test the behaviour of the embedded module when UserCommand commands are received
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include <gtest/gtest.h>
#include <cstring>

#include "scrutiny.hpp"
#include "scrutiny_test.hpp"

class TestDatalogControl : public ScrutinyTest
{
protected:
    scrutiny::Timebase tb;
    scrutiny::MainHandler scrutiny_handler;
    scrutiny::Config config;

    uint8_t _rx_buffer[128];
    uint8_t _tx_buffer[128];
    uint8_t dlbuffer[256];

    virtual void SetUp()
    {
        config.set_buffers(_rx_buffer, sizeof(_rx_buffer), _tx_buffer, sizeof(_tx_buffer));
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
    const scrutiny::protocol::CommandId cmd = scrutiny::protocol::CommandId::DataLogControl;
    const scrutiny::protocol::ResponseCode failure = scrutiny::protocol::ResponseCode::UnsupportedFeature;

    uint8_t tx_buffer[32];

    for (uint16_t i = 0; i <= 255; i++)
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

TEST_F(TestDatalogControl, TestGetBufferSize)
{
    uint8_t tx_buffer[32];
    uint32_t buffer_size = sizeof(dlbuffer);

    uint8_t request_data[8] = {5, 2, 0, 0};
    add_crc(request_data, sizeof(request_data) - 4);

    // Make expected response
    uint8_t expected_response[9 + 4] = {0x81, 3, 0, 0, 1};
    expected_response[5] = static_cast<uint8_t>((buffer_size >> 24) & 0xFF);
    expected_response[6] = static_cast<uint8_t>((buffer_size >> 16) & 0xFF);
    expected_response[7] = static_cast<uint8_t>((buffer_size >> 8) & 0xFF);
    expected_response[8] = static_cast<uint8_t>((buffer_size >> 0) & 0xFF);
    add_crc(request_data, sizeof(expected_response) - 4);

    scrutiny_handler.comm()->receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    ASSERT_GT(n_to_read, 0);

    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
}

#endif
