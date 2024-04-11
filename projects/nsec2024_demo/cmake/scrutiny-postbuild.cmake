function(scrutiny_generate_sfd)
    set(WORKDIR ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_sfd)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        DEPENDS ${PROJECT_NAME}.elf
        BYPRODUCTS ${PROJECT_NAME}.sfd 
            COMMAND ${CMAKE_COMMAND} -E make_directory ${WORKDIR}
            COMMAND scrutiny elf2varmap ${PROJECT_NAME}.elf --output ${WORKDIR}
            COMMAND scrutiny get-firmware-id ${PROJECT_NAME}.elf --output ${WORKDIR}
            COMMAND scrutiny tag-firmware-id ${PROJECT_NAME}.elf --inplace
            COMMAND scrutiny make-metadata --output ${WORKDIR} --project-name ${PROJECT_NAME} --version "V1.2.3" --author "ACME inc."
           # scrutiny add-alias ${WORKDIR} --file alias1.json
           # scrutiny add-alias ${WORKDIR} --file alias2.json
           COMMAND scrutiny make-sfd ${WORKDIR} ${PROJECT_NAME}.sfd
        )

endfunction()