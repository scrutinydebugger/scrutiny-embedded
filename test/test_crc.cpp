//    test_crc.cpp
//        Test CRC calculation
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "scrutinytest/scrutinytest.hpp"
#include <stdint.h>

#include "scrutiny_tools.hpp"

TEST(TestCRC, TestCRC32)
{
    uint8_t data[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    uint32_t crc = scrutiny::tools::crc32(data, 10);
    EXPECT_EQ(crc, 622876539u);
}

TEST(TestCRC, TestCRC32_broken)
{
    uint8_t data[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    uint32_t crc = scrutiny::tools::crc32(data, 5);
    crc = scrutiny::tools::crc32(&data[5], 5, crc);
    EXPECT_EQ(crc, 622876539u);
}
