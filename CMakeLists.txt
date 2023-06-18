# Copyright (c) 2021-2023 Scrutiny Debugger
# License : MIT - See LICENSE file.
# Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)

cmake_minimum_required(VERSION 3.14)

option(SCRUTINY_BUILD_TEST "Activate test suite" ON)
option(SCRUTINY_ENABLE_DATALOGGING "Activate test suite" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(lib)
add_subdirectory(projects)

if (SCRUTINY_BUILD_TEST)
    add_subdirectory(test)
endif()
