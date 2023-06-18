#!/bin/bash
set -euo pipefail

APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
BUILD_DIR="$APP_ROOT/build"
mkdir -p "$BUILD_DIR"

cmake -G Ninja -S "$APP_ROOT" -B "$BUILD_DIR"
cmake --build "$BUILD_DIR"

${BUILD_DIR}/CMakeDemoProject