//    test_memory_control.cpp
//        Test the behaviour of the embedded module when MemoryControl commands are received
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny.hpp"
#include "scrutiny_test.hpp"
#include "scrutinytest/scrutinytest.hpp"
#include <cstring>

static unsigned char _rx_buffer[128];
static unsigned char _tx_buffer[128];

class TestMemoryControl : public ScrutinyTest
{
  protected:
    scrutiny::Timebase tb;
    scrutiny::MainHandler scrutiny_handler;
    scrutiny::Config config;

    TestMemoryControl() :
        ScrutinyTest(),
        tb(),
        scrutiny_handler(),
        config()
    {
    }

    virtual void SetUp()
    {
        config.set_buffers(_rx_buffer, sizeof(_rx_buffer), _tx_buffer, sizeof(_tx_buffer));
        scrutiny_handler.init(&config);
        scrutiny_handler.comm()->connect();
    }
};

// ================================= Read =================================
/*
    Read a single memory block and expect a response with a valid content
*/
TEST_F(TestMemoryControl, TestReadSingleAddress)
{
// Building request
#if CHAR_BIT == 8
    unsigned char data_buf[] = { 0x11, 0x22, 0x33 };
#elif CHAR_BIT == 16
    unsigned char data_buf[] = { 0x1122, 0x3344, 0x5566 };
#endif
    SCRUTINY_CONSTEXPR uint32_t addr_size = SIZEOF_8BITS(uintptr_t);
    SCRUTINY_CONSTEXPR uint16_t data_size_8bits = SIZEOF_8BITS(data_buf);
    unsigned char request_data[8 + addr_size + 2] = { 3, 1, 0, addr_size + 2 };
    unsigned int index = 4;
    index += encode_addr(&request_data[index], data_buf);
    request_data[index++] = (data_size_8bits >> 8) & 0xFF;
    request_data[index++] = (data_size_8bits >> 0) & 0xFF;
    add_crc(request_data, sizeof(request_data) - 4);

    // Building expected response
    unsigned char tx_buffer[32];
    SCRUTINY_CONSTEXPR uint16_t datalen = addr_size + 2 + data_size_8bits;
    unsigned char expected_response[9 + datalen] = { 0x83, 1, 0, 0, datalen };
    index = 5;
    index += encode_addr(&expected_response[index], data_buf);
    expected_response[index++] = (data_size_8bits >> 8) & 0xFF;
    expected_response[index++] = (data_size_8bits >> 0) & 0xFF;

    // Manually encode the response to guarantee big endianness
#if CHAR_BIT == 8
    expected_response[index++] = 0x11;
    expected_response[index++] = 0x22;
    expected_response[index++] = 0x33;
#elif CHAR_BIT == 16
    // Protocol is big endian, but data bytes endianness is target specific.
    if (is_little_endian())
    {
        expected_response[index++] = 0x22;
        expected_response[index++] = 0x11;
        expected_response[index++] = 0x44;
        expected_response[index++] = 0x33;
        expected_response[index++] = 0x66;
        expected_response[index++] = 0x55;
    }
    else
    {
        expected_response[index++] = 0x11;
        expected_response[index++] = 0x22;
        expected_response[index++] = 0x33;
        expected_response[index++] = 0x44;
        expected_response[index++] = 0x55;
        expected_response[index++] = 0x66;
    }
#endif
    add_crc(expected_response, sizeof(expected_response) - 4);

    // Process
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

/*
    Reads a 3 memory block and expect a response with a valid content
*/
TEST_F(TestMemoryControl, TestReadMultipleAddress)
{
#if CHAR_BIT == 8
    unsigned char data_buf1[] = { 0x11, 0x22, 0x33 };
    unsigned char data_buf2[] = { 0x12, 0x34, 0x56, 0x78 };
    unsigned char data_buf3[] = { 0x13, 0x24 };
#elif CHAR_BIT == 16
    unsigned char data_buf1[] = { 0x1122, 0x3344, 0x5566 };
    unsigned char data_buf2[] = { 0x1234, 0x5678, 0x9abc, 0xdef0 };
    unsigned char data_buf3[] = { 0x1324, 0x3546 };

#endif
    unsigned char tx_buffer[64];
    SCRUTINY_CONSTEXPR uint32_t addr_size = SIZEOF_8BITS(uintptr_t);
    SCRUTINY_CONSTEXPR uint16_t data_size1 = SIZEOF_8BITS(data_buf1);
    SCRUTINY_CONSTEXPR uint16_t data_size2 = SIZEOF_8BITS(data_buf2);
    SCRUTINY_CONSTEXPR uint16_t data_size3 = SIZEOF_8BITS(data_buf3);
    SCRUTINY_CONSTEXPR uint16_t datalen_req = (addr_size + 2) * 3;

    // Building request
    unsigned char request_data[8 + datalen_req] = { 3, 1, 0, datalen_req };
    unsigned int index = 4;
    index += encode_addr(&request_data[index], data_buf1);
    request_data[index++] = (data_size1 >> 8) & 0xFF;
    request_data[index++] = (data_size1 >> 0) & 0xFF;
    index += encode_addr(&request_data[index], data_buf2);
    request_data[index++] = (data_size2 >> 8) & 0xFF;
    request_data[index++] = (data_size2 >> 0) & 0xFF;
    index += encode_addr(&request_data[index], data_buf3);
    request_data[index++] = (data_size3 >> 8) & 0xFF;
    request_data[index++] = (data_size3 >> 0) & 0xFF;
    add_crc(request_data, sizeof(request_data) - 4);

    // Building expected_response
    SCRUTINY_CONSTEXPR uint16_t datalen_resp = (addr_size + 2) * 3 + data_size1 + data_size2 + data_size3;
    unsigned char expected_response[9 + datalen_resp] = { 0x83, 1, 0, 0, datalen_resp };
    index = 5;
    index += encode_addr(&expected_response[index], data_buf1);
    expected_response[index++] = (data_size1 >> 8) & 0xFF;
    expected_response[index++] = (data_size1 >> 0) & 0xFF;
    scrutiny::tools::memcpy_dilate_8bits_native(&expected_response[index], data_buf1, data_size1);
    index += data_size1;
    index += encode_addr(&expected_response[index], data_buf2);
    expected_response[index++] = (data_size2 >> 8) & 0xFF;
    expected_response[index++] = (data_size2 >> 0) & 0xFF;
    scrutiny::tools::memcpy_dilate_8bits_native(&expected_response[index], data_buf2, data_size2);
    index += data_size2;
    index += encode_addr(&expected_response[index], data_buf3);
    expected_response[index++] = (data_size3 >> 8) & 0xFF;
    expected_response[index++] = (data_size3 >> 0) & 0xFF;
    scrutiny::tools::memcpy_dilate_8bits_native(&expected_response[index], data_buf3, data_size3);
    index += data_size3;
    add_crc(expected_response, sizeof(expected_response) - 4);

    // Processing
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

/*
    Sends multiple requests with an invalid amount of data and expect to receive and "InvalidRequest" response
*/
TEST_F(TestMemoryControl, TestReadAddressInvalidRequest)
{
    SCRUTINY_CONSTEXPR uint32_t addr_size = SIZEOF_8BITS(void *);
    const scrutiny::protocol::CommandId::eCommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    uint_least8_t const subfn = static_cast<uint_least8_t>(scrutiny::protocol::MemoryControl::Subfunction::Read);
    const scrutiny::protocol::ResponseCode::eResponseCode code = scrutiny::protocol::ResponseCode::InvalidRequest;

    unsigned char tx_buffer[64];

    // Building request
    unsigned char request_data[64] = { static_cast<unsigned char>(cmd), subfn };
    uint16_t length_to_receive;
    for (unsigned int i = 0; i < 32; i++)
    {
        if (i % (addr_size + 2) == 0)
        {
            // This is a valid request. We skip it
            continue;
        }

        uint16_t length_to_test = static_cast<uint16_t>(i);
        length_to_receive = 8 + length_to_test;
        request_data[2] = static_cast<unsigned char>(length_to_test >> 8); // Encode length
        request_data[3] = static_cast<unsigned char>(length_to_test);
        add_crc(request_data, length_to_receive - 4);

        scrutiny_handler.receive_data(request_data, length_to_receive);
        scrutiny_handler.process(0);

        uint16_t n_to_read = scrutiny_handler.data_to_send();
        ASSERT_GT(n_to_read, 0u) << "[ i=" << static_cast<uint32_t>(i) << "]";
        ASSERT_LT(n_to_read, sizeof(tx_buffer)) << "[i=" << static_cast<uint32_t>(i) << "]";
        scrutiny_handler.pop_data(tx_buffer, n_to_read);
        // Now we expect an InvalidRequest response
        ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, code) << "[i=" << static_cast<uint32_t>(i) << "]";
        scrutiny_handler.process(0);
    }
}

/*
    Sends multiple request for 2 blocks of data. The first block will almost fill the transmit buffer.
    Depending on the size of the second block, we expect either a valid respons or an "overflow" response if we ask
    for more data than what can fit in the TX buffer.
*/

TEST_F(TestMemoryControl, TestReadAddressOverflow)
{
    SCRUTINY_CONSTEXPR uint32_t addr_size = SIZEOF_8BITS(void *);
    const scrutiny::protocol::CommandId::eCommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    uint_least8_t const subfn = static_cast<uint_least8_t>(scrutiny::protocol::MemoryControl::Subfunction::Read);
    const scrutiny::protocol::ResponseCode::eResponseCode overflow = scrutiny::protocol::ResponseCode::Overflow;
    const scrutiny::protocol::ResponseCode::eResponseCode ok = scrutiny::protocol::ResponseCode::OK;

    unsigned char tx_buffer[sizeof(_tx_buffer) + 9];
    unsigned char some_buffer[sizeof(_tx_buffer)] = { 0 };
    uint16_t buf1_size_8bits = sizeof(_tx_buffer) - (addr_size + 2) * 2 - (CHAR_BIT / 8); // We fill all the buffer minus 1 byte.
    // buf1_size_8bits uses sizeof tx buffer, tx buffer is counted in multiple of 8 bits. This is not a mistake.

    // Building request
    unsigned char request_data[64] = { static_cast<unsigned char>(cmd), subfn, 0, (addr_size + 2) * 2 };
    unsigned int index = 4;
    index += encode_addr(&request_data[index], &some_buffer);
    request_data[index++] = static_cast<unsigned char>(buf1_size_8bits >> 8);
    request_data[index++] = static_cast<unsigned char>(buf1_size_8bits >> 0);

    index += encode_addr(&request_data[index], &some_buffer); // 2nd block

    uint16_t length_to_receive;
    // Increase length of 2nd block of data.
    for (unsigned int length_char = 0; length_char < 4; length_char += (CHAR_BIT / 8))
    {
        length_to_receive = 8 + (addr_size + 2) * 2;
        uint16_t length_8bits = length_char * (CHAR_BIT / 8);

        request_data[index + 0] = static_cast<unsigned char>(length_8bits >> 8); // encode length of 2nd block
        request_data[index + 1] = static_cast<unsigned char>(length_8bits >> 0);
        add_crc(request_data, length_to_receive - 4);

        scrutiny_handler.receive_data(request_data, length_to_receive);
        scrutiny_handler.process(0);

        uint16_t n_to_read = scrutiny_handler.data_to_send();
        ASSERT_LE(n_to_read, sizeof(tx_buffer));
        scrutiny_handler.pop_data(tx_buffer, n_to_read);

        if (length_char < 2) // Below 2, we don't overflow the tx buffer
        {
            ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, ok) << "[length=" << static_cast<uint32_t>(length_char) << "]";
        }
        else // We should have overflown the tx buffer here.
        {
            ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, overflow) << "[length=" << static_cast<uint32_t>(length_char) << "]";
        }
        scrutiny_handler.process(0);
    }
}

TEST_F(TestMemoryControl, TestReadLengthCauseInternalOverflow)
{
    SCRUTINY_CONSTEXPR uint32_t addr_size = SIZEOF_8BITS(void *);
    const scrutiny::protocol::CommandId::eCommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    uint_least8_t const subfn = static_cast<uint_least8_t>(scrutiny::protocol::MemoryControl::Subfunction::Read);
    const scrutiny::protocol::ResponseCode::eResponseCode invalid_request = scrutiny::protocol::ResponseCode::InvalidRequest;

    unsigned char tx_buffer[16] = { 0 };
    unsigned char some_buffer[16] = { 0 };
    uint16_t buf1_size = 0xFFFF;

    // Building request
    unsigned char request_data[64] = { static_cast<unsigned char>(cmd), subfn, 0, (addr_size + 2) };
    unsigned int index = 4;
    index += encode_addr(&request_data[index], &some_buffer);
    request_data[index++] = static_cast<unsigned char>(buf1_size >> 8);
    request_data[index++] = static_cast<unsigned char>(buf1_size >> 0);

    uint16_t length_to_receive = 8 + (addr_size + 2);
    add_crc(request_data, length_to_receive - 4);

    scrutiny_handler.receive_data(request_data, length_to_receive);
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_GT(n_to_read, 0);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    scrutiny_handler.pop_data(tx_buffer, n_to_read);

    ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, invalid_request);
}

/*
We try to read a forbidden memory region and expects to be responded with a "Forbidden" response code.
We define a buffer of 16 bytes. We forbid access to 4 bytes in the middle then try to read a window
of 4 bytes that start at the beginning of the buffer then slide to the right.

4 accesses won't touch the forbidden region. 8 Will touch it.
*/
TEST_F(TestMemoryControl, TestReadForbiddenAddress)
{
    const scrutiny::protocol::CommandId::eCommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    uint_least8_t const subfn = static_cast<uint_least8_t>(scrutiny::protocol::MemoryControl::Subfunction::Read);
    const scrutiny::protocol::ResponseCode::eResponseCode forbidden = scrutiny::protocol::ResponseCode::Forbidden;
    const scrutiny::protocol::ResponseCode::eResponseCode ok = scrutiny::protocol::ResponseCode::OK;

    unsigned char tx_buffer[32];
    unsigned char buf[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    // indices [6,7,8,9] are forbidden
    uintptr_t start = reinterpret_cast<uintptr_t>(buf) + 6;
    uintptr_t end = start + 4;
    scrutiny::AddressRange forbidden_ranges[] = { scrutiny::tools::make_address_range(start, end) };
    config.set_forbidden_address_range(forbidden_ranges, sizeof(forbidden_ranges) / sizeof(scrutiny::AddressRange));

    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    SCRUTINY_CONSTEXPR unsigned char datalen = SIZEOF_8BITS(void *) + 2;
    unsigned char request_data[8 + datalen] = { 3, 1, 0, datalen };
    uint16_t window_size = 4;
    uint16_t window_size_8bits = (window_size * (CHAR_BIT / 8));
    unsigned int index = 0;
    for (unsigned int i = 0; i < sizeof(buf) - window_size; i++)
    {
        void *read_addr = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(buf) + i);
        index = 4;
        index += encode_addr(&request_data[4], read_addr);
        request_data[index + 0] = static_cast<unsigned char>((window_size_8bits >> 8) & 0xFF);
        request_data[index + 1] = static_cast<unsigned char>((window_size_8bits >> 0) & 0xFF);
        add_crc(request_data, sizeof(request_data) - 4);

        scrutiny_handler.receive_data(request_data, sizeof(request_data));
        scrutiny_handler.process(0);

        uint16_t n_to_read = scrutiny_handler.data_to_send();
        ASSERT_LT(n_to_read, sizeof(tx_buffer));
        scrutiny_handler.pop_data(tx_buffer, n_to_read);

        if (i < 2 || i > 10) // Sliding window is completely out of forbidden region
        {
            ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, ok) << "[i=" << static_cast<uint32_t>(i) << "]";
        }
        else // We expect to be refused access to buffer
        {
            ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, forbidden) << "[i=" << static_cast<uint32_t>(i) << "]";
        }
        scrutiny_handler.process(0);
    }
}

/*
We make sure we can read readonly adress ranges without issues. Same test as TestReadForbiddenAddress, but we expect OK response code.
*/
TEST_F(TestMemoryControl, TestReadReadonlyAddress)
{
    const scrutiny::protocol::CommandId::eCommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    uint_least8_t const subfn = static_cast<uint_least8_t>(scrutiny::protocol::MemoryControl::Subfunction::Read);
    const scrutiny::protocol::ResponseCode::eResponseCode ok = scrutiny::protocol::ResponseCode::OK;

    unsigned char tx_buffer[32];
    unsigned char buf[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

    // indices [6,7,8,9] are readonly
    uintptr_t start = reinterpret_cast<uintptr_t>(buf) + 6;
    uintptr_t end = start + 4;
    scrutiny::AddressRange readonly_range[] = { scrutiny::tools::make_address_range(start, end) };
    config.set_readonly_address_range(readonly_range, sizeof(readonly_range) / sizeof(scrutiny::AddressRange));

    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    SCRUTINY_CONSTEXPR unsigned char datalen = SIZEOF_8BITS(void *) + 2;
    unsigned char request_data[8 + datalen] = { 3, 1, 0, datalen };
    uint16_t window_size = 4;
    unsigned int index = 0;
    for (unsigned int i = 0; i < sizeof(buf) - window_size; i++)
    {
        void *read_addr = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(buf) + i);
        index = 4;
        index += encode_addr(&request_data[4], read_addr);
        request_data[index + 0] = static_cast<unsigned char>(window_size >> 8);
        request_data[index + 1] = static_cast<unsigned char>(window_size >> 0);
        add_crc(request_data, sizeof(request_data) - 4);

        scrutiny_handler.receive_data(request_data, sizeof(request_data));
        scrutiny_handler.process(0);

        uint16_t n_to_read = scrutiny_handler.data_to_send();
        ASSERT_LT(n_to_read, sizeof(tx_buffer));
        scrutiny_handler.pop_data(tx_buffer, n_to_read);

        ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, ok) << "[i=" << static_cast<uint32_t>(i) << "]";

        scrutiny_handler.process(0);
    }
}

#if CHAR_BIT == 16
TEST_F(TestMemoryControl, TestReadSingleAddressNotMultipleOfCharDenied)
{
    const scrutiny::protocol::CommandId::eCommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    uint_least8_t const subfn = static_cast<uint_least8_t>(scrutiny::protocol::MemoryControl::Subfunction::Read);
    const scrutiny::protocol::ResponseCode::eResponseCode invalid = scrutiny::protocol::ResponseCode::InvalidRequest;

    unsigned char buffer[] = { 0x0102, 0x0304, 0x0506, 0x0708, 0x090a };
    uint16_t read_size_8bits = 5; // try to read 2.5 char

    // Building request
    SCRUTINY_CONSTEXPR uint32_t addr_size = SIZEOF_8BITS(uintptr_t);
    SCRUTINY_CONSTEXPR uint16_t datalen_req = addr_size + 2;
    unsigned char request_data[8 + datalen_req] = { 3, 1, 0, datalen_req };
    unsigned int index = 4;
    index += encode_addr(&request_data[index], buffer);
    request_data[index++] = (read_size_8bits >> 8) & 0xFF;
    request_data[index++] = (read_size_8bits >> 0) & 0xFF;
    add_crc(request_data, sizeof(request_data) - 4);

    // Building expected response
    unsigned char tx_buffer[32];
    // Process
    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, invalid);
}
#endif

// ================================= Write =================================

/*
    Write a single memory block and expect it to be written.
*/
TEST_F(TestMemoryControl, TestWriteSingleAddress)
{

    unsigned char data_to_write_8bits[] = { 0x11, 0x22, 0x33, 0x44 }; // big endian encoded
#if CHAR_BIT == 8
    unsigned char buffer[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    unsigned char expected_output_buffer[] = { 0x11, 0x22, 0x33, 0x44, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
#elif CHAR_BIT == 16
    unsigned char buffer[] = { 0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C, 0x0D0E, 0x0F10, 0x1011, 0x1213 };
    unsigned char expected_output_buffer_be[] = { 0x1122, 0x3344, 0x0506, 0x0708, 0x090A, 0x0B0C, 0x0D0E, 0x0F10, 0x1011, 0x1213 };
    unsigned char expected_output_buffer_le[] = { 0x2211, 0x4433, 0x0506, 0x0708, 0x090A, 0x0B0C, 0x0D0E, 0x0F10, 0x1011, 0x1213 };
#endif

    // Building request
    SCRUTINY_CONSTEXPR uint32_t addr_size = SIZEOF_8BITS(uintptr_t);
    SCRUTINY_CONSTEXPR uint16_t datalen_req = addr_size + 2 + sizeof(data_to_write_8bits);
    unsigned char request_data[8 + datalen_req] = { 3, 2, 0, datalen_req };
    unsigned int index = 4;
    index += encode_addr(&request_data[index], buffer);
    request_data[index++] = (sizeof(data_to_write_8bits) >> 8) & 0xFF;
    request_data[index++] = (sizeof(data_to_write_8bits) >> 0) & 0xFF;
    std::memcpy(&request_data[index], data_to_write_8bits, sizeof(data_to_write_8bits));
    add_crc(request_data, sizeof(request_data) - 4);

    // Building expected response
    unsigned char tx_buffer[32];
    SCRUTINY_CONSTEXPR uint16_t datalen_resp = addr_size + 2;
    unsigned char expected_response[9 + datalen_resp] = { 0x83, 2, 0, 0, datalen_resp };
    index = 5;
    index += encode_addr(&expected_response[index], buffer);
    expected_response[index++] = (sizeof(data_to_write_8bits) >> 8) & 0xFF;
    expected_response[index++] = (sizeof(data_to_write_8bits) >> 0) & 0xFF;
    add_crc(expected_response, sizeof(expected_response) - 4);

    // Process
    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);

    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
#if CHAR_BIT == 8
    ASSERT_BUF_EQ(buffer, expected_output_buffer, sizeof(expected_output_buffer));
#elif CHAR_BIT == 16
    if (is_little_endian())
    {
        ASSERT_BUF_EQ(buffer, expected_output_buffer_le, sizeof(expected_output_buffer_le));
    }
    else
    {
        ASSERT_BUF_EQ(buffer, expected_output_buffer_be, sizeof(expected_output_buffer_be));
    }
#endif    
}

/*
    Write a single memory block and expect it to be written.
*/
TEST_F(TestMemoryControl, TestWriteSingleAddressMasked)
{

    unsigned char data_to_write_8bits[] = { 0xFF, 0xFF, 0x00, 0x00, 0x55, 0x55 };
    unsigned char write_mask_8bits[] = { 0xF0, 0xAA, 0xF0, 0xAA, 0xAF, 0x55 };
#if CHAR_BIT == 8
    unsigned char buffer[] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
    unsigned char expected_output_buffer[] = { 0xFA, 0xAA, 0x0A, 0x00, 0x05, 0xFF, 0xAA, 0xAA, 0xAA, 0xAA };
#elif CHAR_BIT == 16
    unsigned char buffer[] = { 0xAAAA, 0xAAAA, 0xAAAA, 0xAAAA, 0xAAAA, 0xAAAA };
    unsigned char expected_output_buffer_be[] = { 0xFAAA, 0x0A00, 0x05FF, 0xAAAA, 0xAAAA, 0xAAAA };
    unsigned char expected_output_buffer_le[] = { 0xAAFA, 0x000A, 0xFF05, 0xAAAA, 0xAAAA, 0xAAAA };
#endif

    // Building request
    SCRUTINY_CONSTEXPR uint32_t addr_size = SIZEOF_8BITS(uintptr_t);
    SCRUTINY_CONSTEXPR uint16_t datalen_req = addr_size + 2 + sizeof(data_to_write_8bits) * 2;
    unsigned char request_data[8 + datalen_req] = { 3, 3, 0, datalen_req };
    unsigned int index = 4;
    index += encode_addr(&request_data[index], buffer);
    request_data[index++] = (sizeof(data_to_write_8bits) >> 8) & 0xFF;
    request_data[index++] = (sizeof(data_to_write_8bits) >> 0) & 0xFF;
    std::memcpy(&request_data[index], data_to_write_8bits, sizeof(data_to_write_8bits));
    index += sizeof(data_to_write_8bits);
    std::memcpy(&request_data[index], write_mask_8bits, sizeof(write_mask_8bits));
    add_crc(request_data, sizeof(request_data) - 4);

    // Building expected response
    unsigned char tx_buffer[32];
    SCRUTINY_CONSTEXPR uint16_t datalen_resp = addr_size + 2;
    unsigned char expected_response[9 + datalen_resp] = { 0x83, 3, 0, 0, datalen_resp };
    index = 5;
    index += encode_addr(&expected_response[index], buffer);
    expected_response[index++] = (sizeof(data_to_write_8bits) >> 8) & 0xFF;
    expected_response[index++] = (sizeof(data_to_write_8bits) >> 0) & 0xFF;
    add_crc(expected_response, sizeof(expected_response) - 4);

    // Process
    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);

    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
#if CHAR_BIT == 8
    ASSERT_BUF_EQ(buffer, expected_output_buffer, sizeof(expected_output_buffer));
#elif CHAR_BIT == 16
    if (is_little_endian())
    {
        ASSERT_BUF_EQ(buffer, expected_output_buffer_le, sizeof(expected_output_buffer_le));
    }
    else
    {
        ASSERT_BUF_EQ(buffer, expected_output_buffer_be, sizeof(expected_output_buffer_be));
    }
#endif  
}

/*
    Write 2  memory block in a single request and expect it to be written.
*/
TEST_F(TestMemoryControl, TestWriteMultipleAddress)
{

    unsigned char data_to_write1[] = { 0x11, 0x22, 0x33, 0x44 };
    unsigned char data_to_write2[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    unsigned char buffer[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
#if CHAR_BIT == 8
    unsigned char expected_output_buffer[] = { 0x11, 0x22, 0x33, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x0a };
#elif CHAR_BIT == 16
    unsigned char expected_output_buffer_be[] = { 0x1122, 0x3344, 0x03, 0xAABB, 0xCCDD, 0xEEFF, 0x07, 0x08, 0x09, 0x0a };
    unsigned char expected_output_buffer_le[] = { 0x2211, 0x4433, 0x03, 0xBBAA, 0xDDCC, 0xFFEE, 0x07, 0x08, 0x09, 0x0a };
#endif

    // Building request
    SCRUTINY_CONSTEXPR uint32_t addr_size = SIZEOF_8BITS(uintptr_t);
    SCRUTINY_CONSTEXPR uint16_t datalen_req = (addr_size + 2) * 2 + sizeof(data_to_write1) + sizeof(data_to_write2);
    unsigned char request_data[8 + datalen_req] = { 3, 2, 0, datalen_req };
    unsigned int index = 4;
    index += encode_addr(&request_data[index], buffer);
    request_data[index++] = (sizeof(data_to_write1) >> 8) & 0xFF;
    request_data[index++] = (sizeof(data_to_write1) >> 0) & 0xFF;
    std::memcpy(&request_data[index], data_to_write1, sizeof(data_to_write1));
    index += sizeof(data_to_write1);
    index += encode_addr(&request_data[index], &buffer[3]);
    request_data[index++] = (sizeof(data_to_write2) >> 8) & 0xFF;
    request_data[index++] = (sizeof(data_to_write2) >> 0) & 0xFF;
    std::memcpy(&request_data[index], data_to_write2, sizeof(data_to_write2));
    index += sizeof(data_to_write2);
    add_crc(request_data, sizeof(request_data) - 4);

    // Building expected response
    unsigned char tx_buffer[32];
    SCRUTINY_CONSTEXPR uint16_t datalen_resp = (addr_size + 2) * 2;
    unsigned char expected_response[9 + datalen_resp] = { 0x83, 2, 0, 0, datalen_resp };
    index = 5;
    index += encode_addr(&expected_response[index], buffer);
    expected_response[index++] = (sizeof(data_to_write1) >> 8) & 0xFF;
    expected_response[index++] = (sizeof(data_to_write1) >> 0) & 0xFF;
    index += encode_addr(&expected_response[index], &buffer[3]);
    expected_response[index++] = (sizeof(data_to_write2) >> 8) & 0xFF;
    expected_response[index++] = (sizeof(data_to_write2) >> 0) & 0xFF;
    add_crc(expected_response, sizeof(expected_response) - 4);

    // Process
    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);

    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
#if CHAR_BIT == 8
    ASSERT_BUF_EQ(buffer, expected_output_buffer, sizeof(expected_output_buffer));
#elif CHAR_BIT == 16
    if (is_little_endian())
    {
        ASSERT_BUF_EQ(buffer, expected_output_buffer_le, sizeof(expected_output_buffer_le));
    }
    else
    {
        ASSERT_BUF_EQ(buffer, expected_output_buffer_be, sizeof(expected_output_buffer_be));
    }
#endif   
}

/*
    Write 2  memory block in a single request and expect it to be written with a mask.
*/
TEST_F(TestMemoryControl, TestWriteMultipleAddressMasked)
{
    unsigned char data_to_write1[] = { 0x11, 0x22, 0x33, 0x44 };
    unsigned char write_mask1[] = { 0xFF, 0x00, 0xF0, 0x0F };
    unsigned char data_to_write2[] = { 0xAA, 0xAA, 0x55, 0x55, 0x0F, 0x00 };
    unsigned char write_mask2[] = { 0x0F, 0xAA, 0xAA, 0x55, 0xF0, 0x00 };

#if CHAR_BIT == 8
    unsigned char buffer[] = { 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x99 };
    unsigned char expected_output_buffer[] = { 0x11, 0x55, 0x35, 0x54, 0x5A, 0xFF, 0x55, 0x55, 0x05, 0x99 };
    unsigned int second_write_index = 4;
#elif CHAR_BIT == 16
    unsigned char buffer[] = { 0x5555, 0x5555, 0x5555, 0x5555, 0x5599 };
    unsigned char expected_output_buffer_be[] = { 0x1155, 0x3554, 0x5AFF, 0x5555, 0x0599 };
    unsigned char expected_output_buffer_le[] = { 0x5511, 0x5435, 0xFF5A, 0x5555, 0x5509 };
    unsigned int second_write_index = 2;
#endif

    // Building request
    SCRUTINY_CONSTEXPR uint32_t addr_size = SIZEOF_8BITS(uintptr_t);
    SCRUTINY_CONSTEXPR uint16_t datalen_req = (addr_size + 2) * 2 + sizeof(data_to_write1) * 2 + sizeof(data_to_write2) * 2;
    unsigned char request_data[8 + datalen_req] = { 3, 3, 0, datalen_req };
    unsigned int index = 4;
    index += encode_addr(&request_data[index], buffer);
    request_data[index++] = (sizeof(data_to_write1) >> 8) & 0xFF;
    request_data[index++] = (sizeof(data_to_write1) >> 0) & 0xFF;
    std::memcpy(&request_data[index], data_to_write1, sizeof(data_to_write1));
    index += sizeof(data_to_write1);
    std::memcpy(&request_data[index], write_mask1, sizeof(write_mask1));
    index += sizeof(write_mask1);

    index += encode_addr(&request_data[index], &buffer[second_write_index]);
    request_data[index++] = (sizeof(data_to_write2) >> 8) & 0xFF;
    request_data[index++] = (sizeof(data_to_write2) >> 0) & 0xFF;
    std::memcpy(&request_data[index], data_to_write2, sizeof(data_to_write2));
    index += sizeof(data_to_write2);
    std::memcpy(&request_data[index], write_mask2, sizeof(write_mask2));
    index += sizeof(write_mask2);
    add_crc(request_data, sizeof(request_data) - 4);

    // Building expected response
    unsigned char tx_buffer[32];
    SCRUTINY_CONSTEXPR uint16_t datalen_resp = (addr_size + 2) * 2;
    unsigned char expected_response[9 + datalen_resp] = { 0x83, 3, 0, 0, datalen_resp };
    index = 5;
    index += encode_addr(&expected_response[index], buffer);
    expected_response[index++] = (sizeof(data_to_write1) >> 8) & 0xFF;
    expected_response[index++] = (sizeof(data_to_write1) >> 0) & 0xFF;
    index += encode_addr(&expected_response[index], &buffer[second_write_index]);
    expected_response[index++] = (sizeof(data_to_write2) >> 8) & 0xFF;
    expected_response[index++] = (sizeof(data_to_write2) >> 0) & 0xFF;
    add_crc(expected_response, sizeof(expected_response) - 4);

    // Process
    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);

    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
#if CHAR_BIT == 8
    ASSERT_BUF_EQ(buffer, expected_output_buffer, sizeof(expected_output_buffer));
#elif CHAR_BIT == 16
    if (is_little_endian())
    {
        ASSERT_BUF_EQ(buffer, expected_output_buffer_le, sizeof(expected_output_buffer_le));
    }
    else
    {
        ASSERT_BUF_EQ(buffer, expected_output_buffer_be, sizeof(expected_output_buffer_be));
    }
#endif  
}

TEST_F(TestMemoryControl, TestWriteSingleAddress_InvalidDataLength)
{
    const scrutiny::protocol::CommandId::eCommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    uint_least8_t const subfn = static_cast<uint_least8_t>(scrutiny::protocol::MemoryControl::Subfunction::Write);
    const scrutiny::protocol::ResponseCode::eResponseCode invalid = scrutiny::protocol::ResponseCode::InvalidRequest;

    unsigned char buffer[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    unsigned char tx_buffer[32];

    // Building request
    SCRUTINY_CONSTEXPR uint32_t addr_size = SIZEOF_8BITS(uintptr_t);
    SCRUTINY_CONSTEXPR uint16_t data_size = 10;
    SCRUTINY_CONSTEXPR uint16_t datalen_req = addr_size + 2 + data_size;
    unsigned char request_data[8 + datalen_req] = { 3, 2, 0, datalen_req };
    unsigned int index = 4;
    index += encode_addr(&request_data[index], buffer);
    request_data[index++] = ((data_size + 1) >> 8) & 0xFF;
    request_data[index++] = ((data_size + 1) >> 0) & 0xFF;
    add_crc(request_data, sizeof(request_data) - 4);

    // Process
    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);

    ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, invalid);
}

TEST_F(TestMemoryControl, TestWriteSingleAddressMasked_InvalidDataLength)
{
    const scrutiny::protocol::CommandId::eCommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    uint_least8_t const subfn = static_cast<uint_least8_t>(scrutiny::protocol::MemoryControl::Subfunction::WriteMasked);
    const scrutiny::protocol::ResponseCode::eResponseCode invalid = scrutiny::protocol::ResponseCode::InvalidRequest;

    unsigned char buffer[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    unsigned char tx_buffer[32];

    // Building request
    SCRUTINY_CONSTEXPR uint32_t addr_size = SIZEOF_8BITS(uintptr_t);
    SCRUTINY_CONSTEXPR uint16_t data_size = 10;
    SCRUTINY_CONSTEXPR uint16_t datalen_req = addr_size + 2 + data_size;
    unsigned char request_data[8 + datalen_req] = { 3, 3, 0, datalen_req };
    unsigned int index = 4;
    index += encode_addr(&request_data[index], buffer);
    request_data[index++] = ((data_size + 1) >> 8) & 0xFF;
    request_data[index++] = ((data_size + 1) >> 0) & 0xFF;
    add_crc(request_data, sizeof(request_data) - 4);

    // Process
    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);

    ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, invalid);
}

/*
Tries to write to a forbidden address range. Same approach as TestReadForbiddenAddress.
Expect denial of access
*/
TEST_F(TestMemoryControl, TestWriteForbiddenAddress)
{
    const scrutiny::protocol::CommandId::eCommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    uint_least8_t const subfn = static_cast<uint_least8_t>(scrutiny::protocol::MemoryControl::Subfunction::Write);
    const scrutiny::protocol::ResponseCode::eResponseCode forbidden = scrutiny::protocol::ResponseCode::Forbidden;
    const scrutiny::protocol::ResponseCode::eResponseCode ok = scrutiny::protocol::ResponseCode::OK;

    unsigned char tx_buffer[32];
    unsigned char buf[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    // indices [6,7,8,9] are forbidden
    uintptr_t start = reinterpret_cast<uintptr_t>(buf) + 6;
    uintptr_t end = start + 4;
    scrutiny::AddressRange forbidden_ranges[] = { scrutiny::tools::make_address_range(start, end) };
    config.set_forbidden_address_range(forbidden_ranges, sizeof(forbidden_ranges) / sizeof(scrutiny::AddressRange));

    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    SCRUTINY_CONSTEXPR uint16_t window_size_8bits = 4 * (CHAR_BIT / 8);
    SCRUTINY_CONSTEXPR unsigned char datalen = SIZEOF_8BITS(void *) + 2 + window_size_8bits;
    unsigned char request_data[8 + datalen] = { 3, 2, 0, datalen };
    unsigned int index = 0;
    for (unsigned int i = 0; i < sizeof(buf) - window_size_8bits; i++)
    {
        void *write_addr = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(buf) + i);
        index = 4;
        index += encode_addr(&request_data[4], write_addr);
        request_data[index + 0] = static_cast<unsigned char>(window_size_8bits >> 8);
        request_data[index + 1] = static_cast<unsigned char>(window_size_8bits >> 0);
        // We don't care about the data to write here. We just check if it's accepted or refused.
        add_crc(request_data, sizeof(request_data) - 4);

        scrutiny_handler.receive_data(request_data, sizeof(request_data));
        scrutiny_handler.process(0);

        uint16_t n_to_read = scrutiny_handler.data_to_send();
        ASSERT_LT(n_to_read, sizeof(tx_buffer));
        scrutiny_handler.pop_data(tx_buffer, n_to_read);

        if (i < 2 || i > 10) // Sliding window is completely out of forbidden region
        {
            ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, ok) << "[i=" << static_cast<uint32_t>(i) << "]";
        }
        else // We expect to be refused access to buffer
        {
            ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, forbidden) << "[i=" << static_cast<uint32_t>(i) << "]";
        }
        scrutiny_handler.process(0);
    }
}

/*
Tries to write to a readonly address range. Same approach as TestReadForbiddenAddress
Expect denial of access
*/
TEST_F(TestMemoryControl, TestWriteReadOnlyAddress)
{
    const scrutiny::protocol::CommandId::eCommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    uint_least8_t const subfn = static_cast<uint_least8_t>(scrutiny::protocol::MemoryControl::Subfunction::Write);
    const scrutiny::protocol::ResponseCode::eResponseCode forbidden = scrutiny::protocol::ResponseCode::Forbidden;
    const scrutiny::protocol::ResponseCode::eResponseCode ok = scrutiny::protocol::ResponseCode::OK;

    unsigned char tx_buffer[32];
    unsigned char buf[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    // indices [6,7,8,9] are forbidden
    uintptr_t start = reinterpret_cast<uintptr_t>(buf) + 6;
    uintptr_t end = start + 4;
    scrutiny::AddressRange forbidden_ranges[] = { scrutiny::tools::make_address_range(start, end) };
    config.set_forbidden_address_range(forbidden_ranges, sizeof(forbidden_ranges) / sizeof(scrutiny::AddressRange));

    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    SCRUTINY_CONSTEXPR uint16_t window_size_8bits = 4 * (CHAR_BIT / 8);
    SCRUTINY_CONSTEXPR unsigned char datalen = SIZEOF_8BITS(void *) + 2 + window_size_8bits;
    unsigned char request_data[8 + datalen] = { 3, 2, 0, datalen };
    unsigned int index = 0;
    for (unsigned int i = 0; i < sizeof(buf) - window_size_8bits; i++)
    {
        void *write_addr = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(buf) + i);
        index = 4;
        index += encode_addr(&request_data[4], write_addr);
        request_data[index + 0] = static_cast<unsigned char>(window_size_8bits >> 8);
        request_data[index + 1] = static_cast<unsigned char>(window_size_8bits >> 0);
        // We don't care about the data to write here. We just check if it's accepted or refused.
        add_crc(request_data, sizeof(request_data) - 4);

        scrutiny_handler.receive_data(request_data, sizeof(request_data));
        scrutiny_handler.process(0);

        uint16_t n_to_read = scrutiny_handler.data_to_send();
        ASSERT_LT(n_to_read, sizeof(tx_buffer));
        scrutiny_handler.pop_data(tx_buffer, n_to_read);

        if (i < 2 || i > 10) // Sliding window is completely out of readonly region
        {
            ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, ok) << "[i=" << static_cast<uint32_t>(i) << "]";
        }
        else // We expect to be refused access to buffer
        {
            ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, forbidden) << "[i=" << static_cast<uint32_t>(i) << "]";
        }
        scrutiny_handler.process(0);
    }
}

/*
    Test that invalid request are processed as such.
    We make a valid request with 2 write command. Then we adjust the request length to have partial or extra data.
    We skip the 2 valid possiblities (1 full wirte command, 2 full write command). All the other shall be returned as invalid.
*/
TEST_F(TestMemoryControl, TestWriteMemoryInvalidRequest)
{
    const scrutiny::protocol::CommandId::eCommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    uint_least8_t const subfn = static_cast<uint_least8_t>(scrutiny::protocol::MemoryControl::Subfunction::Write);
    const scrutiny::protocol::ResponseCode::eResponseCode invalid = scrutiny::protocol::ResponseCode::InvalidRequest;

    SCRUTINY_CONSTEXPR uint32_t addr_size = SIZEOF_8BITS(void *);
    SCRUTINY_CONSTEXPR uint16_t data_to_write_length = 3;
    SCRUTINY_CONSTEXPR uint32_t datalen = (addr_size + 2 + data_to_write_length) * 2;
    unsigned char tx_buffer[64];

    unsigned char some_data[] = { 1, 2, 3, 4 };

    unsigned char request_data[8 + datalen + 64] = { 3, 2, 0, 0 }; // Add 64 bytes because we will put more data.

    for (uint16_t i = 0; i < datalen + 10; i++)
    {
        if (i == datalen || i == datalen / 2)
        {
            // datalen is for 2 valid request. Half of it is the correct length for a valid write to the first address only
            continue;
        }
        uint32_t index = 4;
        // First block
        index += encode_addr(&request_data[index], some_data);
        request_data[index++] = (data_to_write_length >> 8) & 0xFF;
        request_data[index++] = (data_to_write_length >> 0) & 0xFF;
        index += data_to_write_length;
        // Seond block
        index += encode_addr(&request_data[index], some_data);
        request_data[index++] = (data_to_write_length >> 8) & 0xFF;
        request_data[index++] = (data_to_write_length >> 0) & 0xFF;
        index += data_to_write_length;

        // Adjust request length to chop data or add extra bytes.
        request_data[2] = (i >> 8) & 0xFF;
        request_data[3] = (i >> 0) & 0xFF;
        add_crc(request_data, 4 + i);

        scrutiny_handler.receive_data(request_data, sizeof(request_data));
        scrutiny_handler.process(0);

        uint16_t n_to_read = scrutiny_handler.data_to_send();
        ASSERT_LT(n_to_read, sizeof(tx_buffer));
        scrutiny_handler.pop_data(tx_buffer, n_to_read);

        ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, invalid) << "[i=" << static_cast<uint32_t>(i) << "]";

        scrutiny_handler.process(0);
    }
}

#if CHAR_BIT == 16
TEST_F(TestMemoryControl, TestWriteSingleAddressNotMultipleOfCharDenied)
{
    const scrutiny::protocol::CommandId::eCommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    uint_least8_t const subfn = static_cast<uint_least8_t>(scrutiny::protocol::MemoryControl::Subfunction::Write);
    const scrutiny::protocol::ResponseCode::eResponseCode invalid = scrutiny::protocol::ResponseCode::InvalidRequest;

    unsigned char data_to_write_8bits[] = { 0x11, 0x22, 0x33, 0x44, 0x55 }; // 2.5 chars to write. not allowed
    unsigned char buffer[] = { 0x0102, 0x0304, 0x0506, 0x0708 };

    // Building request
    SCRUTINY_CONSTEXPR uint32_t addr_size = SIZEOF_8BITS(uintptr_t);
    SCRUTINY_CONSTEXPR uint16_t datalen_req = addr_size + 2 + sizeof(data_to_write_8bits);
    unsigned char request_data[8 + datalen_req] = { 3, 2, 0, datalen_req };
    unsigned int index = 4;
    index += encode_addr(&request_data[index], buffer);
    request_data[index++] = (sizeof(data_to_write_8bits) >> 8) & 0xFF;
    request_data[index++] = (sizeof(data_to_write_8bits) >> 0) & 0xFF;
    std::memcpy(&request_data[index], data_to_write_8bits, sizeof(data_to_write_8bits));
    add_crc(request_data, sizeof(request_data) - 4);

    // Building expected response
    unsigned char tx_buffer[32];
    // Process
    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_GT(n_to_read, 0u);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, invalid);
}
#endif
