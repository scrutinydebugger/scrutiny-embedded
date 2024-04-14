#/bin/bash
set -euo pipefail

SCRIPT_DIR=$(realpath "$(dirname ${BASH_SOURCE[0]})")
cd "$SCRIPT_DIR" 

source common.sh

[ -z ${ARDUINO_PATH:+x} ] && fatal "ARDUINO_PATH must be set"

info "ARDUINO_PATH=$ARDUINO_PATH"
mkdir -p build
cmake -G Ninja \
    -DARDUINO_PATH=$ARDUINO_PATH \
    -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/cmake/toolchain/mega2526.toolchain.cmake" \
    -S . -B build

ninja -C build