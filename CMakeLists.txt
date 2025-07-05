# Copyright (c) 2021 Scrutiny Debugger
# License : MIT - See LICENSE file.
# Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)

cmake_minimum_required(VERSION 3.14)
project(scrutiny-embedded)

option(SCRUTINY_BUILD_TEST "Activate test suite" OFF)
option(SCRUTINY_ENABLE_DATALOGGING "Enable datalogging feature" ON)
option(SCRUTINY_SUPPORT_64BITS "Enable support for 64bits variables" ON)
option(SCRUTINY_BUILD_CWRAPPER "Build a C99 wrapper" ON)
option(INSTALL_FOLDER "Install folder" ${CMAKE_CURRENT_BINARY_DIR}/install )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib)
if (SCRUTINY_BUILD_CWRAPPER)
    add_subdirectory(cwrapper)
endif()
add_subdirectory(projects)

if (SCRUTINY_BUILD_TEST)
    add_subdirectory(test)
endif()

include(cmake/scrutiny.cmake)    # Make available for project importing us with FetchContent