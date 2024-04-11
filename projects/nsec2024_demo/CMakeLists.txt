
cmake_minimum_required(VERSION 3.16)

project(scrutiny-nsec2024)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION 0)  # Prevent adding -flto, incompatible with -g 

include(cmake/arduino_core.cmake)
include(cmake/arduino_hex.cmake)
include(cmake/arduino_upload.cmake)
include(cmake/scrutiny-postbuild.cmake)

add_subdirectory(3rdparty)

add_executable(${PROJECT_NAME} 
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${THIRDPARTY_INSTALL_DIR}/scrutiny/inc
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Os
    -Wall
    -Wextra
    -pedantic
    -Werror
    -gdwarf-4
)

add_dependencies(${PROJECT_NAME} scrutiny-embedded)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    ArduinoCore
    ${THIRDPARTY_INSTALL_DIR}/scrutiny/libscrutiny-embedded.a
    )
    
arduino_avr_hex(${PROJECT_NAME})
#scrutiny_generate_sfd()