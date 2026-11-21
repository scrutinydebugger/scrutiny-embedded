#!/bin/bash
# Clones scrutiny-testapps and builds the C testapp against this checkout of the library.
set -euo pipefail

APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. >/dev/null 2>&1 && pwd )"

TESTAPPS_REPO=${TESTAPPS_REPO:-https://github.com/scrutinydebugger/scrutiny-testapps}
TESTAPPS_BRANCH=${TESTAPPS_BRANCH:-master}
BUILD_CONTEXT="${BUILD_CONTEXT:-dev}"
CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE:-Release}

TESTAPP=c_testapp
BUILD_DIR="$APP_ROOT/build-${BUILD_CONTEXT}/$TESTAPP"
TESTAPPS_DIR=$(mktemp -d)
trap 'rm -rf "$TESTAPPS_DIR"' EXIT

set -x
git clone --depth 1 --branch "$TESTAPPS_BRANCH" "$TESTAPPS_REPO" "$TESTAPPS_DIR"

# The testapp fetches the library from GitHub. Redirect it to this checkout.
cmake -GNinja \
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
    -DFETCHCONTENT_SOURCE_DIR_SCRUTINY_EMBEDDED="$APP_ROOT" \
    ${@:1} \
    -Wno-dev \
    -S "$TESTAPPS_DIR/$TESTAPP" \
    -B "$BUILD_DIR"

# A misspelled override would silently build against the GitHub version. lib/ declares the last project(scrutiny-embedded).
if ! grep -qxF "scrutiny-embedded_SOURCE_DIR:STATIC=$APP_ROOT/lib" "$BUILD_DIR/CMakeCache.txt"; then
    echo "$TESTAPP was not configured against the local library at $APP_ROOT"
    exit 1
fi

nice cmake --build "$BUILD_DIR" --target all
