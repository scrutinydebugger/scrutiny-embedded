FROM ubuntu:20.04
RUN set -eux;

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=America/Toronto
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update
RUN apt-get install -y \
    build-essential \
    ninja-build \ 
    cmake \
    git

WORKDIR /tmp/