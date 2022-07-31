FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    avr-libc \
    build-essential \
    ninja-build \ 
    cmake \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp/