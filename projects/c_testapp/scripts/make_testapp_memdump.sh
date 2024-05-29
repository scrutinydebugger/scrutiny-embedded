#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

MEMRANGE="$(python3 $SCRIPT_DIR/get_elf_memranges.py ./build-dev/projects/c_testapp/c_testapp)"
./build-dev/projects/c_testapp/c_testapp memdump $MEMRANGE > c_testapp_memdump