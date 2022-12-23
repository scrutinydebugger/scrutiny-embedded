#include <gtest/gtest.h>

#include "scrutiny_test.hpp"
#include "scrutiny.hpp"

#if SCRUTINY_DATALOGGING_ENCODING == SCRUTINY_DATALOGGING_ENCODING_RAW

using namespace scrutiny;

class TestRawEncoder : public ScrutinyTest
{
public:
    TestRawEncoder() : ScrutinyTest(),
                       encoder(dlbuffer, sizeof(dlbuffer), &dlconfig)
    {
    }

protected:
    datalogging::Configuration dlconfig;
    datalogging::RawFormatEncoder encoder;

    uint8_t dlbuffer[128];
};

TEST_F(TestRawEncoder, BasicEncoding)
{
    uint8_t dst_buffer[10];
    uint8_t compare_buf[sizeof(dst_buffer)];
    float var1;
    uint32_t var2;

    dlconfig.block_count = 2;
    dlconfig.memblocks[0] = &var1;
    dlconfig.memblocks[1] = &var2;
    dlconfig.blocksizes[0] = sizeof(var1);
    dlconfig.blocksizes[1] = sizeof(var2);

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