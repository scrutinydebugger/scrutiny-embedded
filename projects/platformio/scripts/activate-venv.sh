#!/bin/bash
set -euo pipefail

PROJECT_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. >/dev/null 2>&1 && pwd -P )"
PY_MODULE_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." >/dev/null 2>&1 && pwd -P )"

VENV_ROOT="$PROJECT_ROOT/venv-platformio";

echo $VENV_ROOT
log() { echo -e "\x1B[92m[OK]\x1B[39m $@"; }

[ ! -d "$VENV_ROOT" ] \
    && log "Missing venv. Creating..." \
    && python3 -m venv "$VENV_ROOT"

source "$VENV_ROOT/bin/activate"

if ! pip3 show wheel 2>&1 >/dev/null; then
    log "Installing wheel..."
    pip3 install wheel
    log "Upgrading pip..."
    pip3 install --upgrade pip
    log "Upgrading setuptools..."
    pip3 install --upgrade setuptools
fi

if ! pip3 show platformio 2>&1 >/dev/null; then
    log "Installing platformio..."
    pip3 install platformio
fi
