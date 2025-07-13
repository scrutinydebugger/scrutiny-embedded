#!/bin/bash
set -euo pipefail

LIB_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/../lib >/dev/null 2>&1 && pwd )"

CPPCHECK_ARGS="-I $LIB_ROOT/inc $LIB_ROOT \
    --error-exitcode=-1
    --enable=all
    --std=c++11
    --suppress=memsetClassFloat
    --suppress=missingIncludeSystem
    --suppress=unusedFunction
    --suppress=missingIncludeSystem
    --suppress=preprocessorErrorDirective
    --suppress=unmatchedSuppression
    --inline-suppr
    --inconclusive
    -DSCRUTINY_STATIC_ANALYSIS"

PLATFORMS="unix32 unix64 win32A win32W win64 avr8 elbrus-e1cp pic8 pic8-enhanced pic16 mips32 native"
for PLATFORM in $PLATFORMS
do 
    echo "===== Platform: $PLATFORM ====="
    cppcheck $CPPCHECK_ARGS --platform=$PLATFORM -DSCRUTINY_ENABLE_DATALOGGING=0 -DSCRUTINY_SUPPORT_64BITS=0
    cppcheck $CPPCHECK_ARGS --platform=$PLATFORM -DSCRUTINY_ENABLE_DATALOGGING=0 -DSCRUTINY_SUPPORT_64BITS=1
    cppcheck $CPPCHECK_ARGS --platform=$PLATFORM -DSCRUTINY_ENABLE_DATALOGGING=1 -DSCRUTINY_SUPPORT_64BITS=0
    cppcheck $CPPCHECK_ARGS --platform=$PLATFORM -DSCRUTINY_ENABLE_DATALOGGING=1 -DSCRUTINY_SUPPORT_64BITS=1
done