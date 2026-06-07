//    test_codecs.cpp
//        Tests the different codecs function used across this project
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny.hpp"
#include "scrutiny_test.hpp"
#include "scrutinytest/scrutinytest.hpp"

class TestCodecs : public ScrutinyTest
{
};

TEST_F(TestCodecs, DecodeBigEndian)
{
    {
        unsigned char buffer16[2] = { 0x12, 0x34 };
        EXPECT_EQ(scrutiny::codecs::decode_16_bits_big_endian_8bits(buffer16), 0x1234u);

        unsigned char buffer32[4] = { 0x12, 0x34, 0x56, 0x78 };
        EXPECT_EQ(scrutiny::codecs::decode_32_bits_big_endian_8bits(buffer32), 0x12345678u);

        unsigned char buffer_float[4] = { 0x40, 0x49, 0x0f, 0xda };
        EXPECT_EQ(scrutiny::codecs::decode_float_big_endian_8bits(buffer_float), 3.1415926f);

#if SCRUTINY_SUPPORT_64BITS
        unsigned char buffer64[8] = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 };
        EXPECT_EQ(scrutiny::codecs::decode_64_bits_big_endian_8bits(buffer64), 0x123456789abcdef0u);
#endif
    }

#if CHAR_BIT == 8
    {
        unsigned char buffer16[2] = { 0x12, 0x34 };
        EXPECT_EQ(scrutiny::codecs::decode_16_bits_big_endian_char(buffer16), 0x1234u);

        unsigned char buffer32[4] = { 0x12, 0x34, 0x56, 0x78 };
        EXPECT_EQ(scrutiny::codecs::decode_32_bits_big_endian_char(buffer32), 0x12345678u);

        unsigned char buffer_float[4] = { 0x40, 0x49, 0x0f, 0xda };
        EXPECT_EQ(scrutiny::codecs::decode_float_big_endian_char(buffer_float), 3.1415926f);

#if SCRUTINY_SUPPORT_64BITS
        unsigned char buffer64[8] = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 };
        EXPECT_EQ(scrutiny::codecs::decode_64_bits_big_endian_char(buffer64), 0x123456789abcdef0u);
#endif
    }

#elif CHAR_BIT == 16
    {
        unsigned char buffer16[1] = { 0x1234 };
        EXPECT_EQ(scrutiny::codecs::decode_16_bits_big_endian_char(buffer16), 0x1234u);

        unsigned char buffer32[2] = { 0x1234, 0x5678 };
        EXPECT_EQ(scrutiny::codecs::decode_32_bits_big_endian_char(buffer32), 0x12345678u);

        unsigned char buffer_float[2] = { 0x4049, 0x0fda };
        EXPECT_EQ(scrutiny::codecs::decode_float_big_endian_char(buffer_float), 3.1415926f);

#if SCRUTINY_SUPPORT_64BITS
        unsigned char buffer64[4] = { 0x1234, 0x5678, 0x9abc, 0xdef0 };
        EXPECT_EQ(scrutiny::codecs::decode_64_bits_big_endian_char(buffer64), 0x123456789abcdef0u);
#endif
    }
#endif
}

TEST_F(TestCodecs, EncodeBigEndian)
{
    unsigned char outbuffer[8];

    unsigned char buffer16[2] = { 0x12, 0x34 };
    scrutiny::codecs::encode_16_bits_big_endian_8bits((uint16_t)0x1234u, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer16, sizeof(buffer16));

    unsigned char buffer32[4] = { 0x12, 0x34, 0x56, 0x78 };
    scrutiny::codecs::encode_32_bits_big_endian_8bits((uint32_t)0x12345678u, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer32, sizeof(buffer32));

    unsigned char buffer_float[4] = { 0x40, 0x49, 0x0f, 0xda };
    scrutiny::codecs::encode_float_big_endian_8bits(3.1415926f, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer_float, sizeof(buffer_float));

#if SCRUTINY_SUPPORT_64BITS
    unsigned char buffer64[8] = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 };
    scrutiny::codecs::encode_64_bits_big_endian_8bits((uint64_t)0x123456789abcdef0u, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer64, sizeof(buffer64));
#endif
}

TEST_F(TestCodecs, DecodeLittleEndian)
{
    unsigned char buffer16[2] = { 0x34, 0x12 };
    EXPECT_EQ(scrutiny::codecs::decode_16_bits_little_endian_8bits(buffer16), 0x1234u);

    unsigned char buffer32[4] = { 0x78, 0x56, 0x34, 0x12 };
    EXPECT_EQ(scrutiny::codecs::decode_32_bits_little_endian_8bits(buffer32), 0x12345678u);

    unsigned char buffer_float[4] = { 0xda, 0x0f, 0x49, 0x40 };
    EXPECT_EQ(scrutiny::codecs::decode_float_little_endian_8bits(buffer_float), 3.1415926f);

#if SCRUTINY_SUPPORT_64BITS
    unsigned char buffer64[8] = { 0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12 };
    EXPECT_EQ(scrutiny::codecs::decode_64_bits_little_endian_8bits(buffer64), 0x123456789abcdef0u);
#endif
}

TEST_F(TestCodecs, EncodeLittleEndian)
{
    unsigned char outbuffer[8];

    unsigned char buffer16[2] = { 0x34, 0x12 };
    scrutiny::codecs::encode_16_bits_little_endian_8bits((uint16_t)0x1234u, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer16, sizeof(buffer16));

    unsigned char buffer32[4] = { 0x78, 0x56, 0x34, 0x12 };
    scrutiny::codecs::encode_32_bits_little_endian_8bits((uint32_t)0x12345678u, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer32, sizeof(buffer32));

    unsigned char buffer_float[4] = { 0xda, 0x0f, 0x49, 0x40 };
    scrutiny::codecs::encode_float_little_endian_8bits(3.1415926f, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer_float, sizeof(buffer_float));

#if SCRUTINY_SUPPORT_64BITS
    unsigned char buffer64[8] = { 0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12 };
    scrutiny::codecs::encode_64_bits_little_endian_8bits((uint64_t)0x123456789abcdef0u, outbuffer);
    EXPECT_BUF_EQ(outbuffer, buffer64, sizeof(buffer64));
#endif
}
