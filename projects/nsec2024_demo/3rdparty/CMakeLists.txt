cmake_minimum_required(VERSION 3.16)
include(ExternalProject)

set(THIRDPARTY_INSTALL_DIR ${CMAKE_BINARY_DIR}/3rdparty/install)
set(THIRDPARTY_INSTALL_DIR ${THIRDPARTY_INSTALL_DIR} PARENT_SCOPE)



set(SCRUTINY_PREFIX ${CMAKE_BINARY_DIR}/scrutiny)

ExternalProject_Add(scrutiny-embedded
    GIT_REPOSITORY https://github.com/scrutinydebugger/scrutiny-embedded
    GIT_TAG nsec_demo #v1.0     # Git Tag, branch or commit hash
    PREFIX ${SCRUTINY_PREFIX}
    BINARY_DIR ${SCRUTINY_PREFIX}/build
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DSCRUTINY_ENABLE_DATALOGGING=1
        -DSCRUTINY_SUPPORT_64BITS=1
        -DINSTALL_FOLDER=${THIRDPARTY_INSTALL_DIR}/scrutiny
    BUILD_BYPRODUCTS
        ${THIRDPARTY_INSTALL_DIR}/scrutiny/libscrutiny-embedded.a
)
