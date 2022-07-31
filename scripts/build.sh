#!/bin/bash
set -euo pipefail

APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. >/dev/null 2>&1 && pwd )"

BUILD_CONTEXT="${BUILD_CONTEXT:-dev}"
BUILD_DIR="$APP_ROOT/build-${BUILD_CONTEXT}"

set -x

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

SCRUTINY_WERR=${SCRUTINY_WERR:-OFF}
SCRUTINY_BUILD_TEST=${SCRUTINY_BUILD_TEST:-OFF}
SCRUTINY_BUILD_TESTAPP=${SCRUTINY_BUILD_TESTAPP:-OFF}

cmake -GNinja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE \
        -DSCRUTINY_WERR=$SCRUTINY_WERR \
        -DSCRUTINY_BUILD_TEST=$SCRUTINY_BUILD_TEST \
        -DSCRUTINY_BUILD_TESTAPP=$SCRUTINY_BUILD_TESTAPP \
        -Wno-dev \
        "$APP_ROOT"

nice cmake --build . --target all -- -j8
