if(NOT ARDUINO_PATH)
    message(FATAL_ERROR "Arduino-specific variables are not set. \
                         Did you select the right toolchain file?")
endif()

# Basic compilation flags
add_library(ArduinoFlags INTERFACE)
target_compile_options(ArduinoFlags INTERFACE
    "-fno-exceptions"
    "-ffunction-sections"
    "-fdata-sections"
    "$<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>"
    "-mmcu=${ARDUINO_MCU}"
)
target_compile_definitions(ArduinoFlags INTERFACE
    "F_CPU=${ARDUINO_F_CPU}"
    "ARDUINO=${ARDUINO_VERSION}"
    "ARDUINO_${ARDUINO_BOARD}"
    "ARDUINO_ARCH_AVR"
)
target_link_options(ArduinoFlags INTERFACE
    "-mmcu=${ARDUINO_MCU}"
    "-fuse-linker-plugin"
    "LINKER:--gc-sections"
)
target_compile_features(ArduinoFlags INTERFACE cxx_std_11 c_std_11)

# Arduino Core
add_library(ArduinoCore STATIC
    ${ARDUINO_CORE_PATH}/abi.cpp
    ${ARDUINO_CORE_PATH}/CDC.cpp
    ${ARDUINO_CORE_PATH}/HardwareSerial0.cpp
    ${ARDUINO_CORE_PATH}/HardwareSerial1.cpp
    ${ARDUINO_CORE_PATH}/HardwareSerial2.cpp
    ${ARDUINO_CORE_PATH}/HardwareSerial3.cpp
    ${ARDUINO_CORE_PATH}/HardwareSerial.cpp
    ${ARDUINO_CORE_PATH}/IPAddress.cpp
    ${ARDUINO_CORE_PATH}/main.cpp
    ${ARDUINO_CORE_PATH}/new.cpp
    ${ARDUINO_CORE_PATH}/PluggableUSB.cpp
    ${ARDUINO_CORE_PATH}/Print.cpp
    ${ARDUINO_CORE_PATH}/Stream.cpp
    ${ARDUINO_CORE_PATH}/Tone.cpp
    ${ARDUINO_CORE_PATH}/USBCore.cpp
    ${ARDUINO_CORE_PATH}/WMath.cpp
    ${ARDUINO_CORE_PATH}/WString.cpp
    ${ARDUINO_CORE_PATH}/hooks.c
    ${ARDUINO_CORE_PATH}/WInterrupts.c
    ${ARDUINO_CORE_PATH}/wiring_analog.c
    ${ARDUINO_CORE_PATH}/wiring.c
    ${ARDUINO_CORE_PATH}/wiring_digital.c
    ${ARDUINO_CORE_PATH}/wiring_pulse.c
    ${ARDUINO_CORE_PATH}/wiring_shift.c
    ${ARDUINO_CORE_PATH}/wiring_pulse.S
)
target_link_libraries(ArduinoCore PUBLIC ArduinoFlags)
target_compile_features(ArduinoCore PUBLIC cxx_std_11 c_std_11)
target_include_directories(ArduinoCore PUBLIC 
    ${ARDUINO_CORE_PATH}
    ${ARDUINO_AVR_PATH}/variants/${ARDUINO_VARIANT}
)
if(ARDUINO_USB)
    target_compile_definitions(ArduinoCore PUBLIC
        USB_VID=${ARDUINO_USB_VID}
        USB_PID=${ARDUINO_USB_PID}
        USB_MANUFACTURER=\"Unknown\"
        USB_PRODUCT=\"${ARDUINO_USB_PRODUCT}\"
    )
endif()