# Copyright (c) 2021-2023 Scrutiny Debugger
# License : MIT - See LICENSE file.
# Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)

cmake_minimum_required(VERSION 3.14)

set(GTEST_RELEASE release-1.12.1)       # Last one to support C++11
if (CMAKE_CXX_STANDARD EQUAL 98)
    set(GTEST_RELEASE release-1.8.1)    # Last one to support C++98
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG ${GTEST_RELEASE}
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GMOCK OFF)
set(INSTALL_GTEST OFF)
FetchContent_MakeAvailable(googletest)

enable_testing()

project(scrutiny_unittest)
add_executable(${PROJECT_NAME}
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
    gtest_main
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

include(GoogleTest)

if (NOT ${CMAKE_CROSSCOMPILING})
    gtest_discover_tests(${PROJECT_NAME})
endif()
