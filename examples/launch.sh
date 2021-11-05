#!/bin/bash

cd ../pass
make

cd ../examples
clang -O -emit-llvm example.c -c -o example.bc

opt -load ../pass/spp.so -spp < example.bc > /dev/null

clang -Xclang -load -Xclang ../pass/spp.so -I../pmdk/src/include/ -L../pmdk/src/nondebug/ -lpmem -lpmemobj example.c -o example

rm -rf /dev/shm/spp_test.pool
LD_LIBRARY_PATH=../pmdk/src/nondebug ./example
