//    scrutiny_test.hpp
//        Base class for CPP unit tests.
//        All test should inherit this class.
//         Includes bunch of helper for easy testing.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include <cstdlib>
#include <gtest/gtest.h>
#include <stdint.h>

#include "scrutiny.hpp"

#define ASSERT_BUF_EQ(candidate, expected, size) ASSERT_TRUE(COMPARE_BUF(candidate, expected, size))
#define EXPECT_BUF_EQ(candidate, expected, size) EXPECT_TRUE(COMPARE_BUF(candidate, expected, size))
#define ASSERT_BUF_SET(buffer, val, size) ASSERT_TRUE(CHECK_SET(buffer, val, size))
#define EXPECT_BUF_SET(buffer, val, size) EXPECT_TRUE(CHECK_SET(buffer, val, size))

class ScrutinyTest : public ::testing::Test
{
  protected:
    inline std::vector<uint8_t> make_payload_1(uint8_t v0)
    {
        std::vector<uint8_t> o;
        o.resize(1);
        o[0] = v0;
        return o;
    }

    inline std::vector<uint8_t> make_payload_2(uint8_t v0, uint8_t v1)
    {
        std::vector<uint8_t> o;
        o.resize(2);
        o[0] = v0;
        o[1] = v1;
        return o;
    }

    inline std::vector<uint8_t> make_payload_4(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3)
    {
        std::vector<uint8_t> o;
        o.resize(4);
        o[0] = v0;
        o[1] = v1;
        o[2] = v2;
        o[3] = v3;
        return o;
    }

    inline std::vector<uint8_t> make_payload_8(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4, uint8_t v5, uint8_t v6, uint8_t v7)
    {
        std::vector<uint8_t> o;
        o.resize(8);
        o[0] = v0;
        o[1] = v1;
        o[2] = v2;
        o[3] = v3;
        o[4] = v4;
        o[5] = v5;
        o[6] = v6;
        o[7] = v7;
        return o;
    }

    template <typename T> std::string NumberToString(T Number)
    {
        std::ostringstream ss;
        ss << Number;
        return ss.str();
    }

    inline float round(float const v)
    {
        float const sign = (v >= 0) ? 1.0 : -1.0;
        return static_cast<float>(static_cast<int>(v + 0.5 * sign));
    }

    void add_crc(uint8_t *data, uint16_t data_len);
    void add_crc(scrutiny::protocol::Response *response);
    void fill_buffer_incremental(uint8_t *buffer, uint32_t length);
    unsigned int encode_addr(uint8_t *buffer, void *addr);

    ::testing::AssertionResult COMPARE_BUF(uint8_t const *candidate, uint8_t const *expected, uint32_t const size);
    ::testing::AssertionResult CHECK_SET(uint8_t const *buffer, uint8_t const val, uint32_t const size);
    ::testing::AssertionResult IS_PROTOCOL_RESPONSE(
        uint8_t *buffer,
        scrutiny::protocol::CommandId::E cmd,
        uint8_t subfunction,
        scrutiny::protocol::ResponseCode::E code);
};

namespace scrutiny
{
    namespace protocol
    {
        std::ostream &operator<<(std::ostream &out, ResponseCode val);
    }
} // namespace scrutiny

#if SCRUTINY_ENABLE_DATALOGGING
namespace scrutiny
{
    namespace datalogging
    {
        std::ostream &operator<<(std::ostream &out, DataLogger::State val);
    }
} // namespace scrutiny

#endif
