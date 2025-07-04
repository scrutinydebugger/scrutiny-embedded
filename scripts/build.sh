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

SCRUTINY_ENABLE_DATALOGGING=${SCRUTINY_ENABLE_DATALOGGING:-ON}
SCRUTINY_SUPPORT_64BITS=${SCRUTINY_SUPPORT_64BITS:-ON}
SCRUTINY_DATALOGGING_BUFFER_32BITS=${SCRUTINY_DATALOGGING_BUFFER_32BITS:-OFF}
SCRUTINY_BUILD_CWRAPPER=${SCRUTINY_BUILD_CWRAPPER:-ON}
SCRUTINY_BUILD_TEST=${SCRUTINY_BUILD_TEST:-OFF}
SCRUTINY_BUILD_TESTAPP=${SCRUTINY_BUILD_TESTAPP:-OFF}
CMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD:-11}
CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE:-Release}

cmake -GNinja \
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
        -DSCRUTINY_BUILD_TEST=$SCRUTINY_BUILD_TEST \
        -DSCRUTINY_BUILD_TESTAPP=$SCRUTINY_BUILD_TESTAPP \
        -DSCRUTINY_BUILD_CWRAPPER=$SCRUTINY_BUILD_CWRAPPER \
        -DSCRUTINY_ENABLE_DATALOGGING=$SCRUTINY_ENABLE_DATALOGGING \
        -DSCRUTINY_SUPPORT_64BITS=$SCRUTINY_SUPPORT_64BITS \
        -DSCRUTINY_DATALOGGING_BUFFER_32BITS=$SCRUTINY_DATALOGGING_BUFFER_32BITS \
        -DCMAKE_CXX_STANDARD=$CMAKE_CXX_STANDARD \
        -DINSTALL_FOLDER=$BUILD_DIR/install \
        ${@:1} \
        -Wno-dev \
        -S "$APP_ROOT" \
        -B "$BUILD_DIR"

nice cmake --build "$BUILD_DIR" --target all
cmake --install "$BUILD_DIR"
