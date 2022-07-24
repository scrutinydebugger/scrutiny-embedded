cmake_minimum_required(VERSION 3.14)
project(testapp)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/file1.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/file2.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/argument_parser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/udp_bridge.cpp
        )  

target_link_libraries(${PROJECT_NAME}
    scrutiny-embedded   
    )

if (WIN32)
    target_link_libraries(${PROJECT_NAME} 
        wsock32
    )
endif (WIN32)


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



