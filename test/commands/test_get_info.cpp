//    test_get_info.cpp
//        Test the behaviour of the embedded module when GetInfo commands are received
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "scrutinytest/scrutinytest.hpp"
#include <cstring>

#include "scrutiny.hpp"
#include "scrutiny_base_test.hpp"

class TestGetInfo : public ScrutinyBaseTest
{

  protected:
    scrutiny::Timebase tb;
    scrutiny::MainHandler scrutiny_handler;
    scrutiny::Config config;

    uint8_t _rx_buffer[128];
    uint8_t _tx_buffer[128];

    scrutiny::LoopHandler *loops[3];

    scrutiny::FixedFrequencyLoopHandler fixed_freq_loop;
    scrutiny::VariableFrequencyLoopHandler variable_freq_loop;
    scrutiny::FixedFrequencyLoopHandler fixed_freq_loop_no_datalogging;

    TestGetInfo() :
        ScrutinyBaseTest(),
        tb(),
        scrutiny_handler(),
        config(),
        _rx_buffer(),
        _tx_buffer(),
        loops(),
        fixed_freq_loop(0x12345678, "Loop1"),
        variable_freq_loop("Loop2"),
        fixed_freq_loop_no_datalogging(100, "Loop3")
    {
    }

    virtual void SetUp()
    {

        config.set_buffers(_rx_buffer, sizeof(_rx_buffer), _tx_buffer, sizeof(_tx_buffer));

#if SCRUTINY_ENABLE_DATALOGGING
        fixed_freq_loop_no_datalogging.allow_datalogging(false);
#endif
        loops[0] = &fixed_freq_loop;
        loops[1] = &variable_freq_loop;
        loops[2] = &fixed_freq_loop_no_datalogging;
        config.set_loops(loops, sizeof(loops) / sizeof(loops[0]));

        scrutiny_handler.init(&config);
        scrutiny_handler.comm()->connect();
    }
};

TEST_F(TestGetInfo, TestReadprotocolVersion)
{
    uint8_t request_data[8] = { 1, 1, 0, 0 };
    uint8_t tx_buffer[32];
    uint8_t expected_response[11] = { 0x81, 1, 0, 0, 2, 1, 0 }; // Version 1.0
    add_crc(request_data, sizeof(request_data) - 4);
    add_crc(expected_response, sizeof(expected_response) - 4);
    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);
    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);

    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

TEST_F(TestGetInfo, TestReadSoftwareId)
{
    EXPECT_EQ(sizeof(scrutiny::software_id), SCRUTINY_SOFTWARE_ID_LENGTH);
    uint8_t tx_buffer[SCRUTINY_SOFTWARE_ID_LENGTH + 32];

    // Make request
    uint8_t request_data[8 + SCRUTINY_SOFTWARE_ID_LENGTH] = { 1, 2, 0, 0 };
    request_data[2] = (SCRUTINY_SOFTWARE_ID_LENGTH >> 8) & 0xFF;
    request_data[3] = SCRUTINY_SOFTWARE_ID_LENGTH & 0xFF;
    std::memcpy(&request_data[4], scrutiny::software_id, SCRUTINY_SOFTWARE_ID_LENGTH);
    add_crc(request_data, sizeof(request_data) - 4);

    // Make expected response
    uint8_t expected_response[9 + SCRUTINY_SOFTWARE_ID_LENGTH] = { 0x81, 2, 0 };
    expected_response[3] = (SCRUTINY_SOFTWARE_ID_LENGTH >> 8) & 0xFF;
    expected_response[4] = SCRUTINY_SOFTWARE_ID_LENGTH & 0xFF;
    std::memcpy(&expected_response[5], scrutiny::software_id, SCRUTINY_SOFTWARE_ID_LENGTH);
    add_crc(expected_response, 5 + SCRUTINY_SOFTWARE_ID_LENGTH);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);
    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

/*
    Reads the number of readonly and forbidden memory ranges
*/

TEST_F(TestGetInfo, TestGetSpecialMemoryRegionCount)
{
    uint8_t tx_buffer[32];
    uint8_t *buf[6];

    uintptr_t start = reinterpret_cast<uintptr_t>(buf);
    uintptr_t end = start + 4;
    scrutiny::AddressRange readonly_ranges[] = { scrutiny::tools::make_address_range(start, end),
                                                 scrutiny::tools::make_address_range(start + 1, end + 1) };

    scrutiny::AddressRange forbidden_ranges[] = { scrutiny::tools::make_address_range(start + 2, end + 2) };

    config.set_readonly_address_range(readonly_ranges, sizeof(readonly_ranges) / sizeof(readonly_ranges[0]));
    config.set_forbidden_address_range(forbidden_ranges, sizeof(forbidden_ranges) / sizeof(forbidden_ranges[0]));

    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request
    uint8_t request_data[8] = { 1, 4, 0, 0 };
    add_crc(request_data, sizeof(request_data) - 4);

    // Make expected response
    uint8_t expected_response[9 + 2] = { 0x81, 4, 0, 0, 2, 2, 1 };
    add_crc(expected_response, sizeof(expected_response) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);
    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

/*
    Reads all readonly and forbidden memory range defined in config.
*/

TEST_F(TestGetInfo, TestGetSpecialMemoryRegionLocation)
{
    uint8_t tx_buffer[32];
    uint8_t *buf[4];

    SCRUTINY_CONSTEXPR uint32_t addr_size = sizeof(void *);
    uint64_t start = reinterpret_cast<uint64_t>(buf);
    uint64_t end = start + 4;
    scrutiny::AddressRange readonly_ranges[] = { scrutiny::tools::make_address_range(start, end),
                                                 scrutiny::tools::make_address_range(start + 1, end + 1) };

    scrutiny::AddressRange forbidden_ranges[] = { scrutiny::tools::make_address_range(start + 2, end + 2) };

    config.set_readonly_address_range(readonly_ranges, sizeof(readonly_ranges) / sizeof(readonly_ranges[0]));
    config.set_forbidden_address_range(forbidden_ranges, sizeof(forbidden_ranges) / sizeof(forbidden_ranges[0]));

    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request
    uint8_t request_data[8 + 2] = { 1, 5, 0, 2 };
    uint8_t region_index[] = { 0, 1, 0 };
    uint8_t region_type[] = { 0, 0, 1 };
    // Make expected response
    SCRUTINY_CONSTEXPR uint16_t response_datalen = 2 + addr_size * 2;
    uint8_t expected_response[9 + response_datalen] = {
        0x81,
        5,
        0,
    };
    unsigned int index_resp;
    unsigned int index_req;
    for (uint16_t i = 0; i < 3; i++)
    {
        index_req = 4;
        request_data[index_req++] = region_type[i];
        request_data[index_req++] = region_index[i];
        add_crc(request_data, sizeof(request_data) - 4);

        index_resp = 3;
        expected_response[index_resp++] = (response_datalen >> 8) & 0xFF; // Data length
        expected_response[index_resp++] = (response_datalen >> 0) & 0xFF; // Data length
        expected_response[index_resp++] = region_type[i];                 // Type of memory region. 0 = readonly, 1 = forbidden
        expected_response[index_resp++] = region_index[i];
        index_resp += encode_addr(&expected_response[index_resp], reinterpret_cast<void *>(start + i));
        index_resp += encode_addr(&expected_response[index_resp], reinterpret_cast<void *>(end + i));
        add_crc(expected_response, sizeof(expected_response) - 4);

        scrutiny_handler.receive_data(request_data, sizeof(request_data));
        scrutiny_handler.process(0);

        uint16_t n_to_read = scrutiny_handler.data_to_send();
        ASSERT_LT(n_to_read, sizeof(tx_buffer));
        EXPECT_EQ(n_to_read, sizeof(expected_response));

        uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
        EXPECT_EQ(nread, n_to_read) << "i=" << static_cast<uint32_t>(i);
        ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response)) << "i=" << static_cast<uint32_t>(i);
        scrutiny_handler.process(0);
    }
}

/*
    Request for the location a of readonly memory range that is not set. Expect a failure
*/
TEST_F(TestGetInfo, TestGetSpecialMemoryRegionLocation_WrongIndex)
{
    const scrutiny::protocol::CommandId::E cmd = scrutiny::protocol::CommandId::GetInfo;
    uint8_t const subfn = static_cast<uint8_t>(scrutiny::protocol::GetInfo::Subfunction::GetSpecialMemoryLocation);
    const scrutiny::protocol::ResponseCode::E failure = scrutiny::protocol::ResponseCode::FailureToProceed;

    uint8_t tx_buffer[32];
    uint8_t *buf[4];

    uintptr_t start = reinterpret_cast<uintptr_t>(buf);
    uintptr_t end = start + 4;
    scrutiny::AddressRange readonly_ranges[] = { scrutiny::tools::make_address_range(start, end),
                                                 scrutiny::tools::make_address_range(start + 1, end + 1) };

    scrutiny::AddressRange forbidden_ranges[] = { scrutiny::tools::make_address_range(start + 2, end + 2) };

    config.set_readonly_address_range(readonly_ranges, sizeof(readonly_ranges) / sizeof(readonly_ranges[0]));
    config.set_forbidden_address_range(forbidden_ranges, sizeof(forbidden_ranges) / sizeof(forbidden_ranges[0]));
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request
    uint8_t request_data[8 + 2] = { 1, 5, 0, 2, 0, 2 }; // Oops, index 2 doesn't exist
    add_crc(request_data, sizeof(request_data) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));

    scrutiny_handler.pop_data(tx_buffer, n_to_read);
    ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, failure);
}

/*
    Reads the number of Runtime Published Values
*/

static bool rpv_read_callback(scrutiny::RuntimePublishedValue rpv, scrutiny::AnyType *outval)
{
    static_cast<void>(rpv);
    static_cast<void>(outval);
    // nothing to do here.
    return true;
}

static bool rpv_write_callback(scrutiny::RuntimePublishedValue rpv, scrutiny::AnyType const *inval)
{
    static_cast<void>(rpv);
    static_cast<void>(inval);
    // nothing to do here.
    return true;
}

TEST_F(TestGetInfo, TestGetRPVCount)
{
    uint8_t tx_buffer[32];

    scrutiny::RuntimePublishedValue rpvs[3] = { { 0x1122, scrutiny::VariableType::uint32 },
                                                { 0x3344, scrutiny::VariableType::float32 },
                                                { 0x5566, scrutiny::VariableType::uint16 } };

    config.set_published_values(rpvs, 3, rpv_read_callback, rpv_write_callback);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request
    uint8_t request_data[8] = { 1, 6, 0, 0 };
    add_crc(request_data, sizeof(request_data) - 4);

    // Make expected response
    uint8_t expected_response[9 + 2] = { 0x81, 6, 0, 0, 2, 0, 3 };
    add_crc(expected_response, sizeof(expected_response) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);
    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

/*
    Reads the  Runtime Published Values definitions
*/

TEST_F(TestGetInfo, TestGetRPVDefinition)
{
    uint8_t tx_buffer[64];

    scrutiny::RuntimePublishedValue rpvs[3] = { { 0x1122, scrutiny::VariableType::uint32 },
                                                { 0x3344, scrutiny::VariableType::float32 },
                                                { 0x5566, scrutiny::VariableType::uint16 } };

    config.set_published_values(rpvs, 3, rpv_read_callback, rpv_write_callback);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request
    uint8_t request_data[8 + 4] = { 1, 7, 0, 4, 0, 1, 0, 2 }; // start=1, count =2
    add_crc(request_data, sizeof(request_data) - 4);

    // Make expected response
    uint8_t expected_response[9 + 3 * 2] = { 0x81, 7, 0, 0, 3 * 2 };
    unsigned int index = 5;
    expected_response[index++] = 0x33;
    expected_response[index++] = 0x44;
    expected_response[index++] = static_cast<uint8_t>(scrutiny::VariableType::float32);

    expected_response[index++] = 0x55;
    expected_response[index++] = 0x66;
    expected_response[index++] = static_cast<uint8_t>(scrutiny::VariableType::uint16);

    add_crc(expected_response, sizeof(expected_response) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);
    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

/*
    Make sure that we get an overflow if user tries to read an RPV that is out of bound
*/

TEST_F(TestGetInfo, TestGetRPVDefinitionOverflow)
{
    const scrutiny::protocol::CommandId::E cmd = scrutiny::protocol::CommandId::GetInfo;
    uint8_t const subfn = static_cast<uint8_t>(scrutiny::protocol::GetInfo::Subfunction::GetRuntimePublishedValuesDefinition);
    const scrutiny::protocol::ResponseCode::E failure = scrutiny::protocol::ResponseCode::FailureToProceed;

    uint8_t tx_buffer[32];

    scrutiny::RuntimePublishedValue rpvs[3] = { { 0x1122, scrutiny::VariableType::uint32 },
                                                { 0x3344, scrutiny::VariableType::float32 },
                                                { 0x5566, scrutiny::VariableType::uint16 } };

    config.set_published_values(rpvs, 3, rpv_read_callback, rpv_write_callback);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request
    uint8_t request_data[8 + 4] = { 1, 7, 0, 4, 0, 1, 0, 3 }; // start=1, count =3.  Will overflow
    add_crc(request_data, sizeof(request_data) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));

    scrutiny_handler.pop_data(tx_buffer, n_to_read);
    ASSERT_IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, failure);
}

void dummy_callback(
    uint8_t const subfunction,
    uint8_t const *request_data,
    uint16_t const request_data_length,
    uint8_t *response_data,
    uint16_t *response_data_length,
    uint16_t const response_max_data_length)
{
    static_cast<void>(subfunction);
    static_cast<void>(request_data);
    static_cast<void>(request_data_length);
    static_cast<void>(response_data);
    static_cast<void>(response_data_length);
    static_cast<void>(response_max_data_length);
}

TEST_F(TestGetInfo, TestSupportedFeatures)
{

    for (int i = 0; i < 2; i++)
    {

        config.memory_write_enable = (i == 0) ? true : false;
        config.set_user_command_callback((i == 0) ? SCRUTINY_NULL : dummy_callback);
#if SCRUTINY_ENABLE_DATALOGGING
        uint8_t dl_buffer[128];
        config.set_datalogging_buffers(dl_buffer, sizeof(dl_buffer));
#endif

        scrutiny_handler.init(&config);
        scrutiny_handler.comm()->connect();

        uint8_t tx_buffer[32];

        // Make request
        uint8_t request_data[8] = { 1, 3, 0, 0 };
        add_crc(request_data, sizeof(request_data) - 4);

        // Make expected response
        uint8_t expected_response[9 + 1] = { 0x81, 3, 0, 0, 1 };
        expected_response[5] = 0;

        if (config.memory_write_enable)
        {
            expected_response[5] |= 0x80; // memory write
        }

#if SCRUTINY_ENABLE_DATALOGGING
        expected_response[5] |= 0x40;
#endif

        if (config.is_user_command_callback_set())
        {
            expected_response[5] |= 0x20; // User command
        }

#if SCRUTINY_SUPPORT_64BITS
        expected_response[5] |= 0x10;
#endif

        add_crc(expected_response, sizeof(expected_response) - 4);

        scrutiny_handler.receive_data(request_data, sizeof(request_data));
        scrutiny_handler.process(0);

        uint16_t n_to_read = scrutiny_handler.data_to_send();
        ASSERT_LT(n_to_read, sizeof(tx_buffer)) << "i=" << static_cast<uint32_t>(i);
        EXPECT_EQ(n_to_read, sizeof(expected_response)) << "i=" << static_cast<uint32_t>(i);

        uint16_t nread = scrutiny_handler.pop_data(tx_buffer, n_to_read);
        EXPECT_EQ(nread, n_to_read) << "i=" << static_cast<uint32_t>(i);
        ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response)) << "i=" << static_cast<uint32_t>(i);
    }
}

TEST_F(TestGetInfo, TestGetLoopCount)
{
    uint8_t tx_buffer[32];

    uint8_t request_data[8] = { 1, 8, 0, 0 };
    add_crc(request_data, sizeof(request_data) - 4);

    // Make expected response
    uint8_t expected_response[9 + 1] = { 0x81, 8, 0, 0, 1 };
    expected_response[5] = 3; // 3 loops
    add_crc(expected_response, sizeof(expected_response) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    ASSERT_GT(n_to_read, 0);

    scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

TEST_F(TestGetInfo, TestGetLoopCountNoLoopSet)
{
    scrutiny::Config empty_config;
    empty_config.set_buffers(_tx_buffer, sizeof(_tx_buffer), _rx_buffer, sizeof(_rx_buffer));
    scrutiny_handler.init(&empty_config);
    scrutiny_handler.comm()->connect();

    uint8_t tx_buffer[32];

    uint8_t request_data[8] = { 1, 8, 0, 0 };
    add_crc(request_data, sizeof(request_data) - 4);

    // Make expected response
    uint8_t expected_response[9 + 1] = { 0x81, 8, 0, 0, 1 };
    expected_response[5] = 0; // 2 loops
    add_crc(expected_response, sizeof(expected_response) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    ASSERT_GT(n_to_read, 0);

    scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

TEST_F(TestGetInfo, TestGetLoopDefinitionFixedFreq)
{
    std::string loop_name = "Loop1";
    uint8_t tx_buffer[32];

    uint8_t request_data[8 + 1] = { 1, 9, 0, 1, 0 };
    add_crc(request_data, sizeof(request_data) - 4);

    /*
        ID : 1
        type: 1
        attributes (datalogging bitfield) : 1
        timestep : 4 (only if fixed)
        name length : 1
        name : 0-32 (5)
    */

    // Make expected response
    uint8_t expected_response[9 + 13] = { 0x81, 9, 0, 0, 13 };
    expected_response[5] = 0; // loop ID
    expected_response[6] = static_cast<uint8_t>(scrutiny::LoopType::FIXED_FREQ);
#if SCRUTINY_ENABLE_DATALOGGING
    expected_response[7] = 0x80; // Enabled by default
#else
    expected_response[7] = 0;
#endif
    scrutiny::codecs::encode_32_bits_big_endian(0x12345678u, &expected_response[8]);
    expected_response[12] = static_cast<uint8_t>(loop_name.length());
    scrutiny::tools::strncpy(reinterpret_cast<char *>(&expected_response[13]), loop_name.c_str(), loop_name.size() + 1);
    add_crc(expected_response, sizeof(expected_response) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    ASSERT_GT(n_to_read, 0);

    scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

TEST_F(TestGetInfo, TestGetLoopDefinitionFixedFreqNoDatalogging)
{
    std::string loop_name = "Loop3";
    uint8_t tx_buffer[64];

    uint8_t request_data[8 + 1] = { 1, 9, 0, 1, 2 }; // Read loop 2
    add_crc(request_data, sizeof(request_data) - 4);

    /*
        ID : 1
        type: 1
        attributes (datalogging bitfield) : 1
        timestep : 4 (only if fixed)
        name length : 1
        name : 0-32 (5)
    */

    // Make expected response
    uint8_t expected_response[9 + 13] = { 0x81, 9, 0, 0, 13 };
    expected_response[5] = 2; // loop ID
    expected_response[6] = static_cast<uint8_t>(scrutiny::LoopType::FIXED_FREQ);
    expected_response[7] = 0;
    scrutiny::codecs::encode_32_bits_big_endian(100u, &expected_response[8]);
    expected_response[12] = static_cast<uint8_t>(loop_name.length());
    scrutiny::tools::strncpy(reinterpret_cast<char *>(&expected_response[13]), loop_name.c_str(), loop_name.size() + 1);
    add_crc(expected_response, sizeof(expected_response) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_LE(n_to_read, sizeof(tx_buffer));
    ASSERT_GT(n_to_read, 0);

    scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

TEST_F(TestGetInfo, TestGetLoopDefinitionVariableFreq)
{
    std::string loop_name = "Loop2";
    uint8_t tx_buffer[32];

    uint8_t request_data[8 + 1] = { 1, 9, 0, 1, 1 };
    add_crc(request_data, sizeof(request_data) - 4);

    /*
        ID : 1
        type: 1
        attributes (datalogging bitfield) : 1
        name length : 1
        name : 0-32 (5)
    */

    // Make expected response
    uint8_t expected_response[9 + 9] = { 0x81, 9, 0, 0, 9 };
    expected_response[5] = 1; // loop ID
    expected_response[6] = static_cast<uint8_t>(scrutiny::LoopType::VARIABLE_FREQ);
#if SCRUTINY_ENABLE_DATALOGGING
    expected_response[7] = 0x80; // Enabled by default
#else
    expected_response[7] = 0;
#endif
    expected_response[8] = static_cast<uint8_t>(loop_name.length());

    scrutiny::tools::strncpy(reinterpret_cast<char *>(&expected_response[9]), loop_name.c_str(), loop_name.size() + 1);
    add_crc(expected_response, sizeof(expected_response) - 4);

    scrutiny_handler.receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    ASSERT_GT(n_to_read, 0);

    scrutiny_handler.pop_data(tx_buffer, n_to_read);
    EXPECT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}
