# spp-pass

Compiler pass and associated runtime for Safe Persistent Pointers

### Installation instructions

Initialise the submodules of the repository:

```
git submodule update --init
```

(optionally) To get all build dependencies to build this project, one can use [nix](https://nixos.org).
A tutorial for nix can be found at https://nix.dev/

```console
$ nix-shell
nix-shell> # in this shell all build dependencies are present
```

Compile the `pmdk` fork:

```
cd pmdk
git checkout spp-main
make -j$(nproc)
```

(1) Compile `LLVM` (Warning! High memory consumption):  
```
CC=/path/to/gcc 
CXX=/path/to/g++

cd llvm-project;
mkdir build;
mkdir install;
cd build;
cmake -G "Unix Makefiles" \
-DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
DCMAKE_C_COMPILER=CC \
DCMAKE_CXX_COMPILER=CXX \
BUILD_SHARED_LIBS=ON \
-DCMAKE_INSTALL_PREFIX=../install ../llvm
make -j$(nproc);

// Optional for fast build: LLVM_OPTIMIZED_TABLEGEN=ON

export PATH=$PATH:$PWD/bin
```

(2) Build LLVM gold plugin (llvm only without other packages):
https://llvm.org/docs/GoldPlugin.html

```
cd llvm-project;
mkdir build;
mkdir install;
cd build;
cmake -G "Unix Makefiles" \
-DLLVM_BINUTILS_INCDIR=/usr/include \
DCMAKE_C_COMPILER=CC \
DCMAKE_CXX_COMPILER=CXX \
BUILD_SHARED_LIBS=ON \
-DCMAKE_INSTALL_PREFIX=../install ../llvm
make -j$(nproc)
export PATH=$PATH:$PWD/bin
```

### Usage instructions

Disable mapping address randomization and provide PMDK with appropriately low mapping address hint ([src](https://pmem.io/pmdk/manpages/linux/v1.0/libpmem.3.html)):
```
export PMEM_MMAP_HINT=0
```

If your machine is not equipped with PM, export the following flag to use `flush` instructions instead of `msync`:
```
export PMEM_IS_PMEM_FORCE=1
```
