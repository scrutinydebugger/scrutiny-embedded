//    test_protocol_tx_parsing.cpp
//        Make sure Response objects are correctly encoded in bytes
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "scrutinytest/scrutinytest.hpp"

#include "scrutiny.hpp"
#include "scrutiny_base_test.hpp"

class TestTxParsing : public ScrutinyBaseTest
{
  public:
    TestTxParsing() :
        ScrutinyBaseTest(),
        comm(),
        response_buffer(),
        response(),
        _rx_buffer(),
        _tx_buffer()
    {
    }

  protected:
    scrutiny::Timebase tb;
    scrutiny::protocol::CommHandler comm;
    uint8_t response_buffer[256];
    scrutiny::protocol::Response response;

    uint8_t _rx_buffer[128];
    uint8_t _tx_buffer[128];

    virtual void SetUp()
    {
        comm.init(_rx_buffer, sizeof(_rx_buffer), _tx_buffer, sizeof(_tx_buffer), &tb);
        response.data = response_buffer;
        comm.connect();
    }
};

TEST_F(TestTxParsing, TestReadAllData)
{
    uint8_t buf[256];

    response.command_id = 0x81;
    response.subfunction_id = 0x02;
    response.response_code = 0x03;
    response.data_length = 3;
    response.data[0] = 0x11;
    response.data[1] = 0x22;
    response.data[2] = 0x33;
    add_crc(&response);

    comm.send_response(&response);

    uint8_t expected_data[12] = { 0x81, 2, 3, 0, 3, 0x11, 0x22, 0x33 };
    add_crc(expected_data, 8);

    uint16_t n_to_read = comm.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(buf));
    EXPECT_EQ(n_to_read, sizeof(expected_data));

    uint16_t nread = comm.pop_data(buf, n_to_read);
    EXPECT_EQ(nread, n_to_read);

    ASSERT_BUF_EQ(buf, expected_data, sizeof(expected_data));
}

TEST_F(TestTxParsing, TestReadBytePerByte)
{
    uint8_t buf[256];

    response.command_id = 0x81;
    response.subfunction_id = 0x02;
    response.response_code = 0x03;
    response.data_length = 3;
    response.data[0] = 0x11;
    response.data[1] = 0x22;
    response.data[2] = 0x33;
    add_crc(&response);

    comm.send_response(&response);

    uint8_t expected_data[12] = { 0x81, 2, 3, 0, 3, 0x11, 0x22, 0x33 };
    add_crc(expected_data, 8);

    uint16_t n_to_read = comm.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(buf));
    EXPECT_EQ(n_to_read, sizeof(expected_data));

    uint16_t nread;
    for (uint16_t i = 0; i < n_to_read; i++)
    {
        nread = comm.pop_data(&buf[i], 1);
        EXPECT_EQ(nread, 1u);
    }

    ASSERT_BUF_EQ(buf, expected_data, sizeof(expected_data));
}

TEST_F(TestTxParsing, TestReadByChunk)
{
    uint8_t buf[256];

    response.command_id = 0x81;
    response.subfunction_id = 0x02;
    response.response_code = 0x03;
    response.data_length = 3;
    response.data[0] = 0x11;
    response.data[1] = 0x22;
    response.data[2] = 0x33;
    add_crc(&response);

    comm.send_response(&response);

    uint8_t expected_data[12] = { 0x81, 2, 3, 0, 3, 0x11, 0x22, 0x33 };
    add_crc(expected_data, 8);

    uint16_t n_to_read = comm.data_to_send();
    uint8_t chunks[3] = { 3, 6, 3 };
    ASSERT_EQ(n_to_read, 12u);

    uint16_t nread;
    uint8_t index = 0;
    for (uint32_t i = 0; i < sizeof(chunks); i++)
    {
        nread = comm.pop_data(&buf[index], chunks[i]);
        EXPECT_EQ(nread, static_cast<uint32_t>(chunks[i]));
        index += chunks[i];
    }

    ASSERT_BUF_EQ(buf, expected_data, sizeof(expected_data));
}

TEST_F(TestTxParsing, TestReadMoreThanAvailable)
{
    uint8_t buf[256];

    response.command_id = 0x81;
    response.subfunction_id = 0x02;
    response.response_code = 0x03;
    response.data_length = 3;
    response.data[0] = 0x11;
    response.data[1] = 0x22;
    response.data[2] = 0x33;
    add_crc(&response);

    comm.send_response(&response);

    uint8_t expected_data[12] = { 0x81, 2, 3, 0, 3, 0x11, 0x22, 0x33 };
    add_crc(expected_data, 8);

    uint16_t n_to_read = comm.data_to_send();
    uint16_t nread = comm.pop_data(buf, n_to_read + 10);
    EXPECT_EQ(nread, n_to_read);

    ASSERT_BUF_EQ(buf, expected_data, sizeof(expected_data));
}

TEST_F(TestTxParsing, TestSendsOverflow)
{
    SCRUTINY_STATIC_ASSERT(sizeof(_tx_buffer) < 0xFFFE, "buffer too big");

    SCRUTINY_CONSTEXPR uint16_t datalen = sizeof(_tx_buffer) + 1;
    response.command_id = 0x81;
    response.subfunction_id = 0x02;
    response.response_code = 0x03;

    response.data_length = datalen;
    add_crc(&response);

    comm.send_response(&response);

    uint16_t n_to_read = comm.data_to_send();
    EXPECT_EQ(n_to_read, 0);

    EXPECT_EQ(comm.get_tx_error(), scrutiny::protocol::TxError::Overflow);
}

TEST_F(TestTxParsing, TestSendsFullBuffer)
{
    uint8_t buf[sizeof(_tx_buffer) + scrutiny::protocol::RESPONSE_OVERHEAD];
    SCRUTINY_STATIC_ASSERT(sizeof(_tx_buffer) < 0xFFFF, "buffer too big");

    SCRUTINY_CONSTEXPR uint16_t datalen = sizeof(_tx_buffer);
    response.command_id = 0x81;
    response.subfunction_id = 0x02;
    response.response_code = 0x03;
    response.data_length = datalen;
    uint16_t i = 0;
    uint8_t n = 0;
    for (n = 0, i = 0; i < datalen; i++, n++)
    {
        response.data[i] = n;
    }

    add_crc(&response);

    comm.send_response(&response);
    EXPECT_EQ(comm.get_tx_error(), scrutiny::protocol::TxError::None);

    uint16_t n_to_read = comm.data_to_send();
    ASSERT_EQ(n_to_read, datalen + scrutiny::protocol::RESPONSE_OVERHEAD);

    uint16_t nread = comm.pop_data(buf, n_to_read);
    EXPECT_EQ(nread, n_to_read);

    ASSERT_BUF_EQ(&buf[5], response.data, datalen);
}

TEST_F(TestTxParsing, TestSendsOverflowRestart)
{
    SCRUTINY_STATIC_ASSERT(sizeof(_tx_buffer) < 0xFFFE, "buffer too big");
    SCRUTINY_CONSTEXPR uint16_t datalen = sizeof(_tx_buffer);
    response.command_id = 0x81;
    response.subfunction_id = 0x02;
    response.response_code = 0x03;

    response.data_length = datalen + 1;
    add_crc(&response);

    comm.send_response(&response);
    EXPECT_EQ(comm.data_to_send(), 0);
    EXPECT_EQ(comm.get_tx_error(), scrutiny::protocol::TxError::Overflow);

    response.data_length = datalen;
    comm.send_response(&response);
    EXPECT_EQ(comm.get_tx_error(), scrutiny::protocol::TxError::None);
    EXPECT_EQ(comm.data_to_send(), datalen + scrutiny::protocol::RESPONSE_OVERHEAD);
}
