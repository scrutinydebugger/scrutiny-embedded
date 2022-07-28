cmake_minimum_required(VERSION 3.14)
project(testapp)

option(SCRUTINY_WERR "Converts all warning to error" OFF)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/file1.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/file2.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
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

if(MSVC)    # MSVC doesn't support dwarf output. We still allows build of testapp.
    target_compile_options(${PROJECT_NAME} 
        PRIVATE
            /W4 /Od)

    if(SCRUTINY_WERR)
        target_compile_options(${PROJECT_NAME} PRIVATE /WX)
    endif()

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
    
    if(SCRUTINY_WERR)
        target_compile_options(${PROJECT_NAME} PRIVATE -Werror )
    endif()

    target_link_options(${PROJECT_NAME}
        PUBLIC
            -no-pie
            -gdwarf-4
        )
endif()



