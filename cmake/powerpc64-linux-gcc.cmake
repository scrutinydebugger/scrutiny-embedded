find_program(GCC powerpc64-linux-gnu-gcc REQUIRED)
find_program(GXX powerpc64-linux-gnu-g++ REQUIRED)

set(CMAKE_C_COMPILER ${GCC})
set(CMAKE_CXX_COMPILER ${GXX})
set(CMAKE_SYSTEM_PROCESSOR powerpc64) 
set(CMAKE_SYSTEM_NAME Linux)    # Needs to be set explicitely for CMAKE_CROSS_COMPILING to be True

message(STATUS ${CMAKE_HOST_SYSTEM_NAME})