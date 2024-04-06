# Enter CMake cross-compilation mode
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

# User settings with sensible defaults
set(ARDUINO_PATH "/usr/share/arduino" CACHE PATH "Path of the Arduino packages folder")

# Derived paths
set(ARDUINO_AVR_PATH ${ARDUINO_PATH}/hardware/arduino/avr)
set(ARDUINO_CORE_PATH ${ARDUINO_AVR_PATH}/cores/arduino)
set(ARDUINO_AVRDUDE_PATH ${ARDUINO_PATH}/hardware/tools/avrdude)
set(ARDUINO_AVRDUDE_CONF ${ARDUINO_PATH}/hardware/tools/avrdude.conf)

# Toolchain paths
set(CMAKE_C_COMPILER avr-gcc CACHE FILEPATH "Path to avr-gcc" FORCE)
set(CMAKE_CXX_COMPILER avr-g++ CACHE FILEPATH "Path to avr-g++" FORCE)
set(CMAKE_OBJCOPY avr-objcopy CACHE FILEPATH "Path to avr-objcopy" FORCE)
set(CMAKE_SIZE avr-size CACHE FILEPATH "Path to avr-size" FORCE)
set(ARDUINO_AVRDUDE avrdude CACHE FILEPATH "Path to avrdude" FORCE)

# Only look libraries etc. in the sysroot, but never look there for programs
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
