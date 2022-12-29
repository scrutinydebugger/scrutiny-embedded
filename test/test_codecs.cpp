#include <gtest/gtest.h>
#include "scrutiny.hpp"

TEST(TestCodecs, BigEndian16)
{
    uint8_t buffer[2] = {0x12, 0x34};
    EXPECT_EQ(scrutiny::codecs::decode_16_bits_big_endian(buffer), 0x1234);
}

TEST(TestCodecs, BigEndian32)
{
    uint8_t buffer[4] = {0x12, 0x34, 0x56, 0x78};
    EXPECT_EQ(scrutiny::codecs::decode_32_bits_big_endian(buffer), 0x12345678);
}

#if SCRUTINY_SUPPORT_64BITS
TEST(TestCodecs, BigEndian64)
{
    uint8_t buffer[8] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0};
    EXPECT_EQ(scrutiny::codecs::decode_64_bits_big_endian(buffer), 0x123456789abcdef0);
}
#endif

TEST(TestCodecs, BigEndianFloat)
{
    uint8_t buffer[4] = {0x40, 0x49, 0x0f, 0xda};
    EXPECT_EQ(scrutiny::codecs::decode_float_big_endian(buffer), 3.1415926f);
}

TEST(TestCodecs, LittleEndian16)
{
    uint8_t buffer[2] = {0x12, 0x34};
    EXPECT_EQ(scrutiny::codecs::decode_16_bits_little_endian(buffer), 0x3412);
}

TEST(TestCodecs, LittleEndian32)
{
    uint8_t buffer[4] = {0x12, 0x34, 0x56, 0x78};
    EXPECT_EQ(scrutiny::codecs::decode_32_bits_little_endian(buffer), 0x78563412);
}

#if SCRUTINY_SUPPORT_64BITS
TEST(TestCodecs, LittleEndian64)
{
    uint8_t buffer[8] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0};
    EXPECT_EQ(scrutiny::codecs::decode_64_bits_little_endian(buffer), 0xf0debc9a78563412);
}
#endif

TEST(TestCodecs, LittleEndianFloat)
{
    uint8_t buffer[4] = {0xda, 0x0f, 0x49, 0x40};
    EXPECT_EQ(scrutiny::codecs::decode_float_little_endian(buffer), 3.1415926f);
}