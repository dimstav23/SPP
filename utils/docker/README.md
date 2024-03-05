# Utilities to build the docker images for the environments of SPP, vanilla PMDK and SafePM variants

## Compiler images
In the directory [`compiler_images`](./compiler_images/), we provide a set of scripts and Dockerfiles that build the appropriate images based on `ubuntu-20.04` that contain the compilers required for SPP evaluation. For detailed information, please see [here](./compiler_images/README.md).

In a nutshell, we provide the following:

1. [`create_clang_12_image.sh`](./compiler_images/create_clang_12_image.sh): uses [`Dockerfile_clang_12`](./compiler_images/Dockerfile_clang_12) and builds an image with compiled and configured the `llvm-12` and `clang-12` default versions - the LLVM build flags can be found in [`install_llvm.sh`](./compiler_images/install_llvm.sh) script.
2. [`create_spp_image.sh`](./compiler_images/create_spp_image.sh): uses [`Dockerfile_spp_clang_12`](./compiler_images/Dockerfile_spp_clang_12) and builds an image with compiled and configured the `llvm-12` and `clang-12` SPP versions - the LLVM build flags can be found in [`install_llvm_spp.sh`](./compiler_images/install_llvm_spp.sh) script.

### How to build the images
```
$ cd PROJECT_ROOT/utils/docker/compiler_images
$ ./create_clang_12_image.sh
$ ./create_spp_image.sh
```
The names of the built images are assigned in the scripts:
- `llvm-clang-12-test` for *default LLVM version* and
- `spp-llvm-clang-test` for *SPP LLVM version*.

**Warning:** Be careful! We haven't optimized the image building process and it produces big images in size.
The default `clang` image is approximately 7.7GB while the SPP `clang` image is approximately 5GB.
The llvm-project source code files are removed after its installation.

### Build images with the appropriate dependencies
In the directory `packaged_environments`:
1. `create_env_pmdk.sh`: use `Dockerfile_pmdk` and build an image with all the necessary dependencies to run the performance benchmarks - it fetches and installs *pmdk, libpmemobj-cpp, pmemkv, pmemkv-bench*.
2. `create_env_spp.sh`: use `Dockerfile_spp` and build an image with all the necessary dependencies to run the performance benchmarks - it fetches and installs *SPP/pmdk, libpmemobj-cpp, pmemkv, pmemkv-bench*.

The included patches are used to adapt the build system of the packages to include the *SPP* pass flags.

### How to build the images
```
$ cd PROJECT_ROOT/utils/docker/packaged_environments
$ ./create_env_pmdk.sh
$ ./create_env_spp.sh
$ ./create_env_safepm.sh
```
The names of the built images are assigned in the scripts;`vanilla_pmdk` for *vanilla pmdk version* and `spp` for *SPP version*.