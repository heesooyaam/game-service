FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    clang \
    cmake \
    ninja-build \
    gdb \
    git \
    bash-completion \
    && rm -rf /var/lib/apt/lists/*
