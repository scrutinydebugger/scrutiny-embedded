#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

MEMRANGE="$(python3 $SCRIPT_DIR/get_elf_memranges.py ./build-dev/projects/testapp/testapp)"
./build-dev/projects/testapp/testapp memdump $MEMRANGE > testapp_memdump