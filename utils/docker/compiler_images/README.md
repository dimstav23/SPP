## Directory to build an image with llvm/clang installation

The `create_spp_image.sh` script clones the SPP directory, fetches the submodules and compiles the llvm-project it contains. The install directory is set to be `/llvm/`.
The llvm-project source code files are removed.

Be careful! It produces a big image in size.

## How to run
```
$ GIT_TOKEN=(your github token) ./create_spp_image.sh
```
