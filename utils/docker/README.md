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

---

## Build images with the appropriate dependencies
In the directory [`packaged_environments`](./packaged_environments/), we provide a set of scripts, Dockerfiles and patches that build the appropriate images based on the compiler images built in the previous [step](#compiler-images) that enclose the environments of SPP, vanilla PMDK and SafePM variants, required for SPP evaluation. For detailed information, please see [here](./packaged_environments/README.md).

In a nutshell, we provide the following:

1. [`create_env_pmdk.sh`](./packaged_environments/create_env_pmdk.sh): uses [`Dockerfile_pmdk`](./packaged_environments/Dockerfile_pmdk) and builds an image with all the necessary dependencies to run the performance benchmarks - it fetches and installs `pmdk`, `libpmemobj-cpp`, `pmemkv`, `pmemkv-bench` and `phoenix`.
2. [`create_env_spp.sh`](./packaged_environments/create_env_spp.sh): uses [`Dockerfile_spp`](./packaged_environments/Dockerfile_spp) and builds an image with all the necessary dependencies to run the performance benchmarks - it fetches and installs `SPP/pmdk`, `libpmemobj-cpp`, `pmemkv`, `pmemkv-bench` and `phoenix`.
3. [`create_env_safepm.sh`](./packaged_environments/create_env_safepm.sh): uses [`Dockerfile_SafePM`](./packaged_environments/Dockerfile_safepm) and builds an image with all the necessary dependencies to run the performance benchmarks - it fetches and installs `SafePM/pmdk`, `libpmemobj-cpp`, `pmemkv`, `pmemkv-bench` and `phoenix`.

The included patches are used to adapt the build system of the packages and include the SPP pass flags, depending on the targetted environment.

**Note**: We further provide scripts to build additional environments:
- `*_O1.sh`: These scripts (with the suffix `_O1`) produce the same environment but set the optimization level to `O1`.
- `*_mock_*.sh`: These scripts produce the SPP environment but rule out the SPP pointer instrumentation by forcing the SPP injected function to immediately return without performing any action.

### How to build the images
```
$ cd PROJECT_ROOT/utils/docker/packaged_environments
$ ./create_env_pmdk.sh
$ ./create_env_spp.sh
$ ./create_env_safepm.sh
```

**Warning:** Be careful! We haven't optimized the image building process and it produces big images in size:
- `vanilla_pmdk` ~12.6GB
- `spp` ~9.85GB
- `safepm` ~13GB