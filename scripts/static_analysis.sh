#!/bin/bash
set -euo pipefail

LIB_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/../lib >/dev/null 2>&1 && pwd )"

CPPCHECK_ARGS="-I $LIB_ROOT/inc $LIB_ROOT \
    --error-exitcode=-1 \
    --enable=all    \
    --suppress=memsetClassFloat   \
    --suppress=missingIncludeSystem   \
    --suppress=unusedFunction   \
    --suppress=missingIncludeSystem \
    --suppress=preprocessorErrorDirective \
    --suppress=unmatchedSuppression \
    --inline-suppr"

cppcheck $CPPCHECK_ARGS -DSCRUTINY_ENABLE_DATALOGGING=0 -DSCRUTINY_SUPPORT_64BITS=0
cppcheck $CPPCHECK_ARGS -DSCRUTINY_ENABLE_DATALOGGING=0 -DSCRUTINY_SUPPORT_64BITS=1
cppcheck $CPPCHECK_ARGS -DSCRUTINY_ENABLE_DATALOGGING=1 -DSCRUTINY_SUPPORT_64BITS=0
cppcheck $CPPCHECK_ARGS -DSCRUTINY_ENABLE_DATALOGGING=1 -DSCRUTINY_SUPPORT_64BITS=1
