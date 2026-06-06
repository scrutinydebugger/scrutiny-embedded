//    test_ipc.cpp
//        Test for the Inter-process communication tools
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny_ipc.hpp"
#include "scrutinytest/scrutinytest.hpp"
#include <stdint.h>
#include <stdlib.h>

#if __unix__
#include <unistd.h>
#endif

#if SCRUTINY_HAS_CPP11 && defined(__STDCPP_THREADS__) && __STDCPP_THREADS__
#include <thread>
#define TEST_IPC_CPPTHREAD
#elif defined(_POSIX_THREADS) || defined(_REENTRANT)
#include <pthread.h>
#define TEST_IPC_POSIX_THREAD
#else
// Win32 with c++98 ???
#error "Test require an OS with thread capabilities."
#endif

#if SCRUTINY_HAS_CPP11
#include <chrono>
#else
#include <time.h>
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

#if defined(TEST_IPC_CPPTHREAD)
    std::thread thread(thread_func);
#elif defined(TEST_IPC_POSIX_THREAD)
    pthread_t thread;
    ASSERT_EQ(pthread_create(&thread, NULL, thread_func_pthread, NULL), 0);
#else
#error
#endif

#if SCRUTINY_HAS_CPP11
    auto t1 = std::chrono::high_resolution_clock::now();
#else
    struct timespec t1;
    clock_gettime(CLOCK_MONOTONIC, &t1);
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
        struct timespec t2;
        clock_gettime(CLOCK_MONOTONIC, &t2);
        double elapsed_secs = double(t2.tv_sec - t1.tv_sec) + double(t2.tv_nsec - t1.tv_nsec) / 1e9;
        if (elapsed_secs > TIMEOUT_SEC)
        {
            thread_data.thread_exit = true;
            timeout = true;
        }
#endif
    }

#if defined(TEST_IPC_CPPTHREAD)
    ASSERT_TRUE(thread.joinable());
    thread.join();
#elif defined(TEST_IPC_POSIX_THREAD)
    ASSERT_EQ(pthread_join(thread, NULL), 0);
#else
#error
#endif

    EXPECT_FALSE(thread_data.error_found_in_main) << "At Iteration #" << thread_data.error_at_iter;
    EXPECT_FALSE(thread_data.error_found_in_thread) << "At Iteration #" << thread_data.error_at_iter;
    EXPECT_GE(my_value, 1000); // Local test > 3.5M
    EXPECT_GE(thread_data.thread_exit_value, 1000);
}
