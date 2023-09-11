# Copyright (c) 2021-2023 Scrutiny Debugger
# License : MIT - See LICENSE file.
# Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)

cmake_minimum_required(VERSION 3.14)
project(scrutiny-embedded)

option(SCRUTINY_BUILD_TEST "Activate test suite" ON)
option(SCRUTINY_ENABLE_DATALOGGING "Enable datalogging feature" ON)
option(SCRUTINY_SUPPORT_64BITS "Enable support for 64bits variables" ON)
option(INSTALL_FOLDER "Install folder" ${CMAKE_CURRENT_BINARY_DIR}/install )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib)
add_subdirectory(projects)

if (SCRUTINY_BUILD_TEST)
    add_subdirectory(test)
endif()
