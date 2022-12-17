//    test_ipc.cpp
//        Test for the Inter-process communication tools
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include <gtest/gtest.h>
#include "scrutiny_ipc.hpp"
#include <stdlib.h>

enum class SomeEnum
{
    VAL1,
    VAL2,
    VAL3
};

struct SomeData
{
public:
    uint32_t u32;
    SomeEnum e;
};

TEST(TestIPC, Basic)
{
    scrutiny::IPCMessage<SomeData> msg;
    msg.data.u32 = 123;
    msg.data.e = SomeEnum::VAL1;

    EXPECT_FALSE(msg.has_content());
    msg.commit();
    EXPECT_TRUE(msg.has_content());
    msg.clear();
    EXPECT_FALSE(msg.has_content());

    SomeData local_data;
    local_data.u32 = 444;
    local_data.e = SomeEnum::VAL2;

    msg.set_data(local_data);
    EXPECT_TRUE(msg.has_content());
    EXPECT_EQ(msg.data.u32, 444);
    EXPECT_EQ(msg.data.e, SomeEnum::VAL2);

    SomeData local_data2 = msg.pop();
    EXPECT_EQ(local_data2.u32, 444);
    EXPECT_EQ(local_data2.e, SomeEnum::VAL2);

    EXPECT_FALSE(msg.has_content());
}