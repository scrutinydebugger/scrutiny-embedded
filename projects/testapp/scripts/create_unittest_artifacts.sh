#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_ROOT="${SCRIPT_DIR}/../../.."

cd ${PROJECT_ROOT}

OUTPUT_FOLDER=unittest_outputs
rm -rf "$OUTPUT_FOLDER"
mkdir "$OUTPUT_FOLDER"
export BUILD_CONTEXT=dev

for DWARF_VERSION in 2 3 4; do
    rm -rf build-${BUILD_CONTEXT}
    SCRUTINY_BUILD_TESTAPP=1 SCRUTINY_TESTAPP_DWARF_VERSION=${DWARF_VERSION} ./scripts/build.sh -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc
    filename=testapp-dwarf-gcc-dwarf${DWARF_VERSION}
    filepath="$OUTPUT_FOLDER/$filename"
    cp build-${BUILD_CONTEXT}/projects/testapp/testapp "$filepath"
    ${SCRIPT_DIR}/make_testapp_memdump.sh "$filepath"

    rm -rf build-${BUILD_CONTEXT}
    SCRUTINY_BUILD_TESTAPP=1 SCRUTINY_TESTAPP_DWARF_VERSION=${DWARF_VERSION} ./scripts/build.sh -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang
    filename=testapp-dwarf-clang-dwarf${DWARF_VERSION}
    filepath="$OUTPUT_FOLDER/$filename"
    cp build-${BUILD_CONTEXT}/projects/testapp/testapp "$filepath"
    ${SCRIPT_DIR}/make_testapp_memdump.sh "$filepath"
done