//    main.cpp
//        The default main for launching the test framework
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "scrutinytest/scrutinytest.hpp"

#include <iostream>
#include <stdint.h>

#if __unix__
#define HAS_TIMESTAMP_FUNC
static uint32_t timestamp_ms_func()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
    {
        return 0;
    }

    return static_cast<uint32_t>(ts.tv_sec * 1000) + static_cast<uint32_t>(ts.tv_nsec / 1000000);
}
#endif

int main(int argc, char *argv[])
{
    static_cast<void>(argc);
    static_cast<void>(argv);
    scrutinytest::set_ostream(&std::cout);
#ifdef HAS_TIMESTAMP_FUNC
    scrutinytest::set_timestamp_func(timestamp_ms_func);
#endif
    return scrutinytest::main();
}
