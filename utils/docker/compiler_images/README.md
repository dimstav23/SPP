# Compiler images

## Workflow description

We provide a set of scripts and Dockerfiles that build the appropriate images based on `ubuntu-20.04` that contain the compilers required for SPP evaluation.

More precisely, we build one image having the default `clang` and one with the SPP version of `clang`.
The default tags for these images are `llvm-clang-12-test` and `spp-llvm-clang-test` respectively.

- [`Dockerfile_clang_12`](./Dockerfile_clang_12): Dockerfile that instruments the image creation for the default `clang` compiler. It takes the `ubuntu-20.04` image as a starting point, installs the package dependencies, clones the SPP and the default LLVM repositories and runs the [`install_llvm.sh`](./install_llvm.sh) script that performs the installation of LLVM. It further exports the appropriate environment variables to be used inside the containers based on this image.
- [`Dockerfile_spp_clang_12`](./Dockerfile_spp_clang_12): Dockerfile that instruments the image creation for the SPP `clang` compiler. It takes the `ubuntu-20.04` image as a starting point, installs the package dependencies, clones the SPP repository, initializes the LLVM submodule of SPP and runs the [`install_llvm_spp.sh`](./install_llvm_spp.sh) script that performs the installation of LLVM. It further exports the appropriate environment variables to be used inside the containers based on this image.
- [`install_llvm.sh`](./install_llvm.sh): Contains the building commands of default LLVM. The default install directory is set to `/llvm`.
- [`install_llvm_spp.sh`](./install_llvm_spp.sh): Contains the building commands of SPP LLVM. The default install directory is set to `/llvm`.
- [`create_clang_12_image.sh`](./create_clang_12_image.sh): Triggers the image build for the default `clang` compiler image.
- [`create_spp_image.sh`](./create_spp_image.sh): Triggers the image build for the SPP `clang` compiler image.

**Warning:** Be careful! We haven't optimized the image building process and it produces big images in size.
The default `clang` image is approximately 7.7GB while the SPP `clang` image is approximately 5GB.
The llvm-project source code files are removed after its installation.

---

## How to run (Warning: takes some time)
```
$ ./create_clang_12_image.sh
$ ./create_spp_image.sh
```
