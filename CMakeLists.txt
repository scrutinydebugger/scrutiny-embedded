# Copyright (c) 2021 Scrutiny Debugger
# License : MIT - See LICENSE file.
# Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)

cmake_minimum_required(VERSION 3.14)
project(scrutiny-embedded)

macro(SCRUTINY_OPTION name default_value type docstring)
    if(NOT DEFINED ${name})
        set(${name} ${default_value} CACHE ${type} ${docstring})
    else()
        set(${name} ${${name}} CACHE ${type} ${docstring} FORCE)
    endif()
    message(STATUS "Scrutiny option: ${name}=${${name}}")
endmacro()

SCRUTINY_OPTION(SCRUTINY_BUILD_TEST                     OFF         BOOL    "Activate test suite")
SCRUTINY_OPTION(SCRUTINY_ENABLE_DATALOGGING             ON          BOOL    "Enable datalogging feature")
SCRUTINY_OPTION(SCRUTINY_SUPPORT_64BITS                 OFF         BOOL    "Enable support for 64bits variables")
SCRUTINY_OPTION(SCRUTINY_SUPPORT_PROTECTED_REGIONS      OFF         BOOL    "Allow setting read-only and forbidden regions")
SCRUTINY_OPTION(SCRUTINY_BUILD_CWRAPPER                 ON          BOOL    "Build a C99 wrapper")
SCRUTINY_OPTION(SCRUTINY_DATALOGGING_BUFFER_32BITS      OFF         BOOL    "Allow datalogging buffers bigger than 65536 bytes")
SCRUTINY_OPTION(SCRUTINY_USE_ASAN                       OFF         BOOL    "Build Scrutiny with Address Sanitizer (for unit testing)")
SCRUTINY_OPTION(SCRUTINY_DATALOGGING_MAX_SIGNAL         16          STRING  "Maximum number of datalogging signal if datalogging is enabled")
SCRUTINY_OPTION(SCRUTINY_REQUEST_MAX_PROCESS_TIME_US    100000      STRING  "Maximum time allowed to process a request (us)")
SCRUTINY_OPTION(SCRUTINY_COMM_RX_TIMEOUT_US             50000       STRING  "Maximum time between reception of 2 consecutive byte (us)")
SCRUTINY_OPTION(SCRUTINY_COMM_HEARTBEAT_TIMEOUT_US      5000000     STRING  "Maximum time without communication before closing the session (us)")
SCRUTINY_OPTION(SCRUTINY_PROTOCOL_VERSION_MAJOR         1           STRING  "Protocol version major number")
SCRUTINY_OPTION(SCRUTINY_PROTOCOL_VERSION_MINOR         0           STRING  "Protocol version minor")
SCRUTINY_OPTION(SCRUTINY_DATALOGGING_ENCODING           SCRUTINY_DATALOGGING_ENCODING_RAW   STRING  "Datalogging encoding scheme")

SCRUTINY_OPTION(INSTALL_FOLDER  ${CMAKE_CURRENT_BINARY_DIR}/install STRING "Install folder" )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (SCRUTINY_USE_ASAN)
    add_compile_options(-fsanitize=address)
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -fsanitize=address")
endif()

add_subdirectory(lib)
include(lib/cmake/scrutiny.cmake)    # Make available for project importing us with FetchContent

if (SCRUTINY_BUILD_CWRAPPER)
    add_subdirectory(cwrapper)
endif()
add_subdirectory(projects)

if (SCRUTINY_BUILD_TEST)
    add_subdirectory(test)
endif()
