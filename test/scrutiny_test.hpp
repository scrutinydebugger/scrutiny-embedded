//    scrutinytest.hpp
//        Base class for CPP unit tests.
//        All test should inherit this class.
//         Includes bunch of helper for easy testing.
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "scrutinytest/scrutinytest.hpp"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <vector>
#if !SCRUTINYTEST_NO_OUTPUT
#include <sstream>
#include <string>
#endif

#include "scrutiny.hpp"

#define EXPECT_IS_PROTOCOL_RESPONSE(buffer, cmd, subfunction, code)                                                                                  \
    SCRUTINYTEST_EXPECT_WITH_DETAILS(                                                                                                                \
        TEST_IS_PROTOCOL_RESPONSE(buffer, cmd, subfunction, code),                                                                                   \
        "EXPECT_IS_PROTOCOL_RESPONSE(" #buffer "," #cmd "," #subfunction "," #code ")")

#define ASSERT_IS_PROTOCOL_RESPONSE(buffer, cmd, subfunction, code)                                                                                  \
    SCRUTINYTEST_ASSERT_WITH_DETAILS(                                                                                                                \
        TEST_IS_PROTOCOL_RESPONSE(buffer, cmd, subfunction, code),                                                                                   \
        "ASSERT_IS_PROTOCOL_RESPONSE(" #buffer "," #cmd "," #subfunction "," #code ")")

#if SCRUTINY_HAS_CPP11
#define GET_VEC_DATA(v) (v.data())
#else
#define GET_VEC_DATA(v) (&(v[0]))
#endif

#define SIZEOF_8BITS(x) (static_cast<size_t>(sizeof(x) * (CHAR_BIT / 8)))

class ScrutinyTest : public scrutinytest::TestCase
{
  protected:
    inline void memcpy_dilate_8bits(void *const dst, void const *const src, size_t const nb_8bits)
    {
#if CHAR_BIT == 8
        memcpy(dst, src, nb_8bits);
#elif CHAR_BIT == 16
        for (size_t i = 0; i < (nb_8bits >> 1); i++)
        {
            static_cast<unsigned char *>(dst)[2 * i] = (static_cast<unsigned char const *>(src)[i] >> 8) & 0xFF;
            static_cast<unsigned char *>(dst)[2 * i + 1] = (static_cast<unsigned char const *>(src)[i] & 0xFF);
        }
#endif
    }

    inline std::vector<unsigned char> make_payload_1(unsigned char v0)
    {
        std::vector<unsigned char> o;
        o.resize(1);
        o[0] = v0;
        return o;
    }

    inline std::vector<unsigned char> make_payload_2(unsigned char v0, unsigned char v1)
    {
        std::vector<unsigned char> o;
        o.resize(2);
        o[0] = v0;
        o[1] = v1;
        return o;
    }

    inline std::vector<unsigned char> make_payload_4(unsigned char v0, unsigned char v1, unsigned char v2, unsigned char v3)
    {
        std::vector<unsigned char> o;
        o.resize(4);
        o[0] = v0;
        o[1] = v1;
        o[2] = v2;
        o[3] = v3;
        return o;
    }

    inline std::vector<unsigned char> make_payload_8(
        unsigned char v0,
        unsigned char v1,
        unsigned char v2,
        unsigned char v3,
        unsigned char v4,
        unsigned char v5,
        unsigned char v6,
        unsigned char v7)
    {
        std::vector<unsigned char> o;
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

#if !SCRUTINYTEST_NO_OUTPUT
    template <typename T> std::string NumberToString(T Number)
    {
        std::ostringstream ss;
        if (sizeof(T) == sizeof(char))
        {
            ss << static_cast<int>(Number);
        }
        else
        {
            ss << Number;
        }

        return ss.str();
    }
#endif

    inline float round(float const v)
    {
        float const sign = (v >= 0) ? 1.0 : -1.0;
        return static_cast<float>(static_cast<int>(v + 0.5 * sign));
    }

    void add_crc(unsigned char *data, uint16_t data_len);
    void add_crc(scrutiny::protocol::Response *response);
    void fill_buffer_incremental(unsigned char *buffer, uint32_t length);
    unsigned int encode_addr(unsigned char *buffer, void *addr);

    bool TEST_IS_PROTOCOL_RESPONSE(
        unsigned char *buffer,
        scrutiny::protocol::CommandId::eCommandId cmd,
        uint_least8_t subfunction,
        scrutiny::protocol::ResponseCode::eResponseCode code);
};

namespace scrutiny
{
    namespace protocol
    {
        scrutinytest::ostream &operator<<(scrutinytest::ostream &out, ResponseCode val);
    }
} // namespace scrutiny

#if SCRUTINY_ENABLE_DATALOGGING
namespace scrutiny
{
    namespace datalogging
    {
        scrutinytest::ostream &operator<<(scrutinytest::ostream &out, DataLogger::State val);
    }
} // namespace scrutiny

#endif
