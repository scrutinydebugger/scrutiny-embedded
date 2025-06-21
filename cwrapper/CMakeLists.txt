# Copyright (c) 2021 Scrutiny Debugger
# License : MIT - See LICENSE file.
# Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)

cmake_minimum_required(VERSION 3.14)
project(scrutiny-cwrapper)

add_library(${PROJECT_NAME} 
    scrutiny_cwrapper.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:cwrapper>
    )

target_link_libraries(${PROJECT_NAME} scrutiny-embedded)


# ----- INSTALL -----
install(FILES "scrutiny_cwrapper.h" 
    DESTINATION ${INSTALL_FOLDER}/cwrapper
)

install(
    TARGETS ${PROJECT_NAME} 
    ARCHIVE DESTINATION ${INSTALL_FOLDER}/cwrapper
)