cmake_minimum_required(VERSION 3.14)
project(CMakeDemoProject)

include(ExternalProject)

# Where the includes and static lib will be written
set(SCRUTINY_DEST_FOLDER ${CMAKE_BINARY_DIR}/3rdparty/scrutiny)

# Fetch and build scrutiny
set(SCRUTINY_PREFIX ${CMAKE_BINARY_DIR}/scrutiny)
ExternalProject_Add(scrutiny-embedded
    GIT_REPOSITORY https://github.com/scrutinydebugger/scrutiny-embedded
    GIT_TAG v1.0     # Git Tag, branch or commit hash
    PREFIX ${SCRUTINY_PREFIX}
    BINARY_DIR ${SCRUTINY_PREFIX}/build
    CMAKE_ARGS
        -DSCRUTINY_ENABLE_DATALOGGING=1
        -DSCRUTINY_SUPPORT_64BITS=1
        -DSCRUTINY_BUILD_TEST=0
    INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} --install ${SCRUTINY_PREFIX}/build
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SCRUTINY_PREFIX}/build/install/ ${SCRUTINY_DEST_FOLDER}
    BUILD_BYPRODUCTS
        ${SCRUTINY_DEST_FOLDER}/libscrutiny-embedded.a
)

add_executable(${PROJECT_NAME} main.cpp)

# Make scrutiny a dependency to guarantee the build order.
add_dependencies(${PROJECT_NAME} scrutiny-embedded)

# Makes scrutiny available to the compiler (headers and static lib)
target_link_libraries(${PROJECT_NAME} ${SCRUTINY_DEST_FOLDER}/libscrutiny-embedded.a)
target_include_directories(${PROJECT_NAME} PRIVATE ${SCRUTINY_DEST_FOLDER}/inc)


