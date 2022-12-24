//    scrutiny_test.hpp
//        Base class for CPP unit tests.
//        All test should inherit this class.
//         Includes bunch of helper for easy testing.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include <gtest/gtest.h>
#include <cstdlib>
#include <cstdint>

#include "scrutiny.hpp"

#define ASSERT_BUF_EQ(a, b, size) ASSERT_TRUE(COMPARE_BUF(a, b, size))
#define EXPECT_BUF_EQ(a, b, size) EXPECT_TRUE(COMPARE_BUF(a, b, size))
#define ASSERT_BUF_SET(a, val, size) ASSERT_TRUE(CHECK_SET(a, val, size))
#define EXPECT_BUF_SET(a, val, size) EXPECT_TRUE(CHECK_SET(a, val, size))

class ScrutinyTest : public ::testing::Test
{
protected:
    void add_crc(uint8_t *data, uint16_t data_len);
    void add_crc(scrutiny::protocol::Response *response);
    void fill_buffer_incremental(uint8_t *buffer, uint32_t length);
    unsigned int encode_addr(uint8_t *buffer, void *addr);

    ::testing::AssertionResult COMPARE_BUF(const uint8_t *candidate, const uint8_t *expected, const uint32_t size);
    ::testing::AssertionResult CHECK_SET(const uint8_t *buffer, const uint8_t val, const uint32_t size);
    ::testing::AssertionResult IS_PROTOCOL_RESPONSE(uint8_t *buffer, scrutiny::protocol::CommandId cmd, uint8_t subfunction, scrutiny::protocol::ResponseCode code);
};