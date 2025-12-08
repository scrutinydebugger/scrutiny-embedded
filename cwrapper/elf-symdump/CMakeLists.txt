cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE EXECUTABLE)

message("====> CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
message("====> CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")
message("====> CMAKE_C_HOST_COMPILER=${CMAKE_C_HOST_COMPILER}")

option(SCRUTINY_ELF_SYMDUMP_STRICT_BUILD "Build with strict options" ON)

project(scrutiny-elf-symdump)

add_executable(${PROJECT_NAME} 
    src/main.c
    src/elf.c
    src/cmd.c
)

if (SCRUTINY_ELF_SYMDUMP_STRICT_BUILD)
    if (MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4 /Od /WX )
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra -Wpedantic )
    endif()
endif()

target_include_directories(${PROJECT_NAME} PRIVATE inc)

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION bin
    )