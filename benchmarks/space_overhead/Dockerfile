FROM spp-llvm-clang-test:latest

# Install dependencies for PMDK
RUN apt-get update && apt-get -qq install -y \
    autoconf \
    automake \
    pkg-config \
    libndctl-dev \
    libdaxctl-dev \
    libglib2.0-dev \
    libncurses5-dev \
    libfuse-dev \
    libuv1-dev \
    pandoc \
    libfabric-dev \
    # Dependencies to compile PMDK
    git \
    build-essential \
    # Install dependencies for memkind
    libnuma-dev \
    libtool \
    numactl \
    unzip \
    # Install dependencies for libpmemobj-cpp
    cmake \
    # Install dependencies for pmemkv
    libtbb-dev \
    # Install dependencies for valgrind
    libc6-dbg \
    sudo

# Install PMDK
WORKDIR /
RUN git clone https://github.com/pmem/pmdk.git /pmdk
WORKDIR /pmdk
RUN git checkout 1.9.2
RUN make -j$(nproc)

# Compile SPP runtime library
WORKDIR /spp-pass/runtime
RUN make clean && make

# Install SPP PMDK
WORKDIR /spp-pass/pmdk
COPY ./pmdk_spp.patch /spp-pass/pmdk/
RUN git apply pmdk_spp.patch
RUN make -j$(nproc)

COPY ./inner_run.sh /spp-pass/pmdk/src/benchmarks
WORKDIR /spp-pass/pmdk/src/benchmarks
