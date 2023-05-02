FROM ubuntu:22.04 as base

ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /tmp/

RUN apt-get update && apt-get install -y \
    ninja-build \ 
    cmake \
    git \
    cppcheck \
    && rm -rf /var/lib/apt/lists/*

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
