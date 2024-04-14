
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

set(PROJECT_NAME_UNTAGGED ${PROJECT_NAME}_untagged)
add_executable(${PROJECT_NAME_UNTAGGED} 
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME_UNTAGGED} PRIVATE 
    ArduinoCore
    scrutiny-embedded
    )
    
arduino_avr_hex(${PROJECT_NAME_UNTAGGED})

# Generate the SFD file
set(WORKDIR ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_sfd)
add_custom_command(
    TARGET ${PROJECT_NAME_UNTAGGED}
    DEPENDS ${PROJECT_NAME_UNTAGGED}.elf
    BYPRODUCTS ${PROJECT_NAME}.sfd 
        COMMAND ${CMAKE_COMMAND} -E make_directory ${WORKDIR}
        COMMAND scrutiny elf2varmap ${PROJECT_NAME_UNTAGGED}.elf --output ${WORKDIR} --loglevel error
        COMMAND scrutiny get-firmware-id ${PROJECT_NAME_UNTAGGED}.elf --output ${WORKDIR} 
        COMMAND scrutiny tag-firmware-id ${PROJECT_NAME_UNTAGGED}.elf ${PROJECT_NAME}.elf
        COMMAND scrutiny make-metadata --output ${WORKDIR} --project-name ${PROJECT_NAME} --version "V1.0.0" --author "Pier-Yves Lessard"
        # scrutiny add-alias ${WORKDIR} --file alias1.json
        # scrutiny add-alias ${WORKDIR} --file alias2.json
        COMMAND scrutiny make-sfd ${WORKDIR} ${PROJECT_NAME}.sfd
    )