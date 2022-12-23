#include <gtest/gtest.h>

#include "scrutiny_test.hpp"
#include "scrutiny.hpp"

#if SCRUTINY_DATALOGGING_ENCODING == SCRUTINY_DATALOGGING_ENCODING_RAW

using namespace scrutiny;

class TestRawEncoder : public ScrutinyTest
{
public:
    TestRawEncoder() : ScrutinyTest(),
                       encoder(&scrutiny_handler, dlbuffer, sizeof(dlbuffer), &dlconfig)
    {
    }

protected:
    void check_canaries();

    MainHandler scrutiny_handler;
    Config config;
    datalogging::Configuration dlconfig;
    datalogging::RawFormatEncoder encoder;

    uint8_t _rx_buffer[128];
    uint8_t _tx_buffer[128];

    uint8_t buffer_canary_1[512];
    uint8_t dlbuffer[128];
    uint8_t buffer_canary_2[512];

    virtual void SetUp()
    {
        config.set_buffers(_rx_buffer, sizeof(_rx_buffer), _tx_buffer, sizeof(_tx_buffer));
        scrutiny_handler.init(&config);

        memset(buffer_canary_1, 0xAA, sizeof(buffer_canary_1));
        memset(buffer_canary_2, 0x55, sizeof(buffer_canary_2));
    }
};

void TestRawEncoder::check_canaries()
{
    for (size_t i = 0; i < sizeof(buffer_canary_1); i++)
    {
        ASSERT_EQ(buffer_canary_1[i], 0xAA) << "Overflow before buffer at i=" << i;
    }

    for (size_t i = 0; i < sizeof(buffer_canary_2); i++)
    {
        ASSERT_EQ(buffer_canary_2[i], 0x55) << "Overflow after buffer at i=" << i;
    }
}

TEST_F(TestRawEncoder, BasicEncoding)
{
    uint8_t dst_buffer[10];
    uint8_t compare_buf[sizeof(dst_buffer)];
    float var1;
    uint32_t var2;

    dlconfig.items_count = 2;

    dlconfig.items_to_log[0].type = datalogging::LoggableType::MEMORY;
    dlconfig.items_to_log[0].data.memory.size = sizeof(var1);
    dlconfig.items_to_log[0].data.memory.address = &var1;

    dlconfig.items_to_log[1].type = datalogging::LoggableType::MEMORY;
    dlconfig.items_to_log[1].data.memory.size = sizeof(var2);
    dlconfig.items_to_log[1].data.memory.address = &var2;

    encoder.init();

    var1 = 1.0f;
    var2 = 0x1111;
    encoder.encode_next_entry();
    var1 = 2.0f;
    var2 = 0x2222;
    encoder.encode_next_entry();
    var1 = 3.0f;
    var2 = 0x3333;
    encoder.encode_next_entry();

    datalogging::RawFormatReader *reader = encoder.get_reader();

    reader->read(dst_buffer, sizeof(dst_buffer));
    var1 = 1.0f;
    var2 = 0x1111;
    memcpy(&compare_buf[0], &var1, 4);
    memcpy(&compare_buf[4], &var2, 4);
    var1 = 2.0f;
    var2 = 0x2222;
    memcpy(&compare_buf[8], &var1, 2);

    EXPECT_BUF_EQ(dst_buffer, compare_buf, sizeof(compare_buf));

    reader->read(dst_buffer, sizeof(dst_buffer));
    memcpy(&compare_buf[0], &(reinterpret_cast<uint8_t *>(&var1)[2]), 2);
    memcpy(&compare_buf[2], &var2, 4);
    var1 = 3.0f;
    memcpy(&compare_buf[6], &var1, 4);

    EXPECT_BUF_EQ(dst_buffer, compare_buf, sizeof(compare_buf));

    reader->read(dst_buffer, sizeof(dst_buffer));
    var2 = 0x3333;
    memcpy(&compare_buf[0], &var2, 4);
    EXPECT_BUF_EQ(dst_buffer, compare_buf, 4);
}

#endif