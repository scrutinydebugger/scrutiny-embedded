//    test_ipc.cpp
//        Test for the Inter-process communication tools
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#include <gtest/gtest.h>
#include "scrutiny_ipc.hpp"
#include <stdlib.h>
#include <thread>
#include <chrono>

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

    msg.send(local_data);
    EXPECT_TRUE(msg.has_content());
    EXPECT_EQ(msg.data.u32, 444u);
    EXPECT_EQ(msg.data.e, SomeEnum::VAL2);

    SomeData local_data2 = msg.pop();
    EXPECT_EQ(local_data2.u32, 444u);
    EXPECT_EQ(local_data2.e, SomeEnum::VAL2);

    EXPECT_FALSE(msg.has_content());
}

static struct
{
    scrutiny::IPCMessage<uint32_t> msg_to_thread;
    scrutiny::IPCMessage<uint32_t> msg_from_thread;
    bool thread_exit;
    bool error_found_in_thread;
    bool error_found_in_main;
    uint32_t error_at_iter;
} thread_data;

void thread_func()
{
    uint32_t expected_msg_value = 0;
    uint32_t my_value = 0;
    thread_data.error_found_in_thread = false;
    while (!thread_data.thread_exit)
    {
        if (thread_data.msg_to_thread.has_content())
        {
            if (thread_data.msg_to_thread.data != expected_msg_value)
            {
                thread_data.error_found_in_thread = true;
                thread_data.error_at_iter = my_value;
                thread_data.thread_exit = true;
            }
            expected_msg_value++;
            thread_data.msg_to_thread.clear();
        }

        if (!thread_data.msg_from_thread.has_content())
        {
            thread_data.msg_from_thread.send(my_value);
            my_value++;
        }
    }
}

TEST(TestIPC, CheckWithThread)
{
    thread_data.thread_exit = false;

    thread_data.error_found_in_main = false;
    thread_data.error_at_iter = 0;

    uint32_t my_value = 0;
    uint32_t expected_thread_value = 0;

    std::thread thread(thread_func);
    auto t1 = std::chrono::high_resolution_clock::now();

    while (!thread_data.thread_exit)
    {
        if (thread_data.msg_from_thread.has_content())
        {
            if (thread_data.msg_from_thread.data != expected_thread_value)
            {
                thread_data.error_found_in_main = true;
                thread_data.error_at_iter = my_value;
                thread_data.thread_exit = true;
            }
            expected_thread_value++;
            thread_data.msg_from_thread.clear();
        }

        if (!thread_data.msg_to_thread.has_content())
        {
            thread_data.msg_to_thread.send(my_value);
            my_value++;
        }

        if (my_value >= 10000000)
        {
            thread_data.thread_exit = true;
        }

        if (std::chrono::high_resolution_clock::now() - t1 > std::chrono::seconds(5))
        {
            thread_data.thread_exit = true;
        }
    }

    thread.join();

    EXPECT_FALSE(thread_data.error_found_in_main) << "At Iteration #" << thread_data.error_at_iter;
    EXPECT_FALSE(thread_data.error_found_in_thread) << "At Iteration #" << thread_data.error_at_iter;
}