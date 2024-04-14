FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /tmp/

RUN apt-get update && apt-get install -y \
    ninja-build \ 
    cmake \
    git \
    wget \
    arduino \
    gcc-avr \
    avrdude \
    avr-libc \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

#RUN pip3 install scrutinydebugger
RUN  git clone -b nsec2024_demo https://github.com/scrutinydebugger/scrutiny-python /tmp/scrutiny-python && pip3 install -e /tmp/scrutiny-python