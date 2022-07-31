!/bin/bash
set -euo pipefail

APP_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/../.. >/dev/null 2>&1 && pwd )"

TARGET="${TARGET:-native}"

DOCKER_TAG="scrutiny/embedded:$TARGET"

set -x

docker build \
    --progress plain \
    --target $TARGET \
    -t $DOCKER_TAG \
    $APP_ROOT

exec docker run -t --rm \
    -i \
    --user `id -u`:`id -g` -e HOME=/tmp \
    -e CCACHE_DIR=/ccache -v $HOME/.ccache:/ccache \
    -e BUILD_CONTEXT=$TARGET \
    -v $APP_ROOT:$APP_ROOT \
    -w $APP_ROOT \
    $DOCKER_TAG \
    "$@"