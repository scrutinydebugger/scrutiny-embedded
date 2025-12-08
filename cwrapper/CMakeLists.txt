# Copyright (c) 2021 Scrutiny Debugger
# License : MIT - See LICENSE file.
# Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)

cmake_minimum_required(VERSION 3.14)
project(scrutiny-cwrapper)

option(SCRUTINY_CWRAPPER_EXTRACT_CPP_CONSTANTS "Extract the C++ constants from the wrapper lib an create a header" ON)

add_library(${PROJECT_NAME}_OBJ OBJECT
    scrutiny_cwrapper.cpp
)

target_include_directories(${PROJECT_NAME}_OBJ PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:cwrapper>
    )

target_link_libraries(${PROJECT_NAME}_OBJ scrutiny-embedded)

add_library(${PROJECT_NAME} )
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_OBJ)

if (${SCRUTINY_CWRAPPER_EXTRACT_CPP_CONSTANTS})
    include(ExternalProject)
    set(SYMDUMP_PROJECT_NAME "scrutiny-elf-symdump")
    set(SYMDUMP_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/${SYMDUMP_PROJECT_NAME}" )
    ExternalProject_Add(${SYMDUMP_PROJECT_NAME}
        SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/elf-symdump 
        PREFIX ${SYMDUMP_PROJECT_NAME}
        CMAKE_ARGS
            -D CMAKE_TOOLCHAIN_FILE=""
            -D SCRUTINY_ELF_SYMDUMP_STRICT_BUILD=OFF     # For maximum compatibility in case of untested compiler
            -D CMAKE_INSTALL_PREFIX=${SYMDUMP_INSTALL_DIR}
    )

    set(ELF_SYMDUP ${SYMDUMP_INSTALL_DIR}/bin/${SYMDUMP_PROJECT_NAME})
    set(GENERATED_HEADER_DIR "${PROJECT_BINARY_DIR}/generated")
    set(CPP_CONSTANT_FILE "${GENERATED_HEADER_DIR}/scrutiny_cwrapper_cpp_constants.h")
    set(CPP_CONSTANTS SCRUTINY_C_MAIN_HANDLER_SIZE SCRUTINY_C_CONFIG_SIZE SCRUTINY_C_LOOP_HANDLER_FF_SIZE SCRUTINY_C_LOOP_HANDLER_VF_SIZE )
    add_custom_command(
        OUTPUT ${CPP_CONSTANT_FILE}
        DEPENDS ${SYMDUMP_PROJECT_NAME} ${PROJECT_NAME}_OBJ
        COMMAND ${ELF_SYMDUP} ARGS -i $<TARGET_OBJECTS:${PROJECT_NAME}_OBJ> -o ${CPP_CONSTANT_FILE}  ${CPP_CONSTANTS}
    )

    target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${GENERATED_HEADER_DIR}>
    )

    add_custom_target(SCRUTINY_CPP_CONSTANTS_TARGET
        DEPENDS ${CPP_CONSTANT_FILE}
    )
    add_dependencies(${PROJECT_NAME} SCRUTINY_CPP_CONSTANTS_TARGET)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SCRUTINY_C_USE_COMPILE_TIME_CONSTANTS)

    install(FILES ${CPP_CONSTANT_FILE} 
        DESTINATION ${INSTALL_FOLDER}/cwrapper
    )
endif()


# ----- INSTALL -----
install(FILES "scrutiny_cwrapper.h" 
    DESTINATION ${INSTALL_FOLDER}/cwrapper
)

install(
    TARGETS ${PROJECT_NAME} 
    ARCHIVE DESTINATION ${INSTALL_FOLDER}/cwrapper
)