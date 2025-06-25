//    test_ipc.cpp
//        Test for the Inter-process communication tools
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny_ipc.hpp"
#include "scrutinytest/scrutinytest.hpp"
#include <stdint.h>
#include <stdlib.h>

#if SCRUTINY_HAS_CPP11
#include <chrono>
#include <thread>
#else
#include <ctime>
#include <pthread.h>
#endif

class SomeEnum
{
  public:
    enum E
    {
        VAL1,
        VAL2,
        VAL3
    };
};

struct SomeData
{
  public:
    uint32_t u32;
    SomeEnum::E e;
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
    uint32_t thread_exit_value;
} thread_data;

void thread_func()
{
    uint32_t expected_msg_value = 0;
    uint32_t my_value = 0;
    thread_data.error_found_in_thread = false;
    thread_data.thread_exit_value = 0;
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
    thread_data.thread_exit_value = my_value;
}

void *thread_func_pthread(void *)
{
    thread_func();
    return NULL;
}

TEST(TestIPC, CheckWithThread)
{
    const int TIMEOUT_SEC = 5;
    thread_data.thread_exit = false;

    thread_data.error_found_in_main = false;
    thread_data.error_at_iter = 0;

    uint32_t my_value = 0;
    uint32_t expected_thread_value = 0;
#if SCRUTINY_HAS_CPP11
    std::thread thread(thread_func);
    auto t1 = std::chrono::high_resolution_clock::now();
#else
    pthread_t thread;
    std::clock_t t1 = std::clock();
    ASSERT_EQ(pthread_create(&thread, NULL, thread_func_pthread, NULL), 0);
#endif
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

#if SCRUTINY_HAS_CPP11
        if (std::chrono::high_resolution_clock::now() - t1 > std::chrono::seconds(TIMEOUT_SEC))
        {
            thread_data.thread_exit = true;
        }
#else
        std::clock_t t2 = std::clock();
        double elapsed_secs = double(t2 - t1) / CLOCKS_PER_SEC;
        if (elapsed_secs > TIMEOUT_SEC)
        {
            thread_data.thread_exit = true;
        }
#endif
    }

#if SCRUTINY_HAS_CPP11
    thread.join();
#else
    ASSERT_EQ(pthread_join(thread, NULL), 0);
#endif

    EXPECT_FALSE(thread_data.error_found_in_main) << "At Iteration #" << thread_data.error_at_iter;
    EXPECT_FALSE(thread_data.error_found_in_thread) << "At Iteration #" << thread_data.error_at_iter;
    EXPECT_GE(my_value, 1000); // Local test > 3.5M
    EXPECT_GE(thread_data.thread_exit_value, 1000);
}
