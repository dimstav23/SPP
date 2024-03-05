set -e

docker build -t llvm-clang-12-test -f=Dockerfile_clang_12 .
