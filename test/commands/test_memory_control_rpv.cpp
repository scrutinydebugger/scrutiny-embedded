//    test_memory_control.cpp
//        Test the behaviour of the embedded module when MemoryControl commands are received
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include <gtest/gtest.h>
#include "scrutiny.hpp"
#include "scrutiny_test.hpp"
#include <cstring>

#include <map>
#include <vector>
#include <tuple>

class TestMemoryControlRPV : public ScrutinyTest
{
protected:
    scrutiny::Timebase tb;
    scrutiny::MainHandler scrutiny_handler;
    scrutiny::Config config;

    uint8_t _rx_buffer[128];
    uint8_t _tx_buffer[128];

    virtual void SetUp()
    {
        config.set_buffers(_rx_buffer, sizeof(_rx_buffer), _tx_buffer, sizeof(_tx_buffer));
        scrutiny_handler.init(&config);
        scrutiny_handler.comm()->connect();
    }
};


// ==== Test Runtime Published Valuers
static bool rpv_read_callback(scrutiny::RuntimePublishedValue rpv, scrutiny::AnyType* outval)
{
    uint32_t v1 = 0x12345678;
    float v2 = 1.34f;
    uint16_t v3 = 0xabcd;

    if (rpv.id == 0x1122 && rpv.type == scrutiny::VariableType::uint32)
    {
        outval->uint32 = v1;
    }
    else if (rpv.id == 0x3344 && rpv.type == scrutiny::VariableType::float32)
    {
        outval->float32 = v2;
    }
    else if (rpv.id == 0x5566 && rpv.type == scrutiny::VariableType::uint16)
    {
        outval->uint16 = v3;
    }
    
    else if (rpv.id == 0x9000 && rpv.type == scrutiny::VariableType::uint8)
    {
        outval->uint8 = 0x99;
    }
    else if (rpv.id == 0x9001 && rpv.type == scrutiny::VariableType::uint16)
    {
        outval->uint16 = 0xA5A5;
    }
    else if (rpv.id == 0x9002 && rpv.type == scrutiny::VariableType::uint32)
    {
        outval->uint32 = 0x99887766;
    }
    else if (rpv.id == 0x9003 && rpv.type == scrutiny::VariableType::uint64)
    {
        outval->uint64 = 0xFFFF111122223333;
    }
    else if (rpv.id == 0x9004 && rpv.type == scrutiny::VariableType::sint8)
    {
        outval->sint8 = -64;
    }
    else if (rpv.id == 0x9005 && rpv.type == scrutiny::VariableType::sint16)
    {
        outval->sint16 = -30000;
    }
    else if (rpv.id == 0x9006 && rpv.type == scrutiny::VariableType::sint32)
    {
        outval->sint32 = -1000000000;
    }
    else if (rpv.id == 0x9007 && rpv.type == scrutiny::VariableType::sint64)
    {
        outval->sint64 = -90000000000;
    }
    else if (rpv.id == 0x9008 && rpv.type == scrutiny::VariableType::float32)
    {
        outval->float32 = 1.45f;
    }
    else if (rpv.id == 0x9009 && rpv.type == scrutiny::VariableType::float64)
    {
        outval->float64 = -5.123L;
    }

    else
    {
        return false;
    }

    return true;
}


struct AllTypeResult {

    void clear()
    {
        some_u8 = 0;
        some_u16 = 0;
        some_u32 = 0;
        some_u64 = 0;
        some_s8 = 0;
        some_s16 = 0;
        some_s32 = 0;
        some_s64 = 0;
        some_f32 = 0;
        some_f64 = 0;
    }

    uint8_t some_u8;
    uint16_t some_u16;
    uint32_t some_u32;
    uint64_t some_u64;

    int8_t some_s8;
    int16_t some_s16;
    int32_t some_s32;
    int64_t some_s64;

    float some_f32;
    double some_f64;
};
static AllTypeResult dest_buffer_for_rpv_write;

static bool rpv_write_callback(const scrutiny::RuntimePublishedValue rpv, const scrutiny::AnyType* inval)
{
     if (rpv.id == 0x1000 && rpv.type == scrutiny::VariableType::uint8)
     {
        dest_buffer_for_rpv_write.some_u8 = inval->uint8;
     }
     else if (rpv.id == 0x1001 && rpv.type == scrutiny::VariableType::uint16)
     {
        dest_buffer_for_rpv_write.some_u16 = inval->uint16;
     }
     else if (rpv.id == 0x1002 && rpv.type == scrutiny::VariableType::uint32)
     {
        dest_buffer_for_rpv_write.some_u32 = inval->uint32;
     }
     else if (rpv.id == 0x1003 && rpv.type == scrutiny::VariableType::uint64)
     {
        dest_buffer_for_rpv_write.some_u64 = inval->uint64;
     }

     else if (rpv.id == 0x1004 && rpv.type == scrutiny::VariableType::sint8)
     {
        dest_buffer_for_rpv_write.some_s8 = inval->sint8;
     }
     else if (rpv.id == 0x1005 && rpv.type == scrutiny::VariableType::sint16)
     {
        dest_buffer_for_rpv_write.some_s16 = inval->sint16;
     }
     else if (rpv.id == 0x1006 && rpv.type == scrutiny::VariableType::sint32)
     {
        dest_buffer_for_rpv_write.some_s32 = inval->sint32;
     }
     else if (rpv.id == 0x1007 && rpv.type == scrutiny::VariableType::sint64)
     {
        dest_buffer_for_rpv_write.some_s64 = inval->sint64;
     }

     else if (rpv.id == 0x1008 && rpv.type == scrutiny::VariableType::float32)
     {
        dest_buffer_for_rpv_write.some_f32 = inval->float32;
     }
     else if (rpv.id == 0x1009 && rpv.type == scrutiny::VariableType::float64)
     {
        dest_buffer_for_rpv_write.some_f64 = inval->float64;
     }
     else
     {
        return false;
     }
     return true;
}


/*
    Try to read a single RPV. Validate that we receive the right value
*/
TEST_F(TestMemoryControlRPV, TestReadSingleRPV)
{
    uint8_t tx_buffer[32];

    scrutiny::RuntimePublishedValue rpvs[3] = {
        {0x1122, scrutiny::VariableType::uint32},
        {0x3344, scrutiny::VariableType::float32},
        {0x5566, scrutiny::VariableType::uint16}
    };

    config.set_published_values(rpvs, sizeof(rpvs)/sizeof(rpvs[0]), rpv_read_callback);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request
    uint8_t request_data[8 + 2] = { 3,4,0,2, 0x11, 0x22 };
    add_crc(request_data, sizeof(request_data) - 4);

    // Make expected response
    uint8_t expected_response[9 + 2+4] = { 0x83, 4, 0, 0, 2+4, 0x11, 0x22, 0x12, 0x34, 0x56, 0x78};
    add_crc(expected_response, sizeof(expected_response) -4);

    scrutiny_handler.comm()->receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);
    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}

/*
    Try to read 3 RPV. Validate that we receive the right value for each of them
*/

TEST_F(TestMemoryControlRPV, TestReadMultipleRPV)
{
    uint8_t tx_buffer[32];

    scrutiny::RuntimePublishedValue rpvs[3] = {
        {0x1122, scrutiny::VariableType::uint32},
        {0x3344, scrutiny::VariableType::float32},
        {0x5566, scrutiny::VariableType::uint16}
    };

    config.set_published_values(rpvs, sizeof(rpvs)/sizeof(rpvs[0]), rpv_read_callback);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request
    uint8_t request_data[8 + 6] = { 3,4,0,6, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    add_crc(request_data, sizeof(request_data) - 4);

    // Make expected response
    // 0x3fab851f = 1.34 in float
    // ID1, value1, ID2, value2, Id3, value3
    uint8_t expected_response[9 + 6+4+4+2] = { 0x83, 4, 0, 0, 6+4+4+2, 0x11, 0x22, 0x12, 0x34, 0x56, 0x78, 0x33, 0x44, 0x3f, 0xab, 0x85, 0x1f, 0x55, 0x66, 0xab, 0xcd};
    add_crc(expected_response, sizeof(expected_response) -4);

    scrutiny_handler.comm()->receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);
    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));
}


/*
    Try to read a RPV of each type. Validate encoding is good
*/

TEST_F(TestMemoryControlRPV, TestReadMultipleRPVEachType)
{
    uint8_t tx_buffer[128];

    scrutiny::RuntimePublishedValue rpvs[] = {
        {0x9000, scrutiny::VariableType::uint8},
        {0x9001, scrutiny::VariableType::uint16},
        {0x9002, scrutiny::VariableType::uint32},
        {0x9003, scrutiny::VariableType::uint64},
        {0x9004, scrutiny::VariableType::sint8},
        {0x9005, scrutiny::VariableType::sint16},
        {0x9006, scrutiny::VariableType::sint32},
        {0x9007, scrutiny::VariableType::sint64},
        {0x9008, scrutiny::VariableType::float32},
        {0x9009, scrutiny::VariableType::float64}
    };

    std::map<uint16_t, std::vector<uint8_t>> expected_encoding;
    expected_encoding.insert({ 0x9000, {0x99} });
    expected_encoding.insert({ 0x9001, {0xA5, 0xA5} });
    expected_encoding.insert({ 0x9002, {0x99, 0x88, 0x77, 0x66} });
    expected_encoding.insert({ 0x9003, {0xFF, 0xFF, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33} });

    expected_encoding.insert({ 0x9004, {0xC0} });   // -64
    expected_encoding.insert({ 0x9005, {0x8A, 0xD0} }); // -30000
    expected_encoding.insert({ 0x9006, {0xC4, 0x65, 0x36, 0x00} }); // -1 000 000 000
    expected_encoding.insert({ 0x9007, {0xff, 0xff, 0xff, 0xeb, 0x0b, 0x94, 0xfc, 0x00} }); // -90 000 000 000

    expected_encoding.insert({ 0x9008, {0x3f, 0xb9, 0x99, 0x9a} }); // 1.45
    expected_encoding.insert({ 0x9009, {0xc0, 0x14, 0x7d, 0xf3, 0xb6, 0x45, 0xa1, 0xcb} });   // -5.123


    config.set_published_values(rpvs, sizeof(rpvs) / sizeof(rpvs[0]), rpv_read_callback);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();
    const uint16_t nb_vals = static_cast<uint16_t>(expected_encoding.size());
    // Make request
    uint16_t request_buffer_size = 8 + nb_vals * 2;
    uint8_t* request_data = new uint8_t[request_buffer_size];
    request_data[0] = 3;    // command
    request_data[1] = 4;    // subcommand
    request_data[2] = ((nb_vals * 2) >> 8) & 0xFF;    // length msb
    request_data[3] = ((nb_vals * 2) & 0xFF);    // length lsb
    unsigned int index = 4;
    for (auto p = expected_encoding.begin(); p != expected_encoding.end(); ++p)
    {
        const uint16_t id = p->first;
        request_data[index++] = (id >> 8) & 0xFF;
        request_data[index++] = (id) & 0xFF;
    }
    add_crc(request_data, request_buffer_size - 4);

    scrutiny_handler.comm()->receive_data(request_data, request_buffer_size);
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));

    uint16_t nread = scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);

    index = 5;
    for (auto p = expected_encoding.begin(); p != expected_encoding.end(); ++p)
    {
        const uint16_t id = p->first;
        ASSERT_LE(index, static_cast<unsigned int>(nread - 2));    // safety check

        EXPECT_EQ(tx_buffer[index], (id >> 8) & 0xFF) << "index=" << index << ", ID = " << id;
        EXPECT_EQ(tx_buffer[index+1], id & 0xFF) << "index=" << index << ", ID = " << id;
        index += 2;
        ASSERT_LE(index, nread-p->second.size());    // safety check
        for (unsigned int n=0; n<p->second.size(); n++)
        {
            EXPECT_EQ(tx_buffer[index], p->second[n]) << "index=" << index << ", ID=" << id << "/Bytes #" << n;
            index++;
        }
    }
}


/*
    Send a read request with invalid data length. Expect an invalidRequest response
*/

TEST_F(TestMemoryControlRPV, TestReadRPVBadRequest)
{
    const scrutiny::protocol::CommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    const uint8_t subfn = static_cast<uint8_t>(scrutiny::protocol::MemoryControl::Subfunction::ReadRPV);
    const scrutiny::protocol::ResponseCode invalid = scrutiny::protocol::ResponseCode::InvalidRequest;

    uint8_t tx_buffer[32];

    scrutiny::RuntimePublishedValue rpvs[3] = {
        {0x1122, scrutiny::VariableType::uint32},
        {0x3344, scrutiny::VariableType::float32},
        {0x5566, scrutiny::VariableType::uint16}
    };

    config.set_published_values(rpvs, sizeof(rpvs)/sizeof(rpvs[0]), rpv_read_callback);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request
    uint8_t request_data[8 + 3] = { 3,4,0,3, 0x11, 0x22, 0x33 };    // Incomplete ID
    add_crc(request_data, sizeof(request_data) - 4);

    scrutiny_handler.comm()->receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);

    ASSERT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, invalid));

    scrutiny_handler.process(0);
}

/*
    Send a read request with an non-existing RPV ID. Expect a FailureToProceed
*/

TEST_F(TestMemoryControlRPV, TestReadRPVNonExistingID)
{
    const scrutiny::protocol::CommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    const uint8_t subfn = static_cast<uint8_t>(scrutiny::protocol::MemoryControl::Subfunction::ReadRPV);
    const scrutiny::protocol::ResponseCode failure = scrutiny::protocol::ResponseCode::FailureToProceed;

    uint8_t tx_buffer[32];

    scrutiny::RuntimePublishedValue rpvs[3] = {
        {0x1122, scrutiny::VariableType::uint32},
        {0x3344, scrutiny::VariableType::float32},
        {0x5566, scrutiny::VariableType::uint16}
    };

    config.set_published_values(rpvs, sizeof(rpvs)/sizeof(rpvs[0]), rpv_read_callback);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request
    uint8_t request_data[8 + 4] = { 3,4,0,4, 0x11, 0x22, 0x99, 0x99 };    // 0x9999 doesn't exists
    add_crc(request_data, sizeof(request_data) - 4);

    scrutiny_handler.comm()->receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);

    ASSERT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, failure));

    scrutiny_handler.process(0);
}

/*
    Send a read request that cause a response biuger than tx buffer. Expects overflow response
*/

TEST_F(TestMemoryControlRPV, TestReadRPVResponseOverflow)
{
    const scrutiny::protocol::CommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    const uint8_t subfn = static_cast<uint8_t>(scrutiny::protocol::MemoryControl::Subfunction::ReadRPV);
    const scrutiny::protocol::ResponseCode overflow = scrutiny::protocol::ResponseCode::Overflow;

    uint8_t tx_buffer[32];

    scrutiny::RuntimePublishedValue rpvs[3] = {
        {0x1122, scrutiny::VariableType::uint32},
        {0x3344, scrutiny::VariableType::float32},
        {0x5566, scrutiny::VariableType::uint16}
    };

    config.set_published_values(rpvs, sizeof(rpvs)/sizeof(rpvs[0]), rpv_read_callback);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    uint16_t bufsize = scrutiny_handler.comm()->tx_buffer_size();
    uint16_t nbrpv = bufsize/6+1;   // Will cause overflow. Do not hceck overhead. Buffer store data only
    uint16_t request_buffer_size = nbrpv*2 + 8;
    uint8_t* request_data = new uint8_t[request_buffer_size];
    request_data[0] = 3;    // command
    request_data[1] = 4;    // subcommand
    request_data[2] = ((nbrpv*2) >> 8)&0xFF;    // length msb
    request_data[3] = ((nbrpv*2) & 0xFF);    // length lsb

    for (uint16_t i=0; i<nbrpv; i++)
    {
        request_data[4+2*i] = 0x11;
        request_data[4+2*i+1] = 0x22;
    }
    // Make request
    add_crc(request_data, request_buffer_size-4);

    scrutiny_handler.comm()->receive_data(request_data, request_buffer_size);
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);

    ASSERT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, overflow));

    scrutiny_handler.process(0);
}

//=================================================
//==================  WRITE  ======================
//=================================================


/*
    Try to write a single RPV. Validate that the data reach the destination intact
*/
TEST_F(TestMemoryControlRPV, TestWriteSingleRPV)
{
    dest_buffer_for_rpv_write.clear();
    uint8_t tx_buffer[32];

    scrutiny::RuntimePublishedValue rpvs[] = {
        {0x1000, scrutiny::VariableType::uint8},
        {0x1001, scrutiny::VariableType::uint16},
        {0x1002, scrutiny::VariableType::uint32},
        {0x1003, scrutiny::VariableType::uint64},
        {0x1004, scrutiny::VariableType::sint8},
        {0x1005, scrutiny::VariableType::sint16},
        {0x1006, scrutiny::VariableType::sint32},
        {0x1007, scrutiny::VariableType::sint64},
        {0x1008, scrutiny::VariableType::float32},
        {0x1009, scrutiny::VariableType::float64}
    };

    config.set_published_values(rpvs, sizeof(rpvs)/sizeof(rpvs[0]), nullptr, rpv_write_callback);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request
    uint8_t request_data[8 + 2 + 4] = { 3,5,0,6, 0x10, 0x02, 0x11, 0x22, 0x33, 0x44 };
    add_crc(request_data, sizeof(request_data) - 4);

    // Make expected response
    uint8_t expected_response[9 + 2 + 1] = { 0x83, 5, 0, 0, 3, 0x10, 0x02, 4}; // id + length
    add_crc(expected_response, sizeof(expected_response) -4);

    scrutiny_handler.comm()->receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);
    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));

    EXPECT_EQ(dest_buffer_for_rpv_write.some_u32, 0x11223344u);
}


/*
    Try to write a multiple RPV. Validate that the data reach the destination intact
*/
TEST_F(TestMemoryControlRPV, TestWriteMultipleRPV)
{
    dest_buffer_for_rpv_write.clear();
    uint8_t tx_buffer[32];

    scrutiny::RuntimePublishedValue rpvs[] = {
        {0x1000, scrutiny::VariableType::uint8},
        {0x1001, scrutiny::VariableType::uint16},
        {0x1002, scrutiny::VariableType::uint32},
        {0x1003, scrutiny::VariableType::uint64},
        {0x1004, scrutiny::VariableType::sint8},
        {0x1005, scrutiny::VariableType::sint16},
        {0x1006, scrutiny::VariableType::sint32},
        {0x1007, scrutiny::VariableType::sint64},
        {0x1008, scrutiny::VariableType::float32},
        {0x1009, scrutiny::VariableType::float64}
    };

    config.set_published_values(rpvs, sizeof(rpvs)/sizeof(rpvs[0]), nullptr, rpv_write_callback);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request
    // 0x1002 = 0x11223344, 0x1006 = -1234567890
    uint8_t request_data[8 + (2+4)*2] = { 3,5,0,(2+4)*2, 0x10, 0x02, 0x11, 0x22, 0x33, 0x44, 0x10, 0x06, 0xB6, 0x69, 0xFD, 0x2E};
    add_crc(request_data, sizeof(request_data) - 4);

    // Make expected response
    uint8_t expected_response[9 + 6] = { 0x83, 5, 0, 0, (2+1)*2, 0x10, 0x02, 4, 0x10, 0x06, 4};
    add_crc(expected_response, sizeof(expected_response) -4);

    scrutiny_handler.comm()->receive_data(request_data, sizeof(request_data));
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    EXPECT_EQ(n_to_read, sizeof(expected_response));

    uint16_t nread = scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);
    ASSERT_BUF_EQ(tx_buffer, expected_response, sizeof(expected_response));


    EXPECT_EQ(dest_buffer_for_rpv_write.some_u32, 0x11223344u);
    EXPECT_EQ(dest_buffer_for_rpv_write.some_s32, -1234567890);
}


/*
    Try to write multiple RPV, on of each type. Validate that the data reach the destination intact
*/
TEST_F(TestMemoryControlRPV, TestWriteAllTypes)
{
    dest_buffer_for_rpv_write.clear();

    scrutiny::RuntimePublishedValue rpvs[] = {
        {0x1000, scrutiny::VariableType::uint8},
        {0x1001, scrutiny::VariableType::uint16},
        {0x1002, scrutiny::VariableType::uint32},
        {0x1003, scrutiny::VariableType::uint64},
        {0x1004, scrutiny::VariableType::sint8},
        {0x1005, scrutiny::VariableType::sint16},
        {0x1006, scrutiny::VariableType::sint32},
        {0x1007, scrutiny::VariableType::sint64},
        {0x1008, scrutiny::VariableType::float32},
        {0x1009, scrutiny::VariableType::float64}
    };

    struct TestEntry
    {
        uint16_t id;
        scrutiny::VariableType type;
        std::vector<uint8_t> data;
    };
    
    std::vector<TestEntry> vals_and_payload;

    vals_and_payload.push_back({ 0x1000, scrutiny::VariableType::uint8, {0x55}}); // 4-6
    vals_and_payload.push_back({ 0x1001, scrutiny::VariableType::uint16, {0xAB, 0xCD}}); //7-10
    vals_and_payload.push_back({ 0x1002, scrutiny::VariableType::uint32, {0x12, 0x34, 0x56, 0x78}}); // 11-16
    vals_and_payload.push_back({ 0x1003, scrutiny::VariableType::uint64, {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10}}); // 17-24
    vals_and_payload.push_back({ 0x1004, scrutiny::VariableType::sint8, {0xC0}});   // 25-27
    vals_and_payload.push_back({ 0x1005, scrutiny::VariableType::sint16, {0x8A, 0xD0}});    // 28 - 31
    vals_and_payload.push_back({ 0x1006, scrutiny::VariableType::sint32, {0xC4, 0x65, 0x36, 0x00}}); // 32 - 37
    vals_and_payload.push_back({ 0x1007, scrutiny::VariableType::sint64, {0xff, 0xff, 0xff, 0xeb, 0x0b, 0x94, 0xfc, 0x00}});    // 38 - 47
    vals_and_payload.push_back({ 0x1008, scrutiny::VariableType::sint32, {0x3f, 0xb9, 0x99, 0x9a}});    // 48 - 53
    vals_and_payload.push_back({ 0x1009, scrutiny::VariableType::sint64, {0xc0, 0x14, 0x7d, 0xf3, 0xb6, 0x45, 0xa1, 0xcb}});    // 54 - 63


    config.set_published_values(rpvs, sizeof(rpvs)/sizeof(rpvs[0]), nullptr, rpv_write_callback);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request
    uint16_t required_request_size = 8; // request overhead
    uint16_t required_response_size = 9; // request overhead

    for (unsigned int i=0; i < vals_and_payload.size(); i++)
    {
        required_request_size += 2 + scrutiny::tools::get_type_size(vals_and_payload[i].type);
        required_response_size += 2 + 1;    // id (2) + len (1)
    }

    uint8_t *request_data = new uint8_t[required_request_size];
    uint8_t *expected_response = new uint8_t[required_response_size];
    uint8_t *tx_buffer = new uint8_t[required_response_size];
    uint16_t request_index = 4;
    uint16_t response_index = 5;

    request_data[0] = 3;
    request_data[1] = 5;
    request_data[2] = ((required_request_size-8) >> 8) & 0xFF ;
    request_data[3] = (required_request_size -8) & 0xFF;

    expected_response[0] = 0x83;
    expected_response[1] = 5;
    expected_response[2] = 0;
    expected_response[3] = ((required_response_size-9)>>8)&0xFF;
    expected_response[4] = (required_response_size-9) & 0xFF;

    for (unsigned int i=0; i < vals_and_payload.size(); i++)
    {
        request_data[request_index++] = (vals_and_payload[i].id >> 8) & 0xFF;
        request_data[request_index++] = (vals_and_payload[i].id) & 0xFF;
        for (unsigned int j=0; j<vals_and_payload[i].data.size(); j++)
        {
            request_data[request_index++] = vals_and_payload[i].data[j];
        }

        expected_response[response_index++] = (vals_and_payload[i].id >> 8) & 0xFF;
        expected_response[response_index++] = (vals_and_payload[i].id) & 0xFF;
        expected_response[response_index++] = static_cast<uint8_t>(vals_and_payload[i].data.size());
    }

    add_crc(request_data, required_request_size - 4);
    add_crc(expected_response, required_response_size - 4);

    scrutiny_handler.comm()->receive_data(request_data, required_request_size);
    scrutiny_handler.process(0);

    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    EXPECT_EQ(n_to_read, required_response_size);

    uint16_t nread = scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    EXPECT_EQ(nread, n_to_read);
    ASSERT_BUF_EQ(tx_buffer, expected_response, required_response_size);

    EXPECT_EQ(dest_buffer_for_rpv_write.some_u8, 0x55u);
    EXPECT_EQ(dest_buffer_for_rpv_write.some_u16, 0xABCDu);
    EXPECT_EQ(dest_buffer_for_rpv_write.some_u32, 0x12345678u);
    EXPECT_EQ(dest_buffer_for_rpv_write.some_u64, 0xfedcba9876543210u);
    EXPECT_EQ(dest_buffer_for_rpv_write.some_s8, -64);
    EXPECT_EQ(dest_buffer_for_rpv_write.some_s16, -30000);
    EXPECT_EQ(dest_buffer_for_rpv_write.some_s32, -1000000000);
    EXPECT_EQ(dest_buffer_for_rpv_write.some_s64, -90000000000);
    EXPECT_EQ(dest_buffer_for_rpv_write.some_f32, 1.45f);
    EXPECT_EQ(dest_buffer_for_rpv_write.some_f64, -5.123);
}


/*
    Send a write request with invalid data length. Expect an invalidRequest response
*/

TEST_F(TestMemoryControlRPV, TestWriteRPVBadRequest)
{
    const scrutiny::protocol::CommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    const uint8_t subfn = static_cast<uint8_t>(scrutiny::protocol::MemoryControl::Subfunction::WriteRPV);
    const scrutiny::protocol::ResponseCode invalid = scrutiny::protocol::ResponseCode::InvalidRequest;

    uint8_t tx_buffer[32];

     scrutiny::RuntimePublishedValue rpvs[] = {
        {0x1000, scrutiny::VariableType::uint8},
        {0x1001, scrutiny::VariableType::uint16},
        {0x1002, scrutiny::VariableType::uint32},
        {0x1003, scrutiny::VariableType::uint64},
        {0x1004, scrutiny::VariableType::sint8},
        {0x1005, scrutiny::VariableType::sint16},
        {0x1006, scrutiny::VariableType::sint32},
        {0x1007, scrutiny::VariableType::sint64},
        {0x1008, scrutiny::VariableType::float32},
        {0x1009, scrutiny::VariableType::float64}
    };

    config.set_published_values(rpvs, sizeof(rpvs)/sizeof(rpvs[0]), nullptr, rpv_write_callback);
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request

    uint8_t fullrequest_no_crc[4 + 2+4] = { 3,5,0, 0, 0x10, 0x02,  0x11, 0x22, 0x33, 0x44 }; 

    for (uint8_t i=1; i<6-1; i++)  // For all size : 1,2,3,4,5 we expect a failure. 0 and 6 would be the correct size
    {
        uint16_t request_size = 8 + i;
        uint8_t* request_data = new uint8_t[request_size];
        std::memcpy(request_data, fullrequest_no_crc, request_size-4);
        request_data[3] = i;
        add_crc(request_data, 4+i);
        
        scrutiny_handler.comm()->receive_data(request_data, request_size);
        scrutiny_handler.process(0);

        uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
        ASSERT_LT(n_to_read, sizeof(tx_buffer)) << " i=" << i;
        ASSERT_EQ(n_to_read, 9u) << " i=" << i;
        scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
        scrutiny_handler.process(0);

        ASSERT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, invalid)) << " i=" << i ;
        delete[] request_data;
    }
}


/*
    Write so many RPV that the response that confirms which RPV is written doesn't fit the TX buffer
    This requires a Rx buffer bigger than the Tx buffer (unlikely)
*/

TEST_F(TestMemoryControlRPV, TestWriteRPVResponseOverflow)
{
    constexpr scrutiny::protocol::CommandId cmd = scrutiny::protocol::CommandId::MemoryControl;
    constexpr uint8_t subfn = static_cast<uint8_t>(scrutiny::protocol::MemoryControl::Subfunction::WriteRPV);
    constexpr scrutiny::protocol::ResponseCode overflow = scrutiny::protocol::ResponseCode::Overflow;

    constexpr uint16_t buffersize_min_mod3 = ((scrutiny::protocol::MINIMUM_TX_BUFFER_SIZE + 2) / 3) * 3;
    constexpr uint16_t nb_write = buffersize_min_mod3/3+1;   //16bits ID + 16bits data
    constexpr uint16_t rx_data_size = nb_write*3;       // 16bits ID + 8bits data
    constexpr uint16_t tx_data_size = (nb_write-1)*3;   // 16bits ID + 8bits length.  -1 to cause overflow
    constexpr uint16_t request_size = rx_data_size + scrutiny::protocol::REQUEST_OVERHEAD;

    static_assert(rx_data_size >= scrutiny::protocol::MINIMUM_RX_BUFFER_SIZE, "Buffer size doesn't match minimum size");
    static_assert(tx_data_size >= scrutiny::protocol::MINIMUM_TX_BUFFER_SIZE, "Buffer size doesn't match minimum size");
    static_assert(buffersize_min_mod3 % 3 == 0, "This should be a multiple of 3");

    uint8_t internal_rx_buffer[rx_data_size];
    uint8_t internal_tx_buffer[tx_data_size];

    scrutiny::RuntimePublishedValue rpvs[] = {
       {0x1000, scrutiny::VariableType::uint8}
    };

    config.set_published_values(rpvs, sizeof(rpvs) / sizeof(rpvs[0]), nullptr, rpv_write_callback);
    config.set_buffers(internal_rx_buffer, sizeof(internal_rx_buffer), internal_tx_buffer, sizeof(internal_tx_buffer));
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    // Make request
    
    uint8_t request_data[request_size] = {3, 5, ((rx_data_size) >> 8) & 0xFF, (rx_data_size) & 0xFF };

    uint16_t index = 4;
    for (uint16_t i = 0; i < nb_write; i++)
    {
        request_data[index++] = 0x10;
        request_data[index++] = 0x00;
        request_data[index++] = 0xAA;
    }

    add_crc(request_data, request_size - 4);

    scrutiny_handler.comm()->receive_data(request_data, request_size);
    scrutiny_handler.process(0);

    uint8_t tx_buffer[32];
    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_EQ(n_to_read, 9u);
    ASSERT_LT(n_to_read, sizeof(tx_buffer));
    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    scrutiny_handler.process(0);

    ASSERT_TRUE(IS_PROTOCOL_RESPONSE(tx_buffer, cmd, subfn, overflow));
}

/*
    Write so many RPV that the response that confirms which RPV is written completely fills the TX buffer without overflow
    This requires a Rx buffer bigger than the Tx buffer (unlikely)
*/
TEST_F(TestMemoryControlRPV, TestWriteRPVResponseFullNoOverflow)
{
    constexpr uint16_t buffersize_min_mod3 = ((scrutiny::protocol::MINIMUM_TX_BUFFER_SIZE + 2) / 3) * 3;
    constexpr uint16_t nb_write = buffersize_min_mod3 / 3;   //16bits ID + 16bits data
    constexpr uint16_t rx_data_size = nb_write * 3;       // 16bits ID + 8bits data
    constexpr uint16_t tx_data_size = nb_write * 3;   // 16bits ID + 8bits length.
    constexpr uint16_t request_size = rx_data_size + scrutiny::protocol::REQUEST_OVERHEAD;
    constexpr uint16_t response_size = tx_data_size + scrutiny::protocol::RESPONSE_OVERHEAD;

    static_assert(rx_data_size >= scrutiny::protocol::MINIMUM_RX_BUFFER_SIZE, "Buffer size doesn't match minimum size");
    static_assert(tx_data_size >= scrutiny::protocol::MINIMUM_TX_BUFFER_SIZE, "Buffer size doesn't match minimum size");
    static_assert(buffersize_min_mod3 % 3 == 0, "This should be a multiple of 3");

    uint8_t internal_rx_buffer[rx_data_size];
    uint8_t internal_tx_buffer[tx_data_size];

    scrutiny::RuntimePublishedValue rpvs[] = {
       {0x1000, scrutiny::VariableType::uint8}
    };

    config.set_published_values(rpvs, sizeof(rpvs) / sizeof(rpvs[0]), nullptr, rpv_write_callback);
    config.set_buffers(internal_rx_buffer, sizeof(internal_rx_buffer), internal_tx_buffer, sizeof(internal_tx_buffer));
    scrutiny_handler.init(&config);
    scrutiny_handler.comm()->connect();

    uint8_t request_data[request_size] = { 3,5, ((rx_data_size) >> 8) & 0xFF, (rx_data_size) & 0xFF };
    uint8_t expected_response[response_size] = { 0x83, 5, 0, ((tx_data_size) >> 8) & 0xFF, (tx_data_size) & 0xFF };

    uint16_t rx_index = 4;
    uint16_t tx_index = 5;
    for (uint16_t i = 0; i < nb_write; i++)
    {
        request_data[rx_index++] = 0x10;    // ID
        request_data[rx_index++] = 0x00;    // ID
        request_data[rx_index++] = 0xAA;    // Data

        expected_response[tx_index++] = 0x10;   // ID
        expected_response[tx_index++] = 0x00;   // ID
        expected_response[tx_index++] = 1;      // length
    }

    add_crc(request_data, request_size - 4);
    add_crc(expected_response, response_size - 4);

    scrutiny_handler.comm()->receive_data(request_data, request_size);
    scrutiny_handler.process(0);

    uint8_t tx_buffer[response_size];
    uint16_t n_to_read = scrutiny_handler.comm()->data_to_send();
    ASSERT_EQ(n_to_read, sizeof(tx_buffer));
    scrutiny_handler.comm()->pop_data(tx_buffer, n_to_read);
    scrutiny_handler.process(0);

    ASSERT_BUF_EQ(tx_buffer, expected_response, response_size);
}
