#!/bin/bash

cd ../pass
make

cd ../examples

#clang -O -emit-llvm example.c -c -o example.bc #produce bitcode
#clang -S -emit-llvm example.c #produce initial .ll
#opt -load ../pass/spp.so -spp -S example.ll -o example_transformed.ll #produce transformed .ll
#llc example_transformed.ll -o example.s #produce transformed .s file

rm -rf example

#clang -g -O0 -Xclang -load -Xclang ../pass/spp.so -I../pmdk/src/include/ -L../pmdk/src/debug/ -lpmem -lpmemobj -DSPP_OFF example.c -o example
#clang -g -O0 -I../pmdk/src/include/ -L../pmdk/src/nondebug/ -DSPP_OFF -lpmem -lpmemobj example.c -o example 

clang -Xclang -load -Xclang ../pass/spp.so  -O2 -I../pmdk/src/include/ -L../pmdk/src/debug/ -DTAG_BITS=15 -lpmem -lpmemobj example.c -o example

rm -rf /dev/shm/spp_test.pool
LD_LIBRARY_PATH=../pmdk/src/nondebug ./example
