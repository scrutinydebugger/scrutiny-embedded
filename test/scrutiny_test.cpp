//    scrutiny_test.cpp
//        Base class for CPP unit tests.
//        All test should inherit this class.
//         Includes bunch of helper for easy testing.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny)
//
//   Copyright (c) 2021-2022 scrutinydebugger

#include <cstdint>
#include "scrutiny.h"
#include "scrutiny_test.h"

void ScrutinyTest::add_crc(uint8_t* data, uint16_t data_len)
{
	uint32_t crc = scrutiny::crc32(data, data_len);
	data[data_len] = (crc >> 24) & 0xFF;
	data[data_len + 1] = (crc >> 16) & 0xFF;
	data[data_len + 2] = (crc >> 8) & 0xFF;
	data[data_len + 3] = (crc >> 0) & 0xFF;
}

void ScrutinyTest::add_crc(scrutiny::protocol::Response* response)
{
	uint8_t header[5];
	header[0] = response->command_id;
	header[1] = response->subfunction_id;
	header[2] = response->response_code;
	header[3] = (response->data_length >> 8) & 0xFF;
	header[4] = response->data_length & 0xFF;

	uint32_t crc = scrutiny::crc32(header, sizeof(header));
	response->crc = scrutiny::crc32(response->data, response->data_length, crc);
}

void ScrutinyTest::fill_buffer_incremental(uint8_t* buffer, uint32_t length)
{
	for (uint32_t i = 0; i < length; i++)
	{
		buffer[i] = static_cast<uint8_t>(i & 0xFFu);
	}
}

::testing::AssertionResult ScrutinyTest::COMPARE_BUF(const uint8_t* candidate, const uint8_t* expected, const uint32_t size)
{
	for (uint32_t i = 0; i < size; ++i)
	{
		if (expected[i] != candidate[i])
		{
			return ::testing::AssertionFailure() << "candidate[" << i
				<< "] (" << static_cast<uint32_t>(candidate[i]) << ") != expected[" << i
				<< "] (" << static_cast<uint32_t>(expected[i]) << ")";
		}
	}

	return ::testing::AssertionSuccess();
}

::testing::AssertionResult ScrutinyTest::IS_PROTOCOL_RESPONSE(uint8_t* buffer, scrutiny::protocol::CommandId cmd, uint8_t subfunction, scrutiny::protocol::ResponseCode code)
{
	if (buffer[0] != (static_cast<uint8_t>(cmd) | 0x80))
	{
		return ::testing::AssertionFailure() << "Wrong command ID. Got " << static_cast<uint32_t>(buffer[0]) << " but expected " << static_cast<uint32_t>(cmd);
	}

	if (buffer[1] != subfunction)
	{
		return ::testing::AssertionFailure() << "Wrong Subfunction. Got " << static_cast<uint32_t>(buffer[1]) << " but expected " << static_cast<uint32_t>(subfunction);
	}

	if (buffer[2] != static_cast<uint8_t>(code))
	{
		return ::testing::AssertionFailure() << "Wrong response code. Got " << static_cast<uint32_t>(buffer[2]) << " but expected " << static_cast<uint32_t>(code);
	}
	uint16_t length = (static_cast<uint16_t>(buffer[3]) << 8) | static_cast<uint16_t>(buffer[4]);
	if (code != scrutiny::protocol::ResponseCode::OK && length != 0)
	{
		return ::testing::AssertionFailure() << "Wrong command length. Got " << static_cast<uint32_t>(length) << " but expected 0";
	}


	return ::testing::AssertionSuccess();
}

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4127)   // Get rid of constexpr always true condition warning.
#endif 

unsigned int ScrutinyTest::encode_addr(uint8_t* buffer, void* addr)
{
	std::uintptr_t ptr = reinterpret_cast<std::uintptr_t>(addr);
	constexpr unsigned int addr_size = sizeof(ptr);

	unsigned int i = addr_size-1;

	if (addr_size >= 1)
	{
		buffer[i--] = static_cast<uint8_t>((ptr >> 0) & 0xFF);
	}

	if (addr_size >= 2)
	{
		buffer[i--] = static_cast<uint8_t>((ptr >> 8) & 0xFF);
	}

	if (addr_size >= 4)
	{
		buffer[i--] = static_cast<uint8_t>((ptr >> 16) & 0xFF);
		buffer[i--] = static_cast<uint8_t>((ptr >> 24) & 0xFF);
	}

	if (addr_size == 8)
	{
		buffer[i--] = static_cast<uint8_t>((ptr >> 32) & 0xFF);
		buffer[i--] = static_cast<uint8_t>((ptr >> 40) & 0xFF);
		buffer[i--] = static_cast<uint8_t>((ptr >> 48) & 0xFF);
		buffer[i--] = static_cast<uint8_t>((ptr >> 56) & 0xFF);
	}

	return addr_size;
}

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif 




