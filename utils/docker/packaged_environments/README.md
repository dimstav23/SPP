# Packaged environments

## Workflow description

we provide a set of scripts, Dockerfiles and patches that build the appropriate images based on the compiler images built in the compiler images (described [here](../compiler_images/README.md)) that enclose the environments of SPP, vanilla PMDK and SafePM variants, required for SPP evaluation.

Overall, these images include the following projects:
1. `PMDK`
2. `libpmemobj-cpp`
3. `pmemkv`
4. `pmemkv-bench`
5. `phoenix`

More precisely, we build one image for each variant, with the following tags:
- `vanilla_pmdk` for the vanilla PMDK variant
- `spp` for the SPP variant
- `safepm` for the SafePM variant

The files of this repository are explained below:
- [`Dockerfile_pmdk`](./Dockerfile_pmdk): Dockerfile that instruments the image creation for the vanilla PMDK environment based on the default `clang` image. Initially it installs further package dependencies required for the projects. It then fetches the appropriate version of the repositories and applies the respective patches. Then, it contains the steps to build and install these projects in the image. 
- [`Dockerfile_spp`](./Dockerfile_spp): Dockerfile that instruments the image creation for the SPP environment based on the SPP `clang` image. Initially it installs further package dependencies required for the projects. It then fetches the appropriate version of the repositories and applies the respective patches. Then, it contains the steps to build and install these projects in the image. 
- [`Dockerfile_SafePM`](./Dockerfile_safepm): Dockerfile that instruments the image creation for the SafePM environment based on the default `clang` image. Initially it installs further package dependencies required for the projects. It then fetches the appropriate version of the repositories and applies the respective patches. Then, it contains the steps to build and install these projects in the image. 
- [`create_env_pmdk.sh`](./create_env_pmdk.sh): uses [`Dockerfile_pmdk`](./Dockerfile_pmdk) and builds an image with all the necessary dependencies to run the performance benchmarks.
- [`create_env_spp.sh`](./create_env_spp.sh): uses [`Dockerfile_spp`](./Dockerfile_spp) and builds an image with all the necessary dependencies to run the performance benchmarks.
- [`create_env_safepm.sh`](./create_env_safepm.sh): uses [`Dockerfile_SafePM`](./Dockerfile_safepm) and builds an image with all the necessary dependencies to run the performance benchmarks.
- `*_O1.sh`: These scripts (with the suffix `_O1`) produce the same environment but set the optimization level to `O1`.
- `*_mock_*.sh`: These scripts produce the SPP environment but rule out the SPP pointer instrumentation by forcing the SPP injected function to immediately return without performing any action.
- `*.patch*`: A set of patches (broken down per package) that are used to adapt the build system of the packages, include the SPP pass and optimization flags, depending on the targetted environment.

**Warning:** Be careful! We haven't optimized the image building process and it produces big images in size:
- `vanilla_pmdk` ~12.6GB
- `spp` ~9.85GB
- `safepm` ~13GB

---

## How to run
```
$ cd PROJECT_ROOT/utils/docker/packaged_environments
$ ./create_env_pmdk.sh
$ ./create_env_spp.sh
$ ./create_env_safepm.sh
```
