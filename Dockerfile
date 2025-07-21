FROM ubuntu:22.04 as base

ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /tmp/

RUN apt-get update && apt-get install -y \
    ninja-build \ 
    cmake \
    git \
    wget \
    && rm -rf /var/lib/apt/lists/*

FROM base as static-analysis
ARG CPPCHECK_VERSION="2.10.3"
ARG CPPCHECK_URL="https://github.com/danmar/cppcheck/archive/refs/tags/${CPPCHECK_VERSION}.tar.gz"
ARG CPPCHECK_FOLDER="cppcheck-${CPPCHECK_VERSION}"
RUN apt-get update \
    && apt-get install -y \ 
    build-essential \
    python3 \
    python3-pip \
    libpcre3-dev \
    && wget $CPPCHECK_URL -O /tmp/cppcheck.tar.gz \
    && tar -xvzf /tmp/cppcheck.tar.gz -C /tmp/ \
    && cd "/tmp/${CPPCHECK_FOLDER}" \
    && make MATCHCOMPILER=no FILESDIR=/usr/share/cppcheck HAVE_RULES=no CXXFLAGS="-O2 -DNDEBUG -Wall -Wno-sign-compare -Wno-unused-function" -j2 \
    && FILESDIR=/usr/share/cppcheck make install  \
    && cd /tmp/ \
    && rm -rf "/tmp/${CPPCHECK_FOLDER}" \
    && rm -rf /var/lib/apt/lists/*

RUN python3 -m pip install scrutinydebugger

FROM base as native-gcc
RUN apt-get update && apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

FROM base as avr-gcc
RUN apt-get update && apt-get install -y \
    binutils \
    gcc-avr \
    avr-libc \
    && rm -rf /var/lib/apt/lists/*

FROM base as native-clang
RUN apt-get update && apt-get install -y \
    clang \
    && rm -rf /var/lib/apt/lists/*    

FROM base as powerpc64-linux-gcc
RUN apt-get update && apt-get install -y \
    gcc-powerpc64-linux-gnu \
    g++-powerpc64-linux-gnu \
    && rm -rf /var/lib/apt/lists/*    

FROM base as aarch64-linux-gcc
RUN apt-get update && apt-get install -y \
    gcc-aarch64-linux-gnu \
    g++-aarch64-linux-gnu \
    && rm -rf /var/lib/apt/lists/*    

FROM base as arm-none-gcc
RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    && rm -rf /var/lib/apt/lists/*

#Jenkins runs everything up to the specified target.
# So that would be the stopping point of the Wait Docker stage
FROM base AS CI

FROM native-gcc as dev-sample
WORKDIR /app
COPY . .
RUN CMAKE_TOOLCHAIN_FILE=\/app/cmake/gcc.cmake \
    SCRUTINY_BUILD_TEST=1 \
    SCRUTINY_BUILD_TESTAPP=1 \
    bash scripts/build.sh

CMD /app/build-dev/projects/testapp/testapp udp-listen 12345

