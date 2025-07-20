#    scrutiny.cmake
#        Scrutiny CMake tools
#
#   - License : MIT - See LICENSE file.
#   - Project :  Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
#
#   Copyright (c) 2021 Scrutiny Debugger

function (scrutiny_postbuild TARGET)

    # Scrutiny Embedded Post-Build stage
    # Produces Scrutiny Firmware Description (.sfd) file & and copy of the 
    # executable injected with the firmware ID (hash of the file)

    set(options OPTIONAL 
        INSTALL_SFD     # The created SFD will be installed locally after being created
    )

    set(oneValueArgs 
        WORK_FOLDER     # OPTIONAL: The temporary folder used to build the SFD file. 
        SCRUTINY_CMD    # OPTIONAL: Path to the scrutiny binary
        SFD_FILENAME    # OPTIONAL: Name to give to the .sfd file

        TAGGED_EXECUTABLE_TARGET_VAR    # OPTIONAL: The output variable that will store the CMake target name of the tagged binary
        TAGGED_EXECUTABLE_NAME          # OPTIONAL: Filename of the tagged binary
        SFD_TARGET_VAR                  # OPTIONAL: The output variable that will store the CMake target name of the SFD file

        METADATA_PROJECT_NAME   # OPTIONAL: The name of the project, embedded in the .sfd file
        METADATA_AUTHOR         # OPTIONAL: The name of the project author, embedded in the .sfd file
        METADATA_VERSION        # OPTIONAL: The version of the project, embedded in the .sfd file
        
        CPPFILT                 # OPTIONAL: The binary to use for demangling. Default to "c++filt" if not provided
    )
    set(multiValueArgs 
        ALIAS_FILES             # OPTIONAL: List of file containing an alias definition. Will eb embedded in the .sfd
        CU_IGNORE_PATTERNS      # OPTIONAL: A list of compile unit to ignore. It can be the file basename or a file path with a glob pattern
        PATH_IGNORE_PATTERNS    # OPTIONAL: A list of variable path to ignore. Specified as a glob pattern
    )
    cmake_parse_arguments(PARSE_ARGV 0 arg "${options}" "${oneValueArgs}" "${multiValueArgs}")

    # Find scrutiny
    if (NOT arg_SCRUTINY_CMD)
        find_program(arg_SCRUTINY_CMD "scrutiny" REQUIRED)
    else()
        # Do nothing. Trust the user. He could apss "python -m scuritny"
    endif()
    message(STATUS "Using scrutiny at ${arg_SCRUTINY_CMD}")

    if (NOT arg_CPPFILT)
        find_program(arg_CPPFILT "c++filt" REQUIRED)
    else()
        find_program(arg_CPPFILT "c++filt" REQUIRED HINTS ${arg_CPPFILT})
    endif()
    
    # Validate work folder
    if (NOT arg_WORKDIR)
        set(arg_WORKDIR ${CMAKE_CURRENT_BINARY_DIR}/_scrutiny_${TARGET}_sfd_workfolder) # Default value
    else()
        if (NOT IS_ABSOLUTE ${arg_WORKDIR})
            set(arg_WORKDIR ${CMAKE_CURRENT_BINARY_DIR}/${arg_WORKDIR})
        else()
            # Try to prevent mistake because we do a "rm -rf" further
            if (NOT ${arg_WORKDIR} MATCHES "^${CMAKE_BINARY_DIR}.+")
                message(SEND_ERROR "WORKDIR must be inside CMAKE_BINARY_DIR" )
            endif()
        endif()
    endif()

    # Validate SFD filename
    if (NOT arg_SFD_FILENAME)
        set(arg_SFD_FILENAME ${TARGET}.sfd) # Default value
    endif()
    if (NOT IS_ABSOLUTE ${arg_SFD_FILENAME})
        # Try to put next to the .elf
        if (CMAKE_RUNTIME_OUTPUT_DIRECTORY)
            set(arg_SFD_FILENAME ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${arg_SFD_FILENAME})
        else()
            set(arg_SFD_FILENAME ${CMAKE_CURRENT_BINARY_DIR}/${arg_SFD_FILENAME})
        endif()
    endif()

    # Catch potential abd argument and report them
    get_target_property(TARGET_TYPE ${TARGET} TYPE)
    if (NOT TARGET_TYPE STREQUAL "EXECUTABLE")
        message(SEND_ERROR "${TARGET} msut be an executable. Got : ${TARGET_TYPE}")
    endif ()
    
    # Give the target we created to the caller
    get_target_property(TARGET_SUFFIX ${TARGET} SUFFIX)
    set(TAGGED_EXECUTABLE_TARGET ${TARGET}_tagged)
    if (arg_TAGGED_EXECUTABLE_TARGET_VAR)
        set(${arg_TAGGED_EXECUTABLE_TARGET_VAR} ${TAGGED_EXECUTABLE_TARGET} PARENT_SCOPE)
    endif()

    # Tagged executable validation
    if (NOT arg_TAGGED_EXECUTABLE_NAME)
        set(arg_TAGGED_EXECUTABLE_NAME ${TARGET}-tagged)    # Default value
        if (TARGET_SUFFIX)  # Apply the same suffix as the source
            set(arg_TAGGED_EXECUTABLE_NAME ${arg_TAGGED_EXECUTABLE_NAME}${TARGET_SUFFIX})
        endif()
    endif()

    # If relative path, try to place next to the binary
    if (NOT IS_ABSOLUTE ${arg_TAGGED_EXECUTABLE_NAME})
        if (CMAKE_RUNTIME_OUTPUT_DIRECTORY)
            set(arg_TAGGED_EXECUTABLE_NAME ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${arg_TAGGED_EXECUTABLE_NAME})
        else()
            set(arg_TAGGED_EXECUTABLE_NAME ${CMAKE_CURRENT_BINARY_DIR}/${arg_TAGGED_EXECUTABLE_NAME})
        endif()
    endif()

    # SFD
    set(SFD_TARGET ${TARGET}_sfd)
    if (arg_SFD_TARGET_VAR)
        set(${arg_SFD_TARGET_VAR} ${SFD_TARGET} PARENT_SCOPE)
    endif()    

    # Metadata
    set(METADATA_ARGS "")
    if (arg_METADATA_PROJECT_NAME)
        set(METADATA_ARGS ${METADATA_ARGS} --project-name ${arg_METADATA_PROJECT_NAME})
    endif()
    if (arg_METADATA_AUTHOR)
        set(METADATA_ARGS ${METADATA_ARGS} --author ${arg_METADATA_AUTHOR})
    endif()
    if (arg_METADATA_VERSION)
        set(METADATA_ARGS ${METADATA_ARGS} --version ${arg_METADATA_VERSION})
    endif()

    # Convert alias file path to absolute path relative to source
    set(ALIAS_LIST_ABS "")
    foreach(ALIAS_FILE ${arg_ALIAS_FILES})
        if (NOT IS_ABSOLUTE ${ALIAS_FILE})
            set(ALIAS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${ALIAS_FILE})
        endif()
        list(APPEND ALIAS_LIST_ABS ${ALIAS_FILE})
    endforeach()

    list(LENGTH ALIAS_LIST_ABS ALIAS_COUNT)     # Count the alias file to skip that step if 0

    # --- Make the SFD ---
    
    message("arg_INSTALL_SFD = ${arg_INSTALL_SFD}")

    add_custom_command(OUTPUT ${arg_SFD_FILENAME}
        DEPENDS ${TARGET} ${ALIAS_LIST_ABS}
        COMMAND ${CMAKE_COMMAND} -E echo "Generating Scrutiny Firmware Description for ${TARGET}"
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${arg_WORKDIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${arg_WORKDIR}
        COMMAND ${arg_SCRUTINY_CMD} 
            elf2varmap $<TARGET_FILE:${TARGET}> 
            --output ${arg_WORKDIR} 
            --loglevel error 
            --cu_ignore_patterns ${arg_CU_IGNORE_PATTERNS}
            --path_ignore_patterns ${arg_PATH_IGNORE_PATTERNS}
            --cppfilt ${arg_CPPFILT}
        COMMAND ${arg_SCRUTINY_CMD} get-firmware-id $<TARGET_FILE:${TARGET}> --output ${arg_WORKDIR} 
        COMMAND ${arg_SCRUTINY_CMD} make-metadata --output ${arg_WORKDIR} ${METADATA_ARGS}
        COMMAND ${arg_SCRUTINY_CMD} $<IF:$<NOT:$<EQUAL:${ALIAS_COUNT},0>>,add-alias,noop> ${arg_WORKDIR} --file ${ALIAS_LIST_ABS}
        COMMAND ${arg_SCRUTINY_CMD} make-sfd ${arg_WORKDIR} ${arg_SFD_FILENAME} $<arg_INSTALL_SFD:--install>
        #COMMAND ${arg_SCRUTINY_CMD} $<IF:$<BOOL:arg_INSTALL_SFD>,install-sfd,noop> ${arg_SFD_FILENAME}
    )
    add_custom_target(${SFD_TARGET} ALL DEPENDS ${arg_SFD_FILENAME})
    set_target_properties(${SFD_TARGET} PROPERTIES TARGET_FILE ${arg_SFD_FILENAME}) 
    

    # --- Make the tagged binary ---
    add_custom_command(OUTPUT ${arg_TAGGED_EXECUTABLE_NAME}
        DEPENDS ${TARGET}
        COMMAND ${arg_SCRUTINY_CMD} tag-firmware-id $<TARGET_FILE:${PROJECT_NAME}> ${arg_TAGGED_EXECUTABLE_NAME}
    )
    add_custom_target(${TAGGED_EXECUTABLE_TARGET} ALL DEPENDS ${arg_TAGGED_EXECUTABLE_NAME})
    set_target_properties(${TAGGED_EXECUTABLE_TARGET} PROPERTIES TARGET_FILE ${arg_TAGGED_EXECUTABLE_NAME})
    
endfunction()