find_program(GCC avr-gcc REQUIRED)
find_program(GXX avr-g++ REQUIRED)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_SYSTEM_NAME Generic)  # Needs to be set explicitely for CMAKE_CROSS_COMPILING to be True
set(CMAKE_SYSTEM_PROCESSOR avr) 

set(CMAKE_C_COMPILER ${GCC})
set(CMAKE_CXX_COMPILER ${GXX})
 