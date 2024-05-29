# Copyright (c) 2021 Scrutiny Debugger
# License : MIT - See LICENSE file.
# Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)

cmake_minimum_required(VERSION 3.14)
project(scrutiny-cwrapper)

add_library(${PROJECT_NAME} 
    scrutiny_cwrapper.cpp
)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

target_include_directories(${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:cwrapper>
    )

target_link_libraries(${PROJECT_NAME} scrutiny-embedded)


if (GENERATE_SIZE_HEADERS)
set(GENERATED_HEADER_NAME ${CMAKE_CURRENT_BINARY_DIR}/generated/scrutiny_c_sizes.hpp)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    BYPRODUCTS GENERATED_HEADER_NAME
    COMMAND ${CMAKE_OBJDUMP}
)
endif()

# ----- INSTALL -----
install(FILES "scrutiny_cwrapper.h" 
    DESTINATION ${INSTALL_FOLDER}/cwrapper
)

install(
    TARGETS ${PROJECT_NAME} 
    ARCHIVE DESTINATION ${INSTALL_FOLDER}/cwrapper
)