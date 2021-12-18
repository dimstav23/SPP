#!/bin/bash

cd ../../pass
make

cd ../examples/jinexample

./create_SPPLIB.sh

echo "... SPP: compiled_hooks.."

make clean;
make;

echo "... SPP:  mytest_generated"
echo "................................................"

LD_LIBRARY_PATH=../../pmdk/src/debug/ ./mytest

echo "................................................"
echo "... SPP:  exiting_launch"

# To get the LLVM IR code:
#opt -load ../../pass/spp.so -spp_tag_cleaning  -spp -S ./obj/llvmlinked.o -o ./obj/transformed.ll
