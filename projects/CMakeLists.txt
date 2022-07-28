cmake_minimum_required(VERSION 3.14)

option(SCRUTINY_BUILD_TESTAPP "Build the test application" OFF)

if (SCRUTINY_BUILD_TESTAPP)
    add_subdirectory(testapp)
endif()