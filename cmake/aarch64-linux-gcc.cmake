find_program(GCC aarch64-linux-gnu-gcc REQUIRED)
find_program(GXX aarch64-linux-gnu-g++ REQUIRED)

set(CMAKE_TRY_COMPILE_TARGET_TYPE EXECUTABLE)
set(CMAKE_SYSTEM_NAME Linux)  # Needs to be set explicitely for CMAKE_CROSSCOMPILING to be True
set(CMAKE_SYSTEM_PROCESSOR aarch64) 

set(CMAKE_C_COMPILER ${GCC})
set(CMAKE_CXX_COMPILER ${GXX})
