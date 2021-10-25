#!/bin/bash

cd pass
make

cd ..
clang -O -emit-llvm example.c -c -o example.bc

opt -load pass/spp.so -spp < example.bc > /dev/null
