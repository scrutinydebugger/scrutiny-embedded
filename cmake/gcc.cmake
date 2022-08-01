find_program(GCC gcc REQUIRED)
find_program(GXX g++ REQUIREDXX)

set(CMAKE_C_COMPILER ${GCC})
set(CMAKE_CXX_COMPILER ${GXX})
