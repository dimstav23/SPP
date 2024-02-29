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

# Compile SPP runtime library
WORKDIR /SPP/runtime
RUN make clean && make

# Install PMDK
WORKDIR /SPP/pmdk
COPY ./pmdk_spp.patch /SPP/pmdk/
RUN git apply pmdk_spp.patch
RUN make -j7 install
RUN ldconfig

WORKDIR /
RUN git clone https://github.com/pmem/valgrind.git
WORKDIR /valgrind
RUN git checkout 06f15d69237501852dd29883940e18da4179830a
RUN ./autogen.sh
RUN ./configure
RUN make -j7
RUN make -j7 install

COPY ./change_pmembench_file_path.sh /SPP/pmdk/src/benchmarks
COPY ./inner_run.sh /SPP/pmdk/src/benchmarks
WORKDIR /SPP/pmdk/src/benchmarks
RUN /SPP/pmdk/src/benchmarks/change_pmembench_file_path.sh
