## Directories to build the environments for spp, vanilla pmdk and safepm (to be added) versions

### Build images with the appropriate llvm compilers
In the directory `compiler_images`:
1. `create_clang_12_image.sh`: use `Dockerfile_clang_12` and build an image with compiled and configured the llvm-12 and clang-12 vanilla verisons - the llvm build flags can be found in `install_llvm.sh` script.
2. `create_spp_image.sh`: use `Dockerfile_spp_clang_12` and build an image with compiled and configured the llvm-12 and clang-12 vanilla verisons - the llvm build flags can be found in `install_llvm_spp.sh` script.

The install directory of llvm is set to be `/llvm/`.
The llvm-project source code files are removed.

*Be careful!* It produces a big image in size and takes considerable time to compile.

### How to build the images
```
$ cd ./compiler_images
$ GIT_TOKEN=(your github token) ./create_clang_12_image.sh
$ GIT_TOKEN=(your github token) ./create_spp_image.sh
```
The names of the built images are assigned in the scripts;`llvm-clang-12-test` for *vanilla llvm version* and `spp-llvm-clang-test` for *SPP llvm version*.

### Build images with the appropriate dependencies
In the directory `packaged_environments`:
1. `create_env_pmdk.sh`: use `Dockerfile_pmdk` and build an image with all the necessary dependencies to run the performance benchmarks - it fetches and installs *pmdk, libpmemobj-cpp, pmemkv, pmemkv-bench*.
2. `create_env_spp.sh`: use `Dockerfile_spp` and build an image with all the necessary dependencies to run the performance benchmarks - it fetches and installs *spp-pass/pmdk, libpmemobj-cpp, pmemkv, pmemkv-bench*.

The included patches are used to adapt the build system of the packages to include the *SPP* pass flags.

### How to build the images
```
$ cd ./packaged_environments
$ ./create_env_pmdk.sh
$ ./create_env_spp.sh
```
The names of the built images are assigned in the scripts;`vanilla_pmdk` for *vanilla pmdk version* and `spp` for *SPP version*.