#!/bin/bash
set -euo pipefail

APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. >/dev/null 2>&1 && pwd )"
BUILD_DIR="$APP_ROOT/build"

set -x

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

BUILD_CONTEXT="${BUILD_CONTEXT:-dev}"

if [ $BUILD_CONTEXT = "ci" ]; then 
        SCRUTINY_WERR=${SCRUTINY_WERR:-ON}
else
        SCRUTINY_WERR=${SCRUTINY_WERR:-OFF}
fi

SCRUTINY_BUILD_TEST=${SCRUTINY_BUILD_TEST:-OFF}
SCRUTINY_BUILD_TESTAPP=${SCRUTINY_BUILD_TESTAPP:-OFF}

cmake -GNinja \
        -DCMAKE_BUILD_TYPE=Release \
        -DSCRUTINY_WERR=$SCRUTINY_WERR \
        -DSCRUTINY_BUILD_TEST=$SCRUTINY_BUILD_TEST \
        -DSCRUTINY_BUILD_TESTAPP=$SCRUTINY_BUILD_TESTAPP \
        -Wno-dev \
        "$APP_ROOT"

nice cmake --build . --target all -- -j8
