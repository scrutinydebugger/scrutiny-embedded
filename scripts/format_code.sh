#!/bin/bash
set -euo pipefail

APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. >/dev/null 2>&1 && pwd )"
cd "$APP_ROOT"

format_dir()
{
    find $1 -iname '*.hpp' -o -iname '*.cpp' -o -iname '*.c' -o -iname '*.h'  | xargs clang-format -i
}

format_dir ./lib
format_dir ./test
format_dir ./projects
format_dir ./cwrapper