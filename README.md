# SPP - Safe Persistent Pointers

This repository containts the compiler passes and associated runtime for Safe Persistent Pointers (SPP) for Persistent Memory (PM).

## Installation instructions

Initially clone the repository:
```
git clone git@github.com:dimstav23/SPP.git
```

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

After this point, you can launch a ```nix-shell``` and the provided environment will contain:
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
3. Builds the `llvm-project` inside a ```nix-shell``` based on the patched `default.nix` containing all the dependencies.
4. Reverts the changes of the `default.nix`.
5. Builds the `SPP runtime library` using the freshly compiled `clang` from step 3.
6. Builds the `pmdk` and applies the SPP passes to its examples and benchmarks (`pmembench`) using the freshly compiled `clang` from step 3. Note that by default, it builds SPP with **TAG_BITS set to 26**.


### Manual project build
You can also build the project manually (step by step).
In the next steps, we define `PROJECT_ROOT` as the root directory of this repository.

1. **Initialise the submodules of the repository:**
```
$ cd PROJECT_ROOT
$ git submodule update --init --recursive
```

2. **Build the `llvm-project`:**

Apply the `install_depndencies.patch` to temporarily use the normal `gcc` compiler for the LLVM,
enter a ```nix-shell```, run the build instructions for LLVM with gold plugin (**Warning!** High memory consumption) and exit the ```nix-shell```:
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

3. **Bulld the `SPP runtime library`:**

Revert the `default.nix` to its original form, enter a ```nix-shell``` that wraps the compiler with the `clang` compiled in step 2, fetches all the dependencies and sets the appropriate environment variables, compile the runtime library and exit the ```nix-shell```:
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

4. **Build the `SPP PMDK` fork:**

Revert the `default.nix` to its original form, enter a ```nix-shell``` that wraps the compiler with the `clang` compiled in step 2 and fetches all the dependencies and sets the appropriate environment variables, compile the `SPP PMDK` fork and exit the ```nix-shell```:
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

Optional compile parameters for `SPP PMDK`: 
- `SPP_OFF=1` to disable the SPP modifications and have the plain `PMDK`.
- `TAG_BITS=xx` to determine the tag size for the returned tagged pointer structure.

**Important**: If you use the `TAG_BITS` parameter, this has to be passed to `SPP runtime library` building phase above (Step 3) as well as to any compiling module of your own (so that it does not mess up with the internal struct definitions).

5. **Enter in the ```nix-shell``` with all the dependencies and built projects:**
```console
$ nix-shell
nix-shell> # in this shell all build dependencies are present
```

The nix-shell already exports the ```CC```, ```CXX```, ```CMAKE_C_COMPILER```, ```CMAKE_CXX_COMPILER```, ```BINUTILS_DIR```, ```PMEM_MMAP_HINT```, ```PMEM_IS_PMEM_FORCE``` environment variables.

---

## Usage instructions

For the instructions below, we assume that you already have completed the [installation steps](#installation-instructions) above and you are in the environment provided by the ```nix-shell```.

### Minimal examples execution

We provide 2 sets of minimal examples.

#### Single source file example 
In the [`examples/simple_example`](./examples/simple_example/) folder you can find a minimal example which can be built and run with the [`launch.sh`](./examples/simple_example/launch.sh) script as follows:
```
$ cd PROJECT_ROOT/examples/simple_example
$ ./launch.sh
```
The example opens a persistent memory pool, allocates the root object and performs various accesses and copies. 
It uses the `/dev/shm` for the pool (with the ```PMEM_IS_PMEM_FORCE``` exported flag, it is handled as PM from the application).
We opted for that option so that this simple example can run in systems without PM.
It also includes some dummy volatile memory actions in the beginning for comparison purposes of the programming model.
When you run the example, it should result in a SEGMENTATION FAULT because of the injected overflow. 
The [`example.c`](./examples/simple_example/example.c) file has some annotated lines that should crash.
Feel free to experiment with those. One of them contains a function that also triggers a PM buffer overflow in the `memcpy` function.
The [`launch.sh`](./examples/simple_example/launch.sh) script also produces the original and transformed LLVM-IR for the simple [`example.c`](./examples/simple_example/example.c) code.

#### Library example
In the [`examples/library_example`](./examples/library_example/) folder you can find a minimal example which can be built and run with the [`launch.sh`](./examples/library_example/launch.sh) script as follows:
```
$ cd PROJECT_ROOT/examples/library_example
$ ./launch.sh
```
This examples showcases how SPP treats libraries. Especially, it highlights also one limitation of SPP when an overflow occurs in a shared library and cannot be caught.
We provide the [`libfuncs.h`](./examples/library_example/libfuncs.h) and [`libfuncs.c`](./examples/library_example/libfuncs.c) source code that is built into both a shared (.so) and a static (.a) library.
This library contains code that performs memory access and memory copies.
We provide the [`driver.c`](./examples/library_example/driver.c) code that performs similar steps with the `simple_example` above but instead of performing the memory ations or calling the functions in the single source, it uses the compiled libraries.
Here, we also use the `/dev/shm` for the pool (with the ```PMEM_IS_PMEM_FORCE``` exported flag, it is handled as PM from the application).
Similarly with the `simple_example`, we opted for that option so that this simple example can run in systems without PM.
When you run the example, it should result in a SEGMENTATION FAULT because of the injected overflow for the case of the `driver_static` produced executable that uses the static library. The `driver_shared` runs normally without an error as the overflow cannot be detected because the tagged pointer is cleared when passed to the external library (clear limitation -- specified also in the paper).
Note the **line 60**: The `driver.c` calls the `test_memcpy` function. In the case of the static library the pointer is not cleared and therefore, the overflow is detected, causing the SEGMENTATION FAULT.
The [`driver.c`](./examples/library_example/driver.c) file has some annotated lines that should crash.
Feel free to experiment with those. 
The [`launch.sh`](./examples/library_example/launch.sh) script also produces the original and transformed LLVM-IR for the [`driver.c`](./examples/library_example/driver.c) code.

### Functionality tests

The functionality testing suite was originally copied from [`SafePM`](https://github.com/TUM-DSE/safepm) and adapted to SPP needs.

#### General information
We provide an extensive set of tests covering multiple types of buffer overflows on PM objects.
The source code of these tests is located in the [`tests`](./tests/) folder.
The name of each file implies the type of overflow that it performs.
More information about each test can be found in the [`README`](./tests/README.md) of the respective folder.

#### Configuration information
These tests create the pool in actual PM in the directory `/mnt/pmem0/dimitrios`.
To make them work on every system, you can easily replace this path with either the path where your PM is mounted or with `/dev/shm`.
To do this for all the tests, simply run:
```
$ cd PROJECT_ROOT
$ sed -i 's|/mnt/pmem0/dimitrios|/dev/shm|' ./tests/*
```

#### Execution
We provide the [`spp_test.sh`](./spp_test.sh) script that is responsible to orchestrate the execution of the tests.
More precisely, the [`spp_test.sh`](./spp_test.sh) script:
- Adapts the [`CMakeLists.txt`](./tests/CMakeLists.txt) of the tests subfolder to reflect the chosen optimization level.
- Creates the `build` directory in the root directory of the repository.
- Rebuilds the [`PMDK`](./pmdk/) and places its installation file in a specified directory (**Note**: it uses the default TAG_BITS option which set the TAG_BITS to **26**).
- Sets the appropriate compilation flags, which are also printed upon execution.
- Builds the functionality tests in the `build` directory, including their IRs (placed in `build/tests`). The building process of the tests is based on the [`CMakeLists.txt`](./tests/CMakeLists.txt) of the subfolder. Please, consult the respective [`README`](./tests/README.md) for more information.
- Runs the tests and prints them in green/red depending if they were successful or not. To achieve this, the `launch.sh` script defines 2 functions, `should_crash` and `should_not_crash`, that check if the execution has the expected outcome based on its output (i.e., we should have no invocation of the `print_fail_flag`, defined in the [`common.h`](./tests/common.h) of the tests). 

To execute the tests, depending on the optimization level that you want your binaries to be compiled with, run:
```
$ cd PROJECT_ROOT
$ OPT_LEVEL=1 ./spp_test.sh
$ OPT_LEVEL=2 ./spp_test.sh
```

### How to easily run pmembench
In the ```nix-shell```, you can run the `pmembench` with the `pmembench_map.cfg` (persistent indices) and `pmembench_tx_spp.cfg` (PM management operations):
```
$ cd PROJECT_ROOT/pmdk/src/benchmarks
$ LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg
$ LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx_spp.cfg
```
Note that this execution directs the output to `stdout`. To store it in a file, simply redirect it to the file of your choice.

To test `pmembench` with the vanilla PMDK (baseline), you can compile the native PMDK with:
```
$ cd PROJECT_ROOT/pmdk/
$ make clobber
$ make clean
$ make -j$(nproc) SPP_OFF=1
```
Then run again the `pmembench` commands as explained above.

**Note**: by default, the `pmembench` uses the `/dev/shm` for the pool (with the ```PMEM_IS_PMEM_FORCE``` exported flag, it is handled as PM from the application).
We opted for that option so that the `pmembench` can run in systems without PM.
For the SPP [pmembench benchmarks](./benchmarks/pmembench), we [replace](./benchmarks/pmembench/run_spp.sh#L6) the path with actual PM to get the reliable measurements.
Feel free to adjust the paths in `pmembench_map.cfg` and `pmembench_tx_spp.cfg` located in `PROJECT_ROOT/pmdk/src/benchmarks` folder of the `pmdk` submodule.

---

## Benchmarks & Artifact Evaluation

We choose to use docker containers to have a reproducible environment for our benchmarks.

### Docker-based environment
We provide a set of Dockerfiles and setup scripts to build the appropriate [compiler images](./utils/docker/compiler_images/) and [runtime environments](./utils/docker/packaged_environments/) for all the variants with all the dependencies and project installations.
More information can be found in the respective [`README`](./utils/docker/README.md).

### Benchmarks
To run the SPP [benchmarks](./benchmarks/):
1. You have to make sure that you have set up the appropriate images for all the variants (`spp`, `vanilla pmdk` and `safepm`). 
Instructions on how to achieve this are provided [here](./utils/docker/README.md).
2. Visit the [benchmarks](./benchmarks/) subfolder and choose the benchmark that you would like to run. Inside each subfolder, there is a dedicated `README` with the appropriate instructions and information about the result files/data.

### Artifact evaluation
We provide a detailed recipe on how to run the required benchmarks for the paper in the [artifact_evaluation](./artifact_evaluation/) folder. This recipe includes instructions on how to produce the required docker images, how to run the benchmarks in the containerized environment and how to visualize the results shown in our paper. 

For more information, please consult the respective [`README`](./artifact_evaluation/README.md).

### Results
We also include a `.zip` archive where we have enclosed the sample results we used for the paper plots. It can be found [here](./benchmarks/sample_results.zip). Note that it does not contain the `phoenix` benchmark results, as it was added later in the paper.

---

## Code structure

### Directories

- [`benchmarks`](./benchmarks/): folder containing the benchmark scripts. For further detailed information, check [here](./benchmarks/README.md).

- [`llvm-project`](./llvm-project/): llvm fork that contains and registers the passes needed for SPP.

- [`runtime`](./runtime/): runtime library for the hook functions. For further detailed information, check [here](./runtime/README.md).

- [`pmdk`](./pmdk/): pmdk fork that contains the modified libpmemobj that uses the enhanced `PMEMoid` structure and constructs the appropriate `tagged pointers`.

- [`plots_utils`](./plot_utils/): scripts used for creating the plots in the SPP paper. More information can be found in the respective [`README`](./plot_utils/README.md).

- [`utils/docker`](./utils/docker/): contains Dockerfiles and setup scripts to build the appropriate [compiler images](./utils/docker/compiler_images/) and [runtime environments](./utils/docker/packaged_environments/) for all the variants with all the dependencies and project installations. More information can be found in the respective [`README`](./utils/docker/README.md).

- [`hw_inst`](./hw_inst/): source code that we used to test the performance of HW instructions for bit manipulation. More information can be found in the respective [`README`](./hw_inst/README.md).

### Important files in the populated repository

- [`runtime/src/spp.c`](./runtime/src/spp.c): hook functions implementation

- [`runtime/src/wrappers_spp.c`](./runtime/src/wrappers_spp.c): the memory intrinsic functions wrappers of SPP

- [`llvm-project/llvm/lib/Transforms/SPP/spp.cpp`](llvm-project/llvm/lib/Transforms/SPP/spp.cpp): SPP module pass implementation

- [`llvm-project/llvm/lib/Transforms/IPO/SPPLTO.cpp`](llvm-project/llvm/lib/Transforms/IPO/SPPLTO.cpp): LTO pass implementation

- [`pmdk/src/libpmemobj/spp_wrappers.c`](./pmdk/src/libpmemobj/spp_wrappers.c): SPP wrappers of the core PM management functions of `libpmemobj` library of PMDK

- [`pmdk/src/include/libpmemobj/base.h`](./pmdk/src/include/libpmemobj/base.h): definition of SPP PMEMoid structure (which adds the *size* field) and the adapted `pmemobj_direct_inline` function that returns the tagged pointer for SPP.

---

### Note
Currently the passes produce a lot of debug messages.
In order to control the debug information, someone can comment out the defined `DEBUG` variables in `runtime/src/spp.c`,
`llvm-project/llvm/lib/Transforms/SPP/spp.cpp` and `llvm-project/llvm/lib/Transforms/IPO/SPPLTO.cpp`.
