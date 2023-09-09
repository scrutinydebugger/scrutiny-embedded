#!/bin/bash
set -euo pipefail

APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. >/dev/null 2>&1 && pwd )"

if [[ $(uname -s) == CYGWIN* ]];then
APP_ROOT=$(cygpath -w "$APP_ROOT")
fi

set -x 

BUILD_CONTEXT="${BUILD_CONTEXT:-dev}"
BUILD_DIR="$APP_ROOT/build-${BUILD_CONTEXT}"

mkdir -p "$BUILD_DIR"

#SCRUTINY_ENABLE_DATALOGGING=${SCRUTINY_ENABLE_DATALOGGING:-ON}
#SCRUTINY_SUPPORT_64BITS=${SCRUTINY_SUPPORT_64BITS:-ON}


SCRUTINY_BUILD_TEST=${SCRUTINY_BUILD_TEST:-OFF}
SCRUTINY_BUILD_TESTAPP=${SCRUTINY_BUILD_TESTAPP:-OFF}
CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE:-Release}

cmake -GNinja \
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
        -DSCRUTINY_BUILD_TEST=$SCRUTINY_BUILD_TEST \
        -DSCRUTINY_BUILD_TESTAPP=$SCRUTINY_BUILD_TESTAPP \
        ${@:1} \
        -Wno-dev \
        -S "$APP_ROOT" \
        -B "$BUILD_DIR"

nice cmake --build "$BUILD_DIR" --target all -- -j8
cmake --install "$BUILD_DIR"
