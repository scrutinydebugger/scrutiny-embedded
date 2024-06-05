if not exist build-dev mkdir build-dev

cmake ^
    -DCMAKE_BUILD_TYPE=Release^
    -DSCRUTINY_BUILD_TESTAPP=ON^
    -DSCRUTINY_BUILD_TEST=ON^
    -Wno-dev^
    -S . -B build-dev || goto ERROR

cmake --build build-dev -j 4  || goto ERROR
@goto END

:ERROR
@echo Cannot build
@EXIT /B 1

:END
@EXIT /B 0
