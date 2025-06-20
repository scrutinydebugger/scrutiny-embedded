//    test_comm_control.cpp
//        Test the behaviour of the embedded module when CommControl commands are received
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include <gtest/gtest.h>
#include <cstring>

#include "scrutiny.hpp"
#include "scrutiny_test.hpp"

#define DISPLAY_NAME "helloworld"
#define DISPLAY_NAME_LENGTH 10

class TestCommControl : public ScrutinyTest
{
protected:
    scrutiny::Timebase tb;
    scrutiny::MainHandler scrutiny_handler;
    scrutiny::Config config;

    uint8_t _rx_buffer[128];
    uint8_t _tx_buffer[128];

    TestCommControl() : ScrutinyTest(),
                        tb(),
                        scrutiny_handler(),
                        config(),
                        _rx_buffer(),
                        _tx_buffer()
    {
    }

    virtual void SetUp()
    {
        config.set_buffers(_rx_buffer, sizeof(_rx_buffer), _tx_buffer, sizeof(_tx_buffer));
        config.max_bitrate = 0x12345678;
        config.display_name = DISPLAY_NAME;

        scrutiny_handler.init(&config);
    }
};

TEST_F(TestCommControl, TestDiscover)
{

    ASSERT_FALSE(scrutiny_handler.comm()->is_connected()); // We should get a Discover response even when not connected.
    ASSERT_EQ(sizeof(scrutiny::protocol::CommControl::DISCOVER_MAGIC), 4u);
    uint8_t request_data[8 + 4] = {2, 1, 0, 4};
    std::memcpy(&request_data[4], scrutiny::protocol::CommControl::DISCOVER_MAGIC, sizeof(scrutiny::protocol::CommControl::DISCOVER_MAGIC));
    std::string display_name = std::string(DISPLAY_NAME);

    uint8_t tx_buffer[64];
    // proto_maj, proto_min, magic, name_len, name
    uint8_t expected_response[9 + 2 + sizeof(scrutiny::software_id) + 1 + DISPLAY_NAME_LENGTH] = {0x82, 1, 0, 0, 2 + sizeof(scrutiny::software_id) + 1 + DISPLAY_NAME_LENGTH}; // Version 1.0

    uint16_t index = 5;
    expected_response[index++] = 1;
    expected_response[index++] = 0;
    std::memcpy(&expected_response[index], scrutiny::software_id, sizeof(scrutiny::software_id));
    index += sizeof(scrutiny::software_id);
    expected_response[index++] = static_cast<uint8_t>(display_name.length());
    std::memcpy(&expected_response[index], display_name.c_str(), display_name.length());

    add_crc(request_data, sizeof(request_data) - 4);
    add_crc(expected_response, sizeof(expected_response) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);
    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);

    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

TEST_F(TestCommControl, TestDiscoverWrongMagic)
{
    ASSERT_EQ(sizeof(scrutiny::protocol::CommControl::DISCOVER_MAGIC), 4u);
    uint8_t request_data[8 + 4] = {2, 1, 0, 4};
    std::memcpy(&request_data[4], scrutiny::protocol::CommControl::DISCOVER_MAGIC, sizeof(scrutiny::protocol::CommControl::DISCOVER_MAGIC));
    request_data[4] = ~request_data[4];
    add_crc(request_data, sizeof(request_data) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_EQ(n_to_read, 0u); // No data because we are not connected and discover is invalid. We stay silent
}

TEST_F(TestCommControl, TestDiscoverWrongMagicWhileConnected)
{
    const scrutiny::protocol::CommandId::E cmd = scrutiny::protocol::CommandId::CommControl;
    uint8_t const subfn = static_cast<uint8_t>(scrutiny::protocol::CommControl::Subfunction::Discover);
    const scrutiny::protocol::ResponseCode::E code = scrutiny::protocol::ResponseCode::InvalidRequest;
    ASSERT_EQ(sizeof(scrutiny::protocol::CommControl::DISCOVER_MAGIC), 4u);

    scrutiny_handler.comm()->connect();
    uint8_t request_data[8 + 4] = {2, 1, 0, 4};
    std::memcpy(&request_data[4], scrutiny::protocol::CommControl::DISCOVER_MAGIC, sizeof(scrutiny::protocol::CommControl::DISCOVER_MAGIC));
    request_data[4] = ~request_data[4];
    add_crc(request_data, sizeof(request_data) - 4);

    uint8_t tx_buffer[32];

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    scrutiny_handler.pop_data(tx_buffer, n_to_read);
    // Now we expect an InvalidRequest response
    ASSERT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, code));
    scrutiny_handler.process(0);
}

TEST_F(TestCommControl, TestHeartbeat)
{
    uint8_t request_data[8 + 4 + 2] = {2, 2, 0, 6};

    uint8_t tx_buffer[32];
    uint8_t expected_response[9 + 4 + 2] = {0x82, 2, 0, 0, 6};

    scrutiny_handler.comm()->connect();
    uint32_t session_id = scrutiny_handler.comm()->get_session_id();
    request_data[4] = (session_id >> 24) & 0xFF;
    request_data[5] = (session_id >> 16) & 0xFF;
    request_data[6] = (session_id >> 8) & 0xFF;
    request_data[7] = (session_id >> 0) & 0xFF;

    expected_response[5] = (session_id >> 24) & 0xFF;
    expected_response[6] = (session_id >> 16) & 0xFF;
    expected_response[7] = (session_id >> 8) & 0xFF;
    expected_response[8] = (session_id >> 0) & 0xFF;

    // So we expect the comm to stay enabled after multiple call to heartbeat even if time goes by
    for (uint16_t challenge = 0; challenge < 4; challenge++)
    {
        request_data[8] = ((challenge >> 8) & 0xFF);
        request_data[9] = (challenge & 0xFF);
        expected_response[9] = ~request_data[8];
        expected_response[10] = ~request_data[9];
        ASSERT_TRUE(scrutiny_handler.comm()->is_connected()) << "challenge=" << static_cast<uint32_t>(challenge);

        add_crc(request_data, sizeof(request_data) - 4);
        add_crc(expected_response, sizeof(expected_response) - 4);
        scrutiny_handler.receive_data(request_data, sizeof(request_data));
        scrutiny_handler.process(SCRUTINY_COMM_HEARTBEAT_TIMEOUT_US / 2);

        uint16_t n_to_read = scrutiny_handler.data_to_send();
        ASSERT_EQ(n_to_read, sizeof(expected_response)) << "challenge=" << static_cast<uint32_t>(challenge);
        uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
        EXPECT_EQ(nread, n_to_read) << "challenge=" << static_cast<uint32_t>(challenge);

        ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response)) << "challenge=" << static_cast<uint32_t>(challenge);
        ASSERT_TRUE(scrutiny_handler.comm()->is_connected()) << "challenge=" << static_cast<uint32_t>(challenge);
        scrutiny_handler.process(0);
    }
}

TEST_F(TestCommControl, TestGetParams)
{
    uint8_t tx_buffer[32];
    uint8_t request_data[8] = {2, 3, 0, 0};
    SCRUTINY_CONSTEXPR uint8_t address_size = sizeof(uintptr_t);
    add_crc(request_data, sizeof(request_data) - 4);
    SCRUTINY_CONSTEXPR uint16_t datalen = 2 + 2 + 4 + 4 + 4 + 1;

    uint8_t expected_response[9 + datalen] = {0x82, 3, 0, 0, datalen};
    uint8_t i = 5;
    expected_response[i++] = (sizeof(_rx_buffer) >> 8) & 0xFF;
    expected_response[i++] = (sizeof(_rx_buffer)) & 0xFF;
    expected_response[i++] = (sizeof(_rx_buffer) >> 8) & 0xFF;
    expected_response[i++] = (sizeof(_rx_buffer)) & 0xFF;
    expected_response[i++] = (config.max_bitrate >> 24) & 0xFF;
    expected_response[i++] = (config.max_bitrate >> 16) & 0xFF;
    expected_response[i++] = (config.max_bitrate >> 8) & 0xFF;
    expected_response[i++] = (config.max_bitrate >> 0) & 0xFF;
    expected_response[i++] = (SCRUTINY_COMM_HEARTBEAT_TIMEOUT_US >> 24) & 0xFF;
    expected_response[i++] = (SCRUTINY_COMM_HEARTBEAT_TIMEOUT_US >> 16) & 0xFF;
    expected_response[i++] = (SCRUTINY_COMM_HEARTBEAT_TIMEOUT_US >> 8) & 0xFF;
    expected_response[i++] = (SCRUTINY_COMM_HEARTBEAT_TIMEOUT_US >> 0) & 0xFF;
    expected_response[i++] = (SCRUTINY_COMM_RX_TIMEOUT_US >> 24) & 0xFF;
    expected_response[i++] = (SCRUTINY_COMM_RX_TIMEOUT_US >> 16) & 0xFF;
    expected_response[i++] = (SCRUTINY_COMM_RX_TIMEOUT_US >> 8) & 0xFF;
    expected_response[i++] = (SCRUTINY_COMM_RX_TIMEOUT_US >> 0) & 0xFF;
    expected_response[i++] = address_size;
    add_crc(expected_response, sizeof(expected_response) - 4);

    scrutiny_handler.comm()->connect();
    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);
    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);

    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

TEST_F(TestCommControl, TestConnect)
{
    ASSERT_EQ(sizeof(scrutiny::protocol::CommControl::CONNECT_MAGIC), 4u);
    uint8_t request_data[8 + 4] = {2, 4, 0, 4};
    std::memcpy(&request_data[4], scrutiny::protocol::CommControl::CONNECT_MAGIC, sizeof(scrutiny::protocol::CommControl::CONNECT_MAGIC));

    uint8_t tx_buffer[32];
    uint8_t expected_response[9 + 4 + 4] = {0x82, 4, 0, 0, 8}; // Version 1.0
    std::memcpy(&expected_response[5], scrutiny::protocol::CommControl::CONNECT_MAGIC, sizeof(scrutiny::protocol::CommControl::CONNECT_MAGIC));

    add_crc(request_data, sizeof(request_data) - 4);

    ASSERT_FALSE(scrutiny_handler.comm()->is_connected());
    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);

    uint32_t session_id = scrutiny_handler.comm()->get_session_id();

    expected_response[9] = (session_id >> 24) & 0xFF;
    expected_response[10] = (session_id >> 16) & 0xFF;
    expected_response[11] = (session_id >> 8) & 0xFF;
    expected_response[12] = (session_id >> 0) & 0xFF;
    add_crc(expected_response, sizeof(expected_response) - 4);

    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
    ASSERT_TRUE(scrutiny_handler.comm()->is_connected());
}

TEST_F(TestCommControl, TestDisconnect)
{
    scrutiny_handler.comm()->connect();
    uint32_t session_id = scrutiny_handler.comm()->get_session_id();
    uint8_t request_data[8 + 4] = {2, 5, 0, 4};
    request_data[4] = (session_id >> 24) & 0xFF;
    request_data[5] = (session_id >> 16) & 0xFF;
    request_data[6] = (session_id >> 8) & 0xFF;
    request_data[7] = (session_id >> 0) & 0xFF;
    add_crc(request_data, sizeof(request_data) - 4);

    uint8_t tx_buffer[32];
    uint8_t expected_response[9] = {0x82, 5, 0, 0, 0}; // Version 1.0
    add_crc(expected_response, sizeof(expected_response) - 4);

    ASSERT_TRUE(scrutiny_handler.comm()->is_connected());
    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);
    scrutiny_handler.process(0); // We need a subsequent call to process because disconnection hapens once the response is completely sent.

    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
    ASSERT_FALSE(scrutiny_handler.comm()->is_connected());
}