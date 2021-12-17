# spp-pass

Compiler pass and associated runtime for Safe Persistent Pointers

### Installation instructions

Initialise the submodules of the repository:

```
git submodule update --init
```

Compile the `pmdk` fork:

```
cd pmdk
git checkout spp-main
make -j$(nproc)
```

Compile `LLVM` (Warning! High memory consumption):

```
cd llvm-project
mkdir build
mkdir install
cd build
cmake -G "Unix Makefiles" -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Debug ../llvm
make -j$(nproc)
export PATH=$PATH:$PWD/bin
```

Compile the llvm pass:

```
mkdir build && cd build
cmake -G "Unix Makefiles" ..
make -j$(nproc)
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
