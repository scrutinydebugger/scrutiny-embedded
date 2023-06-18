# Copyright (c) 2021-2023 Scrutiny Debugger
# License : MIT - See LICENSE file.
# Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)

cmake_minimum_required(VERSION 3.14)
project(testapp)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/file1.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/file2.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/include_test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/argument_parser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/comm_channels/udp_bridge.cpp
)  

if (WIN32)
    target_link_libraries(${PROJECT_NAME} 
        wsock32
    )

    target_sources(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/comm_channels/win_serial_port_bridge.cpp
    )
else()
    target_sources(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/comm_channels/nix_serial_port_bridge.cpp
    )
endif ()

target_include_directories(${PROJECT_NAME} PRIVATE
   ${CMAKE_CURRENT_LIST_DIR}/include
   ${CMAKE_CURRENT_LIST_DIR}/include/comm_channels
)

target_link_libraries(${PROJECT_NAME}
    scrutiny-embedded   
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")    # MSVC doesn't support dwarf output. We still allows build of testapp.
    target_compile_options(${PROJECT_NAME} PRIVATE 
        /W4 /Od /WX
    )

    target_link_options(${PROJECT_NAME} PUBLIC
        /DYNAMICBASE:NO
    )

else()
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

    target_compile_options(${PROJECT_NAME} PRIVATE
        ${COMPILER_NO_PIE_OPTION}  
        -gdwarf-4
        -O0
        -Wall -Wextra -Wpedantic -Werror
    )

    target_compile_options(scrutiny-embedded PRIVATE 
        -Werror 
        ${COMPILER_NO_PIE_OPTION}
    )

    target_link_options(${PROJECT_NAME} PUBLIC
        ${LINKER_NO_PIE_OPTION}
        -gdwarf-4
    )

endif()



