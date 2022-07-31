#!/bin/bash
set -euo pipefail
APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. >/dev/null 2>&1 && pwd )"

SCRUTINY_CI_PROFILE="${SCRUTINY_CI_PROFILE:-none}"
if [ SCRUTINY_CI_PROFILE = "none" ]; then 
    echo "CI profile not set"
    exit 1
fi

set -x

NATIVE_BUILD_FOLDER="$APP_ROOT"/build-native

if [ ${SCRUTINY_CI_PROFILE} = "build-native" ]; then
    unset CMAKE_TOOLCHAIN_FILE
    export SCRUTINY_BUILD_FOLDER="$NATIVE_BUILD_FOLDER"
    export SCRUTINY_BUILD_TEST=1
    export SCRUTINY_BUILD_TESTAPP=1
    $APP_ROOT/scripts/build.sh
    $APP_ROOT/scripts/runtests.sh
    $APP_ROOT/scripts/clean.sh

elif if [ ${SCRUTINY_CI_PROFILE} = "test-native" ]; then
    unset CMAKE_TOOLCHAIN_FILE
    export SCRUTINY_BUILD_TEST=1
    export SCRUTINY_BUILD_TESTAPP=1
    $APP_ROOT/scripts/build.sh
    $APP_ROOT/scripts/runtests.sh
    $APP_ROOT/scripts/clean.sh

elif [ ${SCRUTINY_CI_PROFILE} = "build-avr-gcc" ]; then
    export SCRUTINY_BUILD_TEST=1
    export SCRUTINY_BUILD_TESTAPP=1
    export CMAKE_TOOLCHAIN_FILE=$APP_ROOT/ci/cmake/avr-gcc.cmake
    $APP_ROOT/scripts/build.sh
    $APP_ROOT/scripts/clean.sh

else
    echo "Unknown CI profile : ${SCRUTINY_CI_PROFILE}"
    exit 1
fi
