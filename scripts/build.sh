#!/bin/bash
set -euo pipefail

APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. >/dev/null 2>&1 && pwd )"
BUILD_DIR="$APP_ROOT/build"

set -x

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

BuildContext="${BUILD_CONTEXT:-dev}"
[[ $BuildContext = "ci" ]] && WERR="ON" || WERR="OFF"

cmake -GNinja \
        -DCMAKE_BUILD_TYPE=Release \
        -DSCRUTINY_BUILD_TESTAPP=ON \
        -DSCRUTINY_BUILD_TEST=ON \
        -DSCRUTINY_WERR=$WERR \
        -Wno-dev \
        "$APP_ROOT"

nice cmake --build . --target all -- -j8
