#!/bin/bash

./create_SPPLIB.sh

echo "... SPP: compiled_hooks.."

make clean;
make;

echo "... SPP:  mytest_generated"
echo "................................................"

LD_LIBRARY_PATH=../../pmdk/src/debug/ ./mytest

echo "................................................"
echo "... SPP:  exiting_launch"

