//    test_protocol_rx_parsing.cpp
//        Make sure reception of request bytes are correctly decoded
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "scrutinytest/scrutinytest.hpp"

#include "scrutiny.hpp"
#include "scrutiny_test.hpp"

class TestRxParsing : public ScrutinyTest
{
  protected:
    scrutiny::Timebase tb;
    scrutiny::protocol::CommHandler comm;

    unsigned char _rx_buffer[128];
    unsigned char _tx_buffer[128];

    virtual void SetUp()
    {
        comm.init(_rx_buffer, sizeof(_rx_buffer), _tx_buffer, sizeof(_tx_buffer), &tb);
        comm.connect();
    }
};

//=============================================================================
TEST_F(TestRxParsing, TestRx_ZeroLen_AllInOne)
{
    unsigned char data[8] = { 1, 2, 0, 0 };
    add_crc(data, 4);
    comm.receive_data(data, sizeof(data));

    ASSERT_TRUE(comm.request_received());
    scrutiny::protocol::Request const *req = comm.get_request();
    EXPECT_EQ(req->command_id, 1);
    EXPECT_EQ(req->subfunction_id, 2);
    EXPECT_EQ(req->data_length, 0);

    EXPECT_EQ(comm.get_rx_error(), scrutiny::protocol::RxError::None);
}

//=============================================================================
TEST_F(TestRxParsing, TestRx_ZeroLen_BytePerByte)
{
    unsigned char data[8] = { 1, 2, 0, 0 };
    add_crc(data, 4);

    for (unsigned int i = 0; i < sizeof(data); i++)
    {
        comm.receive_data(&data[i], 1);
        data[i] ^= 0xFF; // destroy previous data
    }

    ASSERT_TRUE(comm.request_received());
    scrutiny::protocol::Request const *req = comm.get_request();
    EXPECT_EQ(req->command_id, 1);
    EXPECT_EQ(req->subfunction_id, 2);
    EXPECT_EQ(req->data_length, 0);

    EXPECT_EQ(comm.get_rx_error(), scrutiny::protocol::RxError::None);
}

//=============================================================================
TEST_F(TestRxParsing, TestRx_NonZeroLen_AllInOne)
{
    unsigned char data[11] = { 1, 2, 0, 3, 0x11, 0x22, 0x33 };
    add_crc(data, 7);
    comm.receive_data(data, sizeof(data));

    ASSERT_TRUE(comm.request_received());
    scrutiny::protocol::Request const *req = comm.get_request();
    EXPECT_EQ(req->command_id, 1);
    EXPECT_EQ(req->subfunction_id, 2);
    EXPECT_EQ(req->data_length, 3);
    EXPECT_EQ(req->data[0], 0x11);
    EXPECT_EQ(req->data[1], 0x22);
    EXPECT_EQ(req->data[2], 0x33);

    EXPECT_EQ(comm.get_rx_error(), scrutiny::protocol::RxError::None);
}

//=============================================================================
TEST_F(TestRxParsing, TestRx_NonZeroLen_BytePerByte)
{
    unsigned char data[11] = { 1, 2, 0, 3, 0x11, 0x22, 0x33 };
    add_crc(data, 7);

    for (unsigned int i = 0; i < sizeof(data); i++)
    {
        comm.receive_data(&data[i], 1);
        data[i] ^= 0xFF; // Destroy previous data
    }

    ASSERT_TRUE(comm.request_received());
    scrutiny::protocol::Request const *req = comm.get_request();
    EXPECT_EQ(req->command_id, 1);
    EXPECT_EQ(req->subfunction_id, 2);
    EXPECT_EQ(req->data_length, 3);
    EXPECT_EQ(req->data[0], 0x11);
    EXPECT_EQ(req->data[1], 0x22);
    EXPECT_EQ(req->data[2], 0x33);

    EXPECT_EQ(comm.get_rx_error(), scrutiny::protocol::RxError::None);
}

//=============================================================================
TEST_F(TestRxParsing, TestRx_NonZeroLen_SplitInLen)
{
    unsigned char data[11] = { 1, 2, 0, 3, 0x11, 0x22, 0x33 };
    add_crc(data, 7);

    comm.receive_data(&data[0], 3); // cmd
    memset(data, 0xFF, 3);          // Destroy data to catch negative indexing
    comm.receive_data(&data[3], 8); // data

    ASSERT_TRUE(comm.request_received());
    scrutiny::protocol::Request const *req = comm.get_request();
    EXPECT_EQ(req->command_id, 1);
    EXPECT_EQ(req->subfunction_id, 2);
    EXPECT_EQ(req->data_length, 3);
    EXPECT_EQ(req->data[0], 0x11);
    EXPECT_EQ(req->data[1], 0x22);
    EXPECT_EQ(req->data[2], 0x33);

    EXPECT_EQ(comm.get_rx_error(), scrutiny::protocol::RxError::None);
}

//=============================================================================
TEST_F(TestRxParsing, TestRx_Discover_2_parts)
{
    unsigned char data[12] = { 1, 2, 0, 4, 0x7e, 0x18, 0xfc, 0x68 };
    add_crc(data, 8);

    for (uint16_t i = 0; i < sizeof(data); i++)
    {
        comm.receive_data(&data[0], i);
        comm.process();

        comm.receive_data(&data[i], sizeof(data) - i);

        ASSERT_TRUE(comm.request_received()) << "i=" << i;
        scrutiny::protocol::Request const *req = comm.get_request();
        EXPECT_EQ(req->command_id, 1) << "i=" << i;
        EXPECT_EQ(req->subfunction_id, 2) << "i=" << i;
        EXPECT_EQ(req->data_length, 4) << "i=" << i;
        EXPECT_EQ(req->data[0], 0x7e) << "i=" << i;
        EXPECT_EQ(req->data[1], 0x18) << "i=" << i;
        EXPECT_EQ(req->data[2], 0xfc) << "i=" << i;
        EXPECT_EQ(req->data[3], 0x68) << "i=" << i;

        EXPECT_EQ(comm.get_rx_error(), scrutiny::protocol::RxError::None) << "i=" << i;
    }
}

//=============================================================================
TEST_F(TestRxParsing, TestRx_UseAllBuffer)
{
    ASSERT_LT(sizeof(_rx_buffer), scrutiny::protocol::MAXIMUM_RX_BUFFER_SIZE); // Lengths are 16bits maximum by protocol definition

    uint16_t datalen = sizeof(_rx_buffer);

    unsigned char data[sizeof(_rx_buffer) + 8] = { 1,
                                                   2,
                                                   static_cast<unsigned char>((datalen >> 8) & 0xFF),
                                                   static_cast<unsigned char>(datalen & 0xFF) };
    add_crc(data, sizeof(_rx_buffer) + 4);

    comm.receive_data(data, sizeof(data));

    ASSERT_TRUE(comm.request_received());
}

//=============================================================================
TEST_F(TestRxParsing, TestRx_Overflow)
{
    ASSERT_LT(sizeof(_rx_buffer), scrutiny::protocol::MAXIMUM_RX_BUFFER_SIZE); // Lengths are 16bits maximum by protocol definition

    uint16_t datalen = sizeof(_rx_buffer) + 1;

    unsigned char data[sizeof(_rx_buffer) + 8] = { 1,
                                                   2,
                                                   static_cast<unsigned char>((datalen >> 8) & 0xFF),
                                                   static_cast<unsigned char>(datalen & 0xFF) };
    add_crc(data, sizeof(_rx_buffer) + 4);

    comm.receive_data(data, sizeof(data));

    ASSERT_FALSE(comm.request_received());
    EXPECT_EQ(comm.get_rx_error(), scrutiny::protocol::RxError::Overflow);
}

//=============================================================================
TEST_F(TestRxParsing, TestRx_OverflowRestoreAfterDelay)
{
    ASSERT_LT(sizeof(_rx_buffer), scrutiny::protocol::MAXIMUM_RX_BUFFER_SIZE); // Lengths are 16bits maximum by protocol definition

    uint16_t datalen = sizeof(_rx_buffer) + 1;

    unsigned char data[sizeof(_rx_buffer) + 8] = { 1,
                                                   2,
                                                   static_cast<unsigned char>((datalen >> 8) & 0xFF),
                                                   static_cast<unsigned char>(datalen & 0xFF) };
    add_crc(data, sizeof(_rx_buffer) + 4);

    comm.receive_data(data, sizeof(data));

    ASSERT_FALSE(comm.request_received());
    EXPECT_EQ(comm.get_rx_error(), scrutiny::protocol::RxError::Overflow);

    tb.step(SCRUTINY_COMM_RX_TIMEOUT_US * 10); // Increase the timesbase from enough time for comm to restart

    datalen = sizeof(_rx_buffer);
    unsigned char data2[sizeof(_rx_buffer) + 8] = { 1,
                                                    2,
                                                    static_cast<unsigned char>((datalen >> 8) & 0xFF),
                                                    static_cast<unsigned char>(datalen & 0xFF) };
    add_crc(data2, sizeof(_rx_buffer) + 4);

    comm.receive_data(data2, sizeof(data2));

    ASSERT_TRUE(comm.request_received());
}

//=============================================================================
TEST_F(TestRxParsing, TestRx_Timeout)
{
    unsigned char data[11] = { 1, 2, 0, 3, 0x11, 0x22, 0x33 };
    add_crc(data, 7);

    for (unsigned char i = 1; i < sizeof(data) - 1; i++)
    {
        comm.receive_data(&data[0], i);
        ASSERT_FALSE(comm.request_received());
        tb.step(SCRUTINY_COMM_RX_TIMEOUT_US * 10);
        comm.receive_data(&data[i], sizeof(data) - i);
        ASSERT_FALSE(comm.request_received());
        comm.reset();
    }
}

//=============================================================================
TEST_F(TestRxParsing, TestRx_BadCRC)
{
    unsigned char data[11] = { 1, 2, 0, 3, 0x11, 0x22, 0x33 };
    add_crc(data, 7);
    data[10] = ~data[10]; // Force bad CRC
    comm.receive_data(data, sizeof(data));

    ASSERT_FALSE(comm.request_received());
}

//=============================================================================
TEST_F(TestRxParsing, TestRx_ReceiveResponseBit_Ignore)
{
    unsigned char data[12] = { 0x81, 2, 0, 4, 0x7e, 0x18, 0xfc, 0x68 };
    add_crc(data, sizeof(data) - 4);
    EXPECT_EQ(comm.get_rx_error(), scrutiny::protocol::RxError::None);
    comm.receive_data(data, sizeof(data));

    EXPECT_FALSE(comm.request_received());
    EXPECT_EQ(comm.get_rx_error(), scrutiny::protocol::RxError::InvalidCommand);

    tb.step(SCRUTINY_COMM_RX_TIMEOUT_US * 10);
    data[0] = 0x1;
    comm.receive_data(data, 1);
    EXPECT_EQ(comm.get_rx_error(), scrutiny::protocol::RxError::None);
}
