FROM ubuntu:20.04 as base

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    ninja-build \ 
    cmake \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp/

FROM base as native
RUN apt-get update && apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

FROM base as avr
RUN apt-get update && apt-get install -y \
    binutils \
    gcc-avr \
    avr-libc \
    && rm -rf /var/lib/apt/lists/*

FROM base as clang
RUN apt-get update && apt-get install -y \
    clang \
    && rm -rf /var/lib/apt/lists/*    
