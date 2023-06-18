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
        ${CMAKE_CURRENT_SOURCE_DIR}/src/datalogging/datalogging.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/datalogging/scrutiny_datalogger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/datalogging/scrutiny_datalogging_trigger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/datalogging/scrutiny_datalogger_raw_encoder.cpp
    )
endif()


if (${SCRUTINY_ENABLE_DATALOGGING})
    message(STATUS "Scrutiny datalogging is ENABLED")
    target_compile_definitions(${PROJECT_NAME} PUBLIC SCRUTINY_ENABLE_DATALOGGING=1)
else()
    message(STATUS "Scrutiny datalogging is DISABLED")
    target_compile_definitions(${PROJECT_NAME} PUBLIC SCRUTINY_ENABLE_DATALOGGING=0
)
endif()

if (${SCRUTINY_SUPPORT_64BITS})
    message(STATUS "Scrutiny support for 64bits is ENABLED")
    target_compile_definitions(${PROJECT_NAME} PUBLIC SCRUTINY_SUPPORT_64BITS=1)
else()
    message(STATUS "Scrutiny support for 64bits is DISABLED")
    target_compile_definitions(${PROJECT_NAME} PUBLIC SCRUTINY_SUPPORT_64BITS=0)
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE )
else()
     target_compile_options(${PROJECT_NAME} PRIVATE -Werror )
endif()


install(
    TARGETS scrutiny-embedded DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/install 
    PUBLIC_HEADER DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/install 
    )