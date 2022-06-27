cmake_minimum_required(VERSION 3.14)

option(BUILD_TESTAPP "Build the test application" OFF)

if (BUILD_TESTAPP)
    add_subdirectory(testapp)
endif()