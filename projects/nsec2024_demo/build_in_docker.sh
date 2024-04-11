#!/bin/bash
set -euo pipefail 

SCRIPT_DIR=$(realpath "$(dirname ${BASH_SOURCE[0]})")
cd $SCRIPT_DIR
WORKDIR=$SCRIPT_DIR

docker build . -t nsec2024
docker run \
    --volume $SCRIPT_DIR:$WORKDIR \
    -e ARDUINO_PATH=/usr/share/arduino \
    -u $(id -u):$(id -g) \
    nsec2024 \
    bash -c $WORKDIR/build.sh