find_program(GCC arm-none-eabi-gcc REQUIRED)
find_program(GXX arm-none-eabi-g++ REQUIRED)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_SYSTEM_NAME Generic)  # Needs to be set explicitely for CMAKE_CROSS_COMPILING to be True
set(CMAKE_SYSTEM_PROCESSOR arm) 

set(CMAKE_C_COMPILER ${GCC})
set(CMAKE_CXX_COMPILER ${GXX})
