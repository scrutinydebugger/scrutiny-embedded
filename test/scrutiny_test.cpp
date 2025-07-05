//    scrutiny_test.cpp
//        Base class for CPP unit tests.
//        All test should inherit this class.
//         Includes bunch of helper for easy testing.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny_test.hpp"
#include "scrutiny.hpp"
#include <stdint.h>
#include <string>

void ScrutinyTest::add_crc(uint8_t *data, uint16_t data_len)
{
    uint32_t crc = scrutiny::tools::crc32(data, data_len);
    data[data_len] = (crc >> 24) & 0xFF;
    data[data_len + 1] = (crc >> 16) & 0xFF;
    data[data_len + 2] = (crc >> 8) & 0xFF;
    data[data_len + 3] = (crc >> 0) & 0xFF;
}

void ScrutinyTest::add_crc(scrutiny::protocol::Response *response)
{
    uint8_t header[5];
    header[0] = response->command_id;
    header[1] = response->subfunction_id;
    header[2] = response->response_code;
    header[3] = (response->data_length >> 8) & 0xFF;
    header[4] = response->data_length & 0xFF;

    uint32_t crc = scrutiny::tools::crc32(header, sizeof(header));
    response->crc = scrutiny::tools::crc32(response->data, response->data_length, crc);
}

void ScrutinyTest::fill_buffer_incremental(uint8_t *buffer, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        buffer[i] = static_cast<uint8_t>(i & 0xFFu);
    }
}

bool ScrutinyTest::TEST_IS_PROTOCOL_RESPONSE(
    uint8_t *buffer,
    scrutiny::protocol::CommandId::eCommandId cmd,
    uint8_t subfunction,
    scrutiny::protocol::ResponseCode::eResponseCode code)
{
    if (buffer[0] != (static_cast<uint8_t>(cmd) | 0x80))
    {
        SCRUTINYTEST_FAIL << "Wrong command ID. Got " << static_cast<uint32_t>(buffer[0]) << " but expected " << static_cast<uint32_t>(cmd);
    }

    if (buffer[1] != subfunction)
    {
        SCRUTINYTEST_FAIL << "Wrong Subfunction. Got " << static_cast<uint32_t>(buffer[1]) << " but expected " << static_cast<uint32_t>(subfunction);
    }

    if (buffer[2] != static_cast<uint8_t>(code))
    {
        SCRUTINYTEST_FAIL << "Wrong response code. Got " << static_cast<scrutiny::protocol::ResponseCode::eResponseCode>(buffer[2])
                          << " but expected " << code;
    }
    uint16_t length = (static_cast<uint16_t>(buffer[3]) << 8) | static_cast<uint16_t>(buffer[4]);
    if (code != scrutiny::protocol::ResponseCode::OK && length != 0)
    {
        SCRUTINYTEST_FAIL << "Wrong command length. Got " << static_cast<uint32_t>(length) << " but expected 0";
    }
    SCRUTINYTEST_PASS;
}

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4127) // Get rid of SCRUTINY_CONSTEXPR always true condition warning.
#endif

unsigned int ScrutinyTest::encode_addr(uint8_t *buffer, void *addr)
{
    uintptr_t ptr = reinterpret_cast<uintptr_t>(addr);
    SCRUTINY_CONSTEXPR unsigned int addr_size = sizeof(ptr);

    unsigned int i = addr_size - 1;

    if (addr_size >= 1)
    {
        buffer[i--] = static_cast<uint8_t>((ptr >> (0 * (addr_size >= 1))) & 0xFF);
    }

    if (addr_size >= 2)
    {
        buffer[i--] = static_cast<uint8_t>((ptr >> (8 * (addr_size >= 2))) & 0xFF);
    }

    if (addr_size >= 4)
    {
        buffer[i--] = static_cast<uint8_t>((ptr >> (16 * (addr_size >= 4))) & 0xFF);
        buffer[i--] = static_cast<uint8_t>((ptr >> (24 * (addr_size >= 4))) & 0xFF);
    }

    if (addr_size == 8)
    {
        buffer[i--] = static_cast<uint8_t>((ptr >> (32 * (addr_size >= 8))) & 0xFF);
        buffer[i--] = static_cast<uint8_t>((ptr >> (40 * (addr_size >= 8))) & 0xFF);
        buffer[i--] = static_cast<uint8_t>((ptr >> (48 * (addr_size >= 8))) & 0xFF);
        buffer[i--] = static_cast<uint8_t>((ptr >> (56 * (addr_size >= 8))) & 0xFF);
    }

    return addr_size;
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace scrutiny
{
    namespace protocol
    {
        std::ostream &operator<<(std::ostream &out, scrutiny::protocol::ResponseCode::eResponseCode val)
        {
            switch (val)
            {
            case scrutiny::protocol::ResponseCode::OK:
                out << "OK";
                break;
            case scrutiny::protocol::ResponseCode::Busy:
                out << "Busy";
                break;
            case scrutiny::protocol::ResponseCode::FailureToProceed:
                out << "FailureToProceed";
                break;
            case scrutiny::protocol::ResponseCode::Forbidden:
                out << "Forbidden";
                break;
            case scrutiny::protocol::ResponseCode::InvalidRequest:
                out << "InvalidRequest";
                break;
            case scrutiny::protocol::ResponseCode::NoResponseToSend:
                out << "NoResponseToSend";
                break;
            case scrutiny::protocol::ResponseCode::Overflow:
                out << "Overflow";
                break;
            case scrutiny::protocol::ResponseCode::ProcessAgain:
                out << "ProcessAgain";
                break;
            case scrutiny::protocol::ResponseCode::UnsupportedFeature:
                out << "UnsupportedFeature";
                break;
            default:
                out << "Unknown";
                break;
            }

            return out << " (" << static_cast<uint32_t>(val) << ")";
        }
    } // namespace protocol
} // namespace scrutiny

#if SCRUTINY_ENABLE_DATALOGGING
namespace scrutiny
{
    namespace datalogging
    {
        std::ostream &operator<<(std::ostream &out, DataLogger::State::eState val)
        {
            switch (val)
            {
            case DataLogger::State::IDLE:
                out << "IDLE";
                break;
            case DataLogger::State::ARMED:
                out << "ARMED";
                break;
            case DataLogger::State::CONFIGURED:
                out << "CONFIGURED";
                break;
            case DataLogger::State::ACQUISITION_COMPLETED:
                out << "ACQUISITION_COMPLETED";
                break;
            case DataLogger::State::ERROR:
                out << "ERROR";
                break;
            default:
                out << "UNKNOWN";
            }

            return out << " (" << static_cast<uint32_t>(val) << ")";
        }
    } // namespace datalogging
} // namespace scrutiny
#endif
