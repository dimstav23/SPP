# SPP - Safe Persistent Pointers

This repository containts the compiler passes and associated runtime for Safe Persistent Pointers (SPP).

## Installation instructions

### Automated project build

We rely on [nix](https://nixos.org/) package manager to reliably build the environment for SPP.
If you do not run NixOS, you can still use the [nix package manager](https://itsfoss.com/ubuntu-install-nix-package-manager/).

SPP has 3 core components:
1. A fork of LLVM (as a submodule) containing the SPP passes ([llvm-project](./llvm-project/))
2. The SPP runtime library ([runtime](./runtime/))
3. A fork of PMDK (as a submodule) containing the SPP wrappers ([pmdk](./pmdk/))

To easily build all of the above we provide the `install_dependencies.sh` script.
Simply execute (takes some minutes):
```console
$ ./install_dependencies.sh
```

After this point, you can launch a `nix-shell` and the provided environment will contain:
- all the required dependecies
- all the required environment variables (```CC```, ```CXX```, ```CMAKE_C_COMPILER```, ```CMAKE_CXX_COMPILER```, ```BINUTILS_DIR```, ```PMEM_MMAP_HINT```, ```PMEM_IS_PMEM_FORCE```)

Simply run:
```console
$ nix-shell
nix-shell> # in this shell all build dependencies are present
```

#### Notes on `install_dependencies.sh`
The `install_dependencies.sh` script performs the following actions:
1. Initializes the repository by fetching all the submodules
2. Applies a patch to `default.nix` required to have the default `gcc` compiler to build LLVM.
3. Builds the `llvm-project` inside a `nix-shell` based on the patched `default.nix` containing all the dependencies.
4. Reverts the changes of the `default.nix`.
5. Builds the `SPP runtime library` using the freshly compiled `clang` from step 3.
6. Builds the `pmdk` and applies the SPP passes to its examples and benchmarks (`pmembench`) using the freshly compiled `clang` from step 3. Note that by default, it builds SPP with **TAG_BITS set to 26**.


### Manual project build
You can also build the project manually (step by step).
In the next steps, we define `PROJECT_ROOT` as the root directory of this repository.

1. Initialise the submodules of the repository:
```
$ cd PROJECT_ROOT
$ git submodule update --init --recursive
```

2. Build the `llvm-project`:

Apply the `install_depndencies.patch` to temporarily use the normal `gcc` compiler for the LLVM,
enter a `nix-shell`, run the build instructions for LLVM with gold plugin (**Warning!** High memory consumption) and exit the `nix-shell`:
```
$ cd PROJECT_ROOT
$ git apply build_dependencies.patch
$ nix-shell
$ cd PROJECT_ROOT/llvm-project
$ git submodule update --init
$ mkdir -p build
$ mkdir -p install
$ cd build
$ cmake -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_BINUTILS_INCDIR=$BINUTILS_DIR  \
  -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
  -DLLVM_TARGETS_TO_BUILD=X86 \
  -DCMAKE_INSTALL_PREFIX=/llvm ../llvm
$ make -j$(nproc)
$ exit
```

3. Bulld the `SPP runtime library`:

Revert the `default.nix` to its original form, enter a `nix-shell` that wraps the compiler with the `clang` compiled in step 2, fetches all the dependencies and sets the appropriate environment variables, compile the runtime library and exit the `nix-shell`:
```
$ cd PROJECT_ROOT
$ git checkout default.nix
$ nix-shell
$ cd PROJECT_ROOT/runtime
$ make clean && make
$ exit
```

Optional compile parameter for `SPP runtime library`: `TAG_BITS` to determine the tag size for the returned tagged pointer structure.
**Important**: If you use this parameter, this has to be passed to `SPP PMDK` building phase below (Step 4) as well as to any compiling module of your own (so that it does not mess up with the internal struct definitions).

4. Build the `SPP PMDK` fork:

Revert the `default.nix` to its original form, enter a `nix-shell` that wraps the compiler with the `clang` compiled in step 2 and fetches all the dependencies and sets the appropriate environment variables, compile the `SPP PMDK` fork and exit the `nix-shell`:
```
$ cd PROJECT_ROOT
$ git checkout default.nix
$ nix-shell
$ cd PROJECT_ROOT/pmdk
$ git checkout spp-pm_ptr_bit
$ make clean -j$(nproc)
$ make clobber -j$(nproc)
$ make -j$(nproc) TAG_BITS=xx
$ exit
```

Optional compile parameter for `SPP PMDK`: `TAG_BITS` to determine the tag size for the returned tagged pointer structure.
**Important**: If you use this parameter, this has to be passed to `SPP runtime library` building phase above (Step 3) as well as to any compiling module of your own (so that it does not mess up with the internal struct definitions).

5. Enter in the `nix-shell` with all the dependencies and built projects:
```console
$ nix-shell
nix-shell> # in this shell all build dependencies are present
```

The nix-shell already exports the ```CC```, ```CXX```, ```CMAKE_C_COMPILER```, ```CMAKE_CXX_COMPILER```, ```BINUTILS_DIR```, ```PMEM_MMAP_HINT```, ```PMEM_IS_PMEM_FORCE``` environment variables.

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

### How to run the tests
After you have successfully compiled `llvm` and the runtime library, you can run our sample tests from the root directory:
```
OPT_LEVEL=1 ./spp_test.sh
```
or
```
OPT_LEVEL=2 ./spp_test.sh
```
depending on the optimization level that you want your binaries to be compiled with.
Note that this command will create a build directory where it will place the binaries,
build the `pmdk`, generate the transformed IRs and run the tests. 
The IRs and the binaries are placed in `build/tests` folder.