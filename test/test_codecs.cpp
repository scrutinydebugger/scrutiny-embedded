#include <gtest/gtest.h>
#include "scrutiny.hpp"
#include "scrutiny_test.hpp"

class TestCodecs : public ScrutinyTest
{
};

TEST_F(TestCodecs, DecodeBigEndian)
{
    uint8_t buffer16[2] = {0x12, 0x34};
    EXPECT_EQ(scrutiny::codecs::decode_16_bits_big_endian(buffer16), 0x1234u);

    uint8_t buffer32[4] = {0x12, 0x34, 0x56, 0x78};
    EXPECT_EQ(scrutiny::codecs::decode_32_bits_big_endian(buffer32), 0x12345678u);

    uint8_t buffer_float[4] = {0x40, 0x49, 0x0f, 0xda};
    EXPECT_EQ(scrutiny::codecs::decode_float_big_endian(buffer_float), 3.1415926f);

#if SCRUTINY_SUPPORT_64BITS
    uint8_t buffer64[8] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0};
    EXPECT_EQ(scrutiny::codecs::decode_64_bits_big_endian(buffer64), 0x123456789abcdef0u);
#endif
}

TEST_F(TestCodecs, EncodeBigEndian)
{
    uint8_t outbuffer[8];

    uint8_t buffer16[2] = {0x12, 0x34};
    scrutiny::codecs::encode_16_bits_big_endian(0x1234u, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer16, sizeof(buffer16));

    uint8_t buffer32[4] = {0x12, 0x34, 0x56, 0x78};
    scrutiny::codecs::encode_32_bits_big_endian(0x12345678u, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer32, sizeof(buffer32));

    uint8_t buffer_float[4] = {0x40, 0x49, 0x0f, 0xda};
    scrutiny::codecs::encode_float_big_endian(3.1415926f, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer_float, sizeof(buffer_float));

#if SCRUTINY_SUPPORT_64BITS
    uint8_t buffer64[8] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0};
    scrutiny::codecs::encode_64_bits_big_endian(0x123456789abcdef0u, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer64, sizeof(buffer64));
#endif
}

TEST_F(TestCodecs, DecodeLittleEndian)
{
    uint8_t buffer16[2] = {0x34, 0x12};
    EXPECT_EQ(scrutiny::codecs::decode_16_bits_little_endian(buffer16), 0x1234u);

    uint8_t buffer32[4] = {0x78, 0x56, 0x34, 0x12};
    EXPECT_EQ(scrutiny::codecs::decode_32_bits_little_endian(buffer32), 0x12345678u);

    uint8_t buffer_float[4] = {0xda, 0x0f, 0x49, 0x40};
    EXPECT_EQ(scrutiny::codecs::decode_float_little_endian(buffer_float), 3.1415926f);

#if SCRUTINY_SUPPORT_64BITS
    uint8_t buffer64[8] = {0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12};
    EXPECT_EQ(scrutiny::codecs::decode_64_bits_little_endian(buffer64), 0x123456789abcdef0u);
#endif
}

TEST_F(TestCodecs, EncodeLittleEndian)
{
    uint8_t outbuffer[8];

    uint8_t buffer16[2] = {0x34, 0x12};
    scrutiny::codecs::encode_16_bits_little_endian(0x1234u, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer16, sizeof(buffer16));

    uint8_t buffer32[4] = {0x78, 0x56, 0x34, 0x12};
    scrutiny::codecs::encode_32_bits_little_endian(0x12345678u, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer32, sizeof(buffer32));

    uint8_t buffer_float[4] = {0xda, 0x0f, 0x49, 0x40};
    scrutiny::codecs::encode_float_little_endian(3.1415926f, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer_float, sizeof(buffer_float));

#if SCRUTINY_SUPPORT_64BITS
    uint8_t buffer64[8] = {0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12};
    scrutiny::codecs::encode_64_bits_little_endian(0x123456789abcdef0u, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer64, sizeof(buffer64));
#endif
}