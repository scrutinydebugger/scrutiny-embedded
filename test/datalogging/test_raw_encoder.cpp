//    test_raw_encoder.cpp
//        Test suite for the RawFormat encoder.
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "scrutinytest/scrutinytest.hpp"

#include "scrutiny.hpp"
#include "scrutiny_test.hpp"

#if SCRUTINY_DATALOGGING_ENCODING == SCRUTINY_DATALOGGING_ENCODING_RAW

using namespace scrutiny;


static unsigned char _rx_buffer[128];
static unsigned char _tx_buffer[128];

static struct {
    unsigned char canary1[128];
    unsigned char data[128];
    unsigned char canary2[128];
}dlbuffer;


class TestRawEncoder : public ScrutinyTest
{
  protected:
    void check_canaries();

    MainHandler scrutiny_handler;
    Config config;
    datalogging::Configuration dlconfig;
    datalogging::RawFormatEncoder encoder;

    TestRawEncoder() :
        ScrutinyTest(),
        scrutiny_handler(),
        config(),
        dlconfig(),
        encoder()
    {
    }

    virtual void SetUp()
    {
        config.set_buffers(_rx_buffer, sizeof(_rx_buffer), _tx_buffer, sizeof(_tx_buffer));
        scrutiny_handler.init(&config);

#if CHAR_BIT == 8
        memset(dlbuffer.canary1, 0xAA, sizeof(dlbuffer.canary1));
        memset(dlbuffer.canary2, 0x55, sizeof(dlbuffer.canary2));
        memset(dlbuffer.data, 0xFF, sizeof(dlbuffer.data));
#elif CHAR_BIT == 16
        memset(dlbuffer.canary1, 0xAAAA, sizeof(dlbuffer.canary1));
        memset(dlbuffer.canary2, 0x5555, sizeof(dlbuffer.canary2));
        memset(dlbuffer.data, 0xFFFF, sizeof(dlbuffer.data));
#endif
    }
};

#if CHAR_BIT == 8
#define CHECK_CANARIES                                                                                                                               \
    do                                                                                                                                               \
    {                                                                                                                                                \
        ASSERT_BUF_SET(dlbuffer.canary1, 0xAA, sizeof(dlbuffer.canary1)) << "dlbuffer canary died!";                                                 \
        ASSERT_BUF_SET(dlbuffer.canary2, 0x55, sizeof(dlbuffer.canary2)) << "dlbuffer canary died!";                                                 \
    } while (0)

#elif CHAR_BIT == 16
#define CHECK_CANARIES                                                                                                                               \
    do                                                                                                                                               \
    {                                                                                                                                                \
        ASSERT_BUF_SET(dlbuffer.canary1, 0xAAAA, sizeof(dlbuffer.canary1)) << "dlbuffer canary died!";                                               \
        ASSERT_BUF_SET(dlbuffer.canary2, 0x5555, sizeof(dlbuffer.canary2)) << "dlbuffer canary died!";                                               \
    } while (0)
#endif

TEST_F(TestRawEncoder, BasicEncoding)
{
    Timebase timebase;
    unsigned char dst_buffer[10];
    unsigned char compare_buf[128];
    float var1;
    uint32_t var2;

    dlconfig.items_count = 3;

    dlconfig.items_to_log[0].type = datalogging::LoggableType::Memory;
    dlconfig.items_to_log[0].data.memory.size = sizeof(var1);
    dlconfig.items_to_log[0].data.memory.address = &var1;

    dlconfig.items_to_log[1].type = datalogging::LoggableType::Memory;
    dlconfig.items_to_log[1].data.memory.size = sizeof(var2);
    dlconfig.items_to_log[1].data.memory.address = &var2;

    dlconfig.items_to_log[2].type = datalogging::LoggableType::Time;

    encoder.init(&scrutiny_handler, &dlconfig, dlbuffer.data, sizeof(dlbuffer.data));
    encoder.set_timebase(&timebase);
    timebase.reset();

    var1 = 1.0f;
    var2 = 0x1111u;
    encoder.encode_next_entry(SCRUTINY_NULL);
    timebase.step(100);
    var1 = 2.0f;
    var2 = 0x2222u;
    encoder.encode_next_entry(SCRUTINY_NULL);
    timebase.step(100);
    var1 = 3.0f;
    var2 = 0x3333u;
    encoder.encode_next_entry(SCRUTINY_NULL);

    datalogging::RawFormatReader *reader = encoder.get_reader();
    reader->reset();

    SCRUTINY_CONSTEXPR size_t single_entry_size = sizeof(var1) + sizeof(var2) + sizeof(scrutiny::timestamp_t);
    EXPECT_EQ(reader->get_entry_count(), 3u);
    EXPECT_EQ(reader->get_total_size(), single_entry_size * 3u); // entry_size*entry_count
    EXPECT_EQ(reader->get_total_size_8bits(), single_entry_size * 3u * CHAR_BIT / 8);

    uint32_t expected_time;

    // Chunk 1
    var1 = 1.0f;
    var2 = 0x1111;
    expected_time = 0;

    memcpy_dilate_8bits(&compare_buf[0], &var1, 4);
    memcpy_dilate_8bits(&compare_buf[4], &var2, 4);
    scrutiny::codecs::encode_32_bits_big_endian_8bits(expected_time, &compare_buf[8]);

    var1 = 2.0f;
    var2 = 0x2222;
    expected_time = 100;
    memcpy_dilate_8bits(&compare_buf[12], &var1, 4);
    memcpy_dilate_8bits(&compare_buf[16], &var2, 4);
    scrutiny::codecs::encode_32_bits_big_endian_8bits(expected_time, &compare_buf[20]);

    var1 = 3.0f;
    var2 = 0x3333;
    expected_time = 200u;
    memcpy_dilate_8bits(&compare_buf[24], &var1, 4);
    memcpy_dilate_8bits(&compare_buf[28], &var2, 4);
    scrutiny::codecs::encode_32_bits_big_endian_8bits(expected_time, &compare_buf[32]);

    unsigned char const chunk_size = sizeof(dst_buffer);
    unsigned char const nbchunk = static_cast<unsigned char>(static_cast<float>(reader->get_total_size_8bits()) / chunk_size + 0.5f);
    uint32_t total_read = 0;
    for (unsigned char i = 0; i < nbchunk; i++)
    {
        uint32_t nread = reader->read_dilate_8bits(dst_buffer, chunk_size);
        total_read += nread;
        if (reader->finished())
        {
            EXPECT_BUF_EQ(dst_buffer, &compare_buf[i * chunk_size], nread);
        }
        else
        {
            EXPECT_EQ(nread, chunk_size);
            EXPECT_BUF_EQ(dst_buffer, &compare_buf[i * chunk_size], chunk_size);
        }
    }

    EXPECT_EQ(total_read, reader->get_total_size_8bits());
    CHECK_CANARIES;
}

#endif
