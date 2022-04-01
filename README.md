# spp-pass

Compiler pass and associated runtime for Safe Persistent Pointers

## Installation instructions

Initialise the submodules of the repository:

```
git submodule update --init
```

To get all build dependencies to build this project, one can use [nix](https://nixos.org).
A tutorial for nix can be found at https://nix.dev/

```console
$ nix-shell
nix-shell> # in this shell all build dependencies are present
```

The nix-shell already exports the ```CC```, ```CXX```, ```CMAKE_C_COMPILER```, ```CMAKE_CXX_COMPILER```, ```BINUTILS_DIR``` environment variables.

### Inside the nix-shell:

(1) Compile the `pmdk` fork:

```
cd pmdk
git checkout spp-main
make -j$(nproc) TAG_BITS=xx
```
Optional compile parameter for `pmdk`: `TAG_BITS` to determine the tag size for the returned tagged pointer structure. 


(2) Compile `LLVM` with gold plugin (Warning! High memory consumption):
https://llvm.org/docs/GoldPlugin.html
```
cd llvm-project;
mkdir build;
mkdir install;
cd build;
cmake -G "Unix Makefiles" \
-DLLVM_BINUTILS_INCDIR=$BINUTILS_DIR \
-DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
DCMAKE_C_COMPILER=CC \
DCMAKE_CXX_COMPILER=CXX \
BUILD_SHARED_LIBS=ON \
-DCMAKE_INSTALL_PREFIX=../install ../llvm
make -j$(nproc);

// Optional for fast build: LLVM_OPTIMIZED_TABLEGEN=ON

export PATH=$PATH:$PWD/bin
```

## Usage instructions

Disable mapping address randomization and provide PMDK with appropriately low mapping address hint ([src](https://pmem.io/pmdk/manpages/linux/v1.0/libpmem.3.html)):
```
export PMEM_MMAP_HINT=0
```

If your machine is not equipped with PM, export the following flag to use `flush` instructions instead of `msync`:
```
export PMEM_IS_PMEM_FORCE=1
```

### Minimal example execution

In the `examples` folder you can find a minimal example which can be built and run with the launch.sh script.
```
cd $PROJECT_ROOT/examples/
./launch.sh
```
The script also produces the original and transformed LLVM-IR for the simple `example.c` code.

#### Note
Currently the passes produce a lot of debug messages. In order to control the debug information, someone can comment out the defined `DEBUG` variables in `runtime/src/spp.c`, `llvm-project/llvm/lib/Transforms/SPP/spp.cpp` and `llvm-project/llvm/lib/Transforms/IPO/SPPLTO.cpp`.

## Code structure

### Directories
`benchmarks`: folder containing the benchmark scripts

`llvm-project`: llvm fork that contains and registers the passes needed for SPP

`runtime`: runtime library for the hook functions

`pmdk`: pmdk fork that contains the modified libpmemobj that uses the enhanced `PMEMoid` structure and constructs the appropriate `tagged pointers`

### Important files

`runtime/src/spp.c`: hook functions implementation

`llvm-project/llvm/lib/Transforms/SPP/spp.cpp`: SPP module pass implementation

`llvm-project/llvm/lib/Transforms/IPO/SPPLTO.cpp`: LTO pass implementation

### How to run pmembench
First make sure that you have correctly compiled `llvm/clang` as defined above and you use the `nix-shell` to use the appropriate wrappers.

Compile PMDK and the instrumented pmembench benchmarks (uses the default number of tag bits):
```
cd $PROJECT_ROOT/pmdk/
make clobber
make clean
make -j$(nproc) 
```
Run the `pmembench` with the `pmembench_map.cfg` (persistent indices) and `pmembench_tx_spp.cfg` (PM management operations):
```
cd $PROJECT_ROOT/pmdk/src/benchmarks
LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg
LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx_spp.cfg
```
Note that this execution redirects the output to `stdout`. To store it in a file, simply redirect it to the file of your choice.

For the PMDK baseline, compile the native PMDK with:
```
cd $PROJECT_ROOT/pmdk/
make clobber
make clean
make -j$(nproc) SPP_OFF=1
```
Then run again the `pmembench` commands as explained above.