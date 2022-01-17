#!/bin/bash
####  Compile spp hooks  ############

CLANG=$(which clang)
CLANGPP=$(which clang++)
GCC=$(which gcc)
AR=$(which llvm-ar)

SPPLIB=../smallexample/spplib/

rm ${SPPLIB}/obj/wrappers.o
rm ${SPPLIB}/obj/spp_hookobj.o
#rm ${SPPLIB}/obj/libspphook.a

set -e

$CLANGPP \
${SPPLIB}/src/wrappers_spp.c \
-c -o \
${SPPLIB}/obj/wrappers.o

$CLANGPP -emit-llvm \
${SPPLIB}/src/spp.c \
-c -o \
${SPPLIB}/obj/spp_hookobj.o

set +e
#$AR q \
#${SPPLIB}/obj/libspphook.a \
#${SPPLIB}/obj/spp_hookobj.o
