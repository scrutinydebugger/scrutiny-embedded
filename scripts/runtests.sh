#!/bin/bash
set -euo pipefail

APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. >/dev/null 2>&1 && pwd )"

if [[ $(uname -s) == CYGWIN* ]];then
APP_ROOT=$(cygpath -w "$APP_ROOT")
fi

BUILD_CONTEXT="${BUILD_CONTEXT:-dev}"
BUILD_DIR="$APP_ROOT/build-${BUILD_CONTEXT}"

set -x

exec "$BUILD_DIR/test/scrutiny_unittest"
