#!/bin/sh

mkdir -p /llvm-project/build /llvm
cd /llvm-project/build

cmake -G "Unix Makefiles" \
-DCMAKE_BUILD_TYPE=Release \
-DLLVM_BINUTILS_INCDIR=/usr/include \
-DLLVM_ENABLE_PROJECTS="clang" \
-DLLVM_TARGETS_TO_BUILD=X86 \
-DCMAKE_INSTALL_PREFIX=/llvm ../llvm

make -j$(nproc)

make install -j$(nproc)

cd /
rm -rf llvm-project
