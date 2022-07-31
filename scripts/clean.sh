#!/bin/bash
set -euo pipefail
APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. >/dev/null 2>&1 && pwd )"
BUILD_DIR=${SCRUTINY_BUILD_DIR:-"$APP_ROOT/build"}

rm -rf "${BUILD_DIR}"