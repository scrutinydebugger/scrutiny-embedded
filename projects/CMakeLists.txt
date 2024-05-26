# Copyright (c) 2021-2023 Scrutiny Debugger
# License : MIT - See LICENSE file.
# Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)

cmake_minimum_required(VERSION 3.14)

option(SCRUTINY_BUILD_TESTAPP "Build the test application" OFF)

if (SCRUTINY_BUILD_TESTAPP)
    add_subdirectory(testapp)
endif()

if (SCRUTINY_BUILD_TESTAPP AND SCRUTINY_BUILD_C_WRAPPER)
    add_subdirectory(c_testapp)
endif()