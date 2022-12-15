cmake_minimum_required(VERSION 3.14)

option(SCRUTINY_BUILD_TEST "Activate test suite" ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib)
add_subdirectory(projects)

# ===============
# Tests

if (SCRUTINY_BUILD_TEST)
    add_subdirectory(test)
endif()
