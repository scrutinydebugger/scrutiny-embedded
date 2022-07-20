cmake_minimum_required(VERSION 3.14)
project(testapp)

set(SRC_DIR  ${CMAKE_CURRENT_LIST_DIR}/src)
set(INCLUDE_DIR  ${CMAKE_CURRENT_LIST_DIR}/include)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
        ${SRC_DIR}/file1.cpp
        ${SRC_DIR}/file2.cpp
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/argument_parser.cpp
        ${SRC_DIR}/comm_channels/udp_bridge.cpp
        )  

if (WIN32)
    target_link_libraries(${PROJECT_NAME} 
        wsock32
    )

    target_sources(${PROJECT_NAME} PRIVATE
        ${SRC_DIR}/comm_channels/win_serial_port_bridge.cpp
    )
else()
    target_sources(${PROJECT_NAME} PRIVATE
        ${SRC_DIR}/comm_channels/nix_serial_port_bridge.cpp
    )
endif ()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/comm_channels
)

target_link_libraries(${PROJECT_NAME}
    scrutiny-embedded   
    )

if(MSVC)    # MSVC doesn't support dwarf output. We still allows build of testapp.
    target_compile_options(${PROJECT_NAME} 
        PRIVATE
            /W4 /Od)

    target_link_options(${PROJECT_NAME}
        PUBLIC
            /DYNAMICBASE:NO
        )
else()
    target_compile_options(${PROJECT_NAME}
        PRIVATE
            -no-pie  
            -gdwarf-4
            -O0
            -Wall -Wextra -Wpedantic
        )

    target_link_options(${PROJECT_NAME}
        PUBLIC
            -no-pie
            -gdwarf-4
        )
endif()



