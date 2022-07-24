cmake_minimum_required(VERSION 3.14)

option(BUILD_TEST "Activate test suite" ON)

add_subdirectory(lib)
add_subdirectory(projects)

# ===============
# Tests

if (BUILD_TEST)
    add_subdirectory(test)
endif()
