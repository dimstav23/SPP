#!/bin/sh

mkdir -p /spp-pass/llvm-project/build /llvm
cd /spp-pass/llvm-project/build

cmake -G "Unix Makefiles" \
-DCMAKE_BUILD_TYPE=Release \
-DLLVM_BINUTILS_INCDIR=/usr/include \
-DLLVM_ENABLE_PROJECTS="clang" \
-DLLVM_TARGETS_TO_BUILD=X86 \
-DCMAKE_INSTALL_PREFIX=/llvm ../llvm

make -j$(nproc) clang

make install -j$(nproc) clang

cd /spp-pass
rm -rf llvm-project
