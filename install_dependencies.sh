#!/bin/bash

set -e

THIS_DIR=$(dirname "$(readlink -f "$0")")

# Fetch the submodules
cd $THIS_DIR
git submodule update --init --recursive

#Apply the patch to default.nix to use the default gcc to compile LLVM
cd $THIS_DIR
git checkout default.nix
git apply build_dependencies.patch

#Enter the nix-shell & compile LLVM
#Note that we escape the BINUTILES_DIR to take the value of the shellHook in default.nix
nix-shell --run "
cd $THIS_DIR/llvm-project && \
git submodule update --init && \
mkdir -p build && \
mkdir -p install && \
cd build && \
cmake -G \"Unix Makefiles\" \
-DCMAKE_BUILD_TYPE=Release \
-DLLVM_BINUTILS_INCDIR=\$BINUTILS_DIR  \
-DLLVM_ENABLE_PROJECTS=\"clang;clang-tools-extra\" \
-DLLVM_TARGETS_TO_BUILD=X86 \
-DCMAKE_INSTALL_PREFIX=/llvm ../llvm && \
make -j$(nproc)
"

# Revert back the default.nix structure to use the impure-clang and use clang in the shell
cd $THIS_DIR
git checkout default.nix

# Compile the SPP runtime with the clang compiler
nix-shell --run "
cd $THIS_DIR/runtime && \
make clean && \
make
"

# # Compile PMDK with the clang compiler
nix-shell --run "
cd $THIS_DIR/pmdk && \
git checkout spp-pm_ptr_bit && \
make clean -j$(nproc) && \
make clobber -j$(nproc) && \
make -j$(nproc) TAG_BITS=26
"
