
cmake_minimum_required(VERSION 3.16)

project(scrutiny-nsec2024)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(cmake/lto.cmake)
include(cmake/arduino_core.cmake)
include(cmake/arduino_hex.cmake)
include(cmake/arduino_upload.cmake)

add_executable(${PROJECT_NAME} 
    src/main.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
    "-Wall"
    "-Wextra"
    "-pedantic"
)

target_link_libraries(${PROJECT_NAME} PUBLIC ArduinoCore)
arduino_avr_hex(${PROJECT_NAME})