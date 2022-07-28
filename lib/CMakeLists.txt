cmake_minimum_required(VERSION 3.14)

option(SCRUTINY_WERR "Converts all warning to error" OFF)

project(scrutiny-embedded)
add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scrutiny_main_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scrutiny_loop_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scrutiny_crc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scrutiny_software_id.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scrutiny_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/scrutiny_comm_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/scrutiny_codec_v1_0.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/scrutiny_protocol_definitions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/protocol/scrutiny_protocol_tools.cpp
    )
    
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/
    )

if (MSVC)
    if(SCRUTINY_WERR)
        target_compile_options(${PROJECT_NAME} PRIVATE /WX)
    endif()
else()
    if(SCRUTINY_WERR)
        target_compile_options(${PROJECT_NAME} PRIVATE -werror )
    endif()
endif()