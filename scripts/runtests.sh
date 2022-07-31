#!/bin/bash
set -euo pipefail

APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. >/dev/null 2>&1 && pwd )"
BUILD_CONTEXT="${BUILD_CONTEXT:-dev}"
BUILD_DIR="$APP_ROOT/build-${BUILD_CONTEXT}"

set -x

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

exec test/scrutiny_unittest
