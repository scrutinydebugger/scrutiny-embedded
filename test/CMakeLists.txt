# Copyright (c) 2021-2023 Scrutiny Debugger
# License : MIT - See LICENSE file.
# Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)

cmake_minimum_required(VERSION 3.14)
include(FetchContent)
FetchContent_Declare(
  scrutinytest
  GIT_REPOSITORY https://github.com/scrutinydebugger/scrutiny-test-framework
  GIT_TAG 4ab9a5d9101601c2d8b54f38f40c4fd7dda042af
)

set(SCRUTINYTEST_DISABLE_EXCEPTIONS OFF)
set(SCRUTINYTEST_NO_DETAILS OFF)
set(SCRUTINYTEST_NO_OUTPUT OFF)
set(SCRUTINYTEST_MAX_TEST_CASES 1024)
set(SCRUTINYTEST_MAX_TEST_SUITES 128)

# For Windows: Prevent overriding the parent project's compiler/linker settings

FetchContent_MakeAvailable(scrutinytest)

project(scrutiny_unittest)
add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scrutiny_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_timebase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_crc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_ipc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_codecs.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol/test_protocol_rx_parsing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol/test_protocol_tx_parsing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol/test_comm_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/commands/test_get_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/commands/test_comm_control.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/commands/test_memory_control.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/commands/test_memory_control_rpv.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/commands/test_user_command.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/commands/test_datalog_control.cpp
    )

if (SCRUTINY_ENABLE_DATALOGGING)
    target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/datalogging/test_fetch_operands.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/datalogging/test_trigger_conditions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/datalogging/test_datalogging_types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/datalogging/test_datalogger.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/datalogging/test_raw_encoder.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/datalogging/raw_format_parser.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/test_variable_fetching.cpp 
    )
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
target_link_libraries(${PROJECT_NAME}
    scrutiny-embedded
    scrutinytest
    )

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(scrutiny-embedded PRIVATE /W4 /Od /WX)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /Od  /WX)
else()
    # We don't require no-pie here. But we need scrutiny-embedded to have the same parameter as this project.
    # So we need to set an explicit value, if we don't set no-pie or -pie (blank), cmake might chose to keep scrutiny-embedded
    # from cache, which might have no-pie because of testapp and decide to set -pie for this project; this will fail.
    # So we use no-pie to have an explicit value, and it will avoid recompilation if we use the same as testapp.

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(COMPILER_NO_PIE_OPTION "-fno-pie")
        set(LINKER_NO_PIE_OPTION "-no-pie") # clang use system linker.
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(COMPILER_NO_PIE_OPTION "-no-pie")
        set(LINKER_NO_PIE_OPTION "-no-pie")
    else()
        set(COMPILER_NO_PIE_OPTION "")
        set(LINKER_NO_PIE_OPTION "")
    endif()

    target_compile_options(scrutiny-embedded PRIVATE -Wall -Wextra -Wpedantic -Werror ${COMPILER_NO_PIE_OPTION})
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror ${COMPILER_NO_PIE_OPTION})
    target_link_options(${PROJECT_NAME} PUBLIC ${LINKER_NO_PIE_OPTION})
endif()
