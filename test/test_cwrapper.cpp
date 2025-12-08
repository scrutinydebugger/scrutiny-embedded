//    test_cwrapper.cpp
//        A testsuite for the CWrapper
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny_cwrapper.h"
#include "scrutiny_test.hpp"
#include "scrutinytest/scrutinytest.hpp"

class TestCWrapper : public ScrutinyTest
{
};

#ifdef SCRUTINY_CWRAPPER_EXTRACT_CPP_CONSTANTS

TEST_F(TestCWrapper, TestExtractSymbols)
{
    // Make sure the constants extracted with symdump matches what the C++ compiler embedded in the cwrapper .o

    extern size_t const SCRUTINY_C_MAIN_HANDLER_SIZE;
    extern size_t const SCRUTINY_C_CONFIG_SIZE;
    extern size_t const SCRUTINY_C_LOOP_HANDLER_FF_SIZE;
    extern size_t const SCRUTINY_C_LOOP_HANDLER_VF_SIZE;

    EXPECT_EQ(SCRUTINY_C_MAIN_HANDLER_SIZE, CPP_CONST_SCRUTINY_C_MAIN_HANDLER_SIZE);
    EXPECT_EQ(SCRUTINY_C_CONFIG_SIZE, CPP_CONST_SCRUTINY_C_CONFIG_SIZE);
    EXPECT_EQ(SCRUTINY_C_LOOP_HANDLER_FF_SIZE, CPP_CONST_SCRUTINY_C_LOOP_HANDLER_FF_SIZE);
    EXPECT_EQ(SCRUTINY_C_LOOP_HANDLER_VF_SIZE, CPP_CONST_SCRUTINY_C_LOOP_HANDLER_VF_SIZE);
}
#endif
