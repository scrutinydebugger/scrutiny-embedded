if not exist build mkdir build

cmake ^
    -DCMAKE_BUILD_TYPE=Release^
    -DBUILD_TESTAPP=ON^
    -DBUILD_TEST=ON^
    -S . -B build || goto ERROR

cmake --build build -j 4  || goto ERROR
@goto END

:ERROR
@echo Cannot build
@EXIT /B 1

:END
@EXIT /B 0
