#!/bin/bash
set -euo pipefail

APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. >/dev/null 2>&1 && pwd )"
BUILD_DIR="$APP_ROOT/build"

set -x

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake -GNinja \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_TESTAPP=ON \
        -DBUILD_TEST=ON \
        "$APP_ROOT"

nice cmake --build . --target all -- -j8
