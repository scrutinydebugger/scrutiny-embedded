#!/bin/bash

#    build.sh
#        A build script for scrutiny-elf-symdump
#
#   - License : MIT - See LICENSE file
#   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-elf-symdump)
#
#    Copyright (c) 2025 Scrutiny Debugger

set -euo pipefail

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
cmake --install . --prefix install
