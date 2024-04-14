
cmake_minimum_required(VERSION 3.16)

project(scrutiny-nsec2024)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION 0)  # Prevent adding -flto, incompatible with -g 

include(cmake/arduino_core.cmake)
include(cmake/arduino_hex.cmake)
include(cmake/arduino_upload.cmake)

include(FetchContent)

set(SCRUTINY_ENABLE_DATALOGGING 1)
set(SCRUTINY_SUPPORT_64BITS 1)

FetchContent_Declare(
  scrutiny-embedded

  GIT_REPOSITORY https://github.com/scrutinydebugger/scrutiny-embedded.git
  GIT_TAG        nsec_demo
)
FetchContent_MakeAvailable(scrutiny-embedded)

add_compile_options(
    -Os
    -Wall 
    -Wextra 
    -pedantic 
    -Werror 
    -gdwarf-4
)

target_link_libraries(scrutiny-embedded PUBLIC ArduinoCore) # Will inherit the build flags

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/scrutiny_integration.cpp
)
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX "_untagged.elf")

target_link_libraries(${PROJECT_NAME} PRIVATE 
    ArduinoCore
    scrutiny-embedded
    )


set(WORKDIR ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_sfd)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    BYPRODUCTS ${PROJECT_NAME}.sfd  ${PROJECT_NAME}.elf
    COMMAND ${CMAKE_COMMAND} -E make_directory ${WORKDIR}
    COMMAND scrutiny elf2varmap $<TARGET_FILE:${PROJECT_NAME}> --output ${WORKDIR} --loglevel error
    COMMAND scrutiny get-firmware-id $<TARGET_FILE:${PROJECT_NAME}> --output ${WORKDIR} 
    COMMAND scrutiny tag-firmware-id $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.elf
    COMMAND scrutiny make-metadata --output ${WORKDIR} --project-name ${PROJECT_NAME} --version "V1.0.0" --author "Pier-Yves Lessard"
    # scrutiny add-alias ${WORKDIR} --file alias1.json
    # scrutiny add-alias ${WORKDIR} --file alias2.json
    COMMAND scrutiny make-sfd ${WORKDIR} ${PROJECT_NAME}.sfd
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    BYPRODUCTS ${PROJECT_NAME}.eep
    COMMAND ${CMAKE_OBJCOPY} ARGS 
        -O ihex -j .eeprom
        --set-section-flags=.eeprom=alloc,load
        --no-change-warnings --change-section-lma 
        .eeprom=0
        ${PROJECT_NAME}.elf
        ${PROJECT_NAME}.eep)

add_custom_command(TARGET ${PROJECT_NAME}  POST_BUILD
    DEPENDS ${PROJECT_NAME}.elf
    BYPRODUCTS ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} ARGS 
        -O ihex -R .eeprom
        ${PROJECT_NAME}.elf
        ${PROJECT_NAME}.hex
)
