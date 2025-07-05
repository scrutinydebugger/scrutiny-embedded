# Copyright (c) 2021-2023 Scrutiny Debugger
# License : MIT - See LICENSE file.
# Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)

cmake_minimum_required(VERSION 3.14)

project(scrutiny-embedded)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scrutiny_main_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scrutiny_loop_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scrutiny_software_id.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scrutiny_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scrutiny_common_codecs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scrutiny_tools.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/scrutiny_comm_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/scrutiny_codec_v1_0.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/scrutiny_protocol_definitions.cpp    
)

if (SCRUTINY_ENABLE_DATALOGGING)
    target_sources(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/datalogging/scrutiny_datalogging.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/datalogging/scrutiny_datalogger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/datalogging/scrutiny_datalogging_trigger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/datalogging/scrutiny_datalogger_raw_encoder.cpp
    )
endif()

if (${SCRUTINY_ENABLE_DATALOGGING})
    message(STATUS "Scrutiny datalogging is ENABLED")
else()
    message(STATUS "Scrutiny datalogging is DISABLED")
endif()

if (${SCRUTINY_SUPPORT_64BITS})
    message(STATUS "Scrutiny support for 64bits is ENABLED")
else()
    message(STATUS "Scrutiny support for 64bits is DISABLED")
endif()

set(SCRUTINY_REQUEST_MAX_PROCESS_TIME_US 100000  CACHE STRING "Maximum time allowed to process a request (us)")
set(SCRUTINY_COMM_RX_TIMEOUT_US 50000 CACHE STRING "Maximum time between reception of 2 consecutive byte (us)")
set(SCRUTINY_COMM_HEARTBEAT_TIMEOUT_US 5000000 CACHE STRING "Maximum time without communication before closing the session (us)")
set(SCRUTINY_PROTOCOL_VERSION_MAJOR 1 CACHE STRING "Protocol version major number")
set(SCRUTINY_PROTOCOL_VERSION_MINOR 0 CACHE STRING "Protocol version minor")
set(SCRUTINY_DATALOGGING_MAX_SIGNAL 32 CACHE STRING "Maximum number of datalogging signal if datalogging is enabled")
set(SCRUTINY_DATALOGGING_ENCODING  SCRUTINY_DATALOGGING_ENCODING_RAW CACHE STRING "Datalogging encoding scheme")
set(SCRUTINY_DATALOGGING_BUFFER_32BITS  OFF CACHE STRING "Allow datalogging buffers bigger than 65536 bytes")
 
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/scrutiny_build_config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/configured/scrutiny_build_config.hpp
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Werror )
endif()


# ----- INSTALL -----
install(
    DIRECTORY inc 
    DESTINATION ${INSTALL_FOLDER}
    PATTERN "*.hpp.in" EXCLUDE
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/configured/scrutiny_build_config.hpp
    DESTINATION ${INSTALL_FOLDER}/inc
    )

install(
    FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/scrutiny.cmake
    DESTINATION ${INSTALL_FOLDER}
    )

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc/>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/configured/>
    $<INSTALL_INTERFACE:inc>
)

install(
    TARGETS ${PROJECT_NAME} 
    ARCHIVE DESTINATION ${INSTALL_FOLDER} 
)

