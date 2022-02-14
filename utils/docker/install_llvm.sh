#!/bin/sh

mkdir -p /spp-pass/llvm-project/build /llvm
cd /spp-pass/llvm-project/build

cmake -G "Unix Makefiles" \
-DLLVM_BINUTILS_INCDIR=/usr/include \
-DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
DCMAKE_C_COMPILER=CC \
DCMAKE_CXX_COMPILER=CXX \
BUILD_SHARED_LIBS=ON \
-DCMAKE_INSTALL_PREFIX=/llvm ../llvm

make -j$(nproc)

make install -j$(nproc)

cd /spp-pass
rm -rf llvm-project
