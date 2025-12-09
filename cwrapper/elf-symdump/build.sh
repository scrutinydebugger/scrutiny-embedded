#!/bin/bash

set -euo pipefail

APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$APP_ROOT"

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
cmake --install . --prefix install
