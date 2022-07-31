#!/bin/bash
set -euo pipefail

APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. >/dev/null 2>&1 && pwd )"

BUILD_CONTEXT="${BUILD_CONTEXT:-dev}"
BUILD_DIR="$APP_ROOT/build-${BUILD_CONTEXT}"

set -x

mkdir -p "$BUILD_DIR"

SCRUTINY_WERR=${SCRUTINY_WERR:-OFF}
SCRUTINY_BUILD_TEST=${SCRUTINY_BUILD_TEST:-OFF}
SCRUTINY_BUILD_TESTAPP=${SCRUTINY_BUILD_TESTAPP:-OFF}

env

cmake -GNinja \
        -DCMAKE_BUILD_TYPE=Release \
        -DSCRUTINY_WERR=$SCRUTINY_WERR \
        -DSCRUTINY_BUILD_TEST=$SCRUTINY_BUILD_TEST \
        -DSCRUTINY_BUILD_TESTAPP=$SCRUTINY_BUILD_TESTAPP \
        -Wno-dev \
        -S "$APP_ROOT" \
        -B "$BUILD_DIR"

nice cmake --build "$BUILD_DIR" --target all -- -j8
