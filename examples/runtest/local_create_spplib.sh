#!/bin/bash

####  Compile spp hooks  ############

CLANG=$(which clang)
GCC=$(which gcc)
AR=$(which llvm-ar)

SPPLIB=../smallexample/spplib/

rm ${SPPLIB}/obj/wrappers.o
rm ${SPPLIB}/obj/spp_hookobj.o
rm ${SPPLIB}/obj/libspphook.a

$CLANG \
${SPPLIB}/src/local_wrappers_spp.c \
-c -o \
${SPPLIB}/obj/wrappers.o

$CLANG -emit-llvm \
${SPPLIB}/src/spp.c \
-c -o \
${SPPLIB}/obj/spp_hookobj.o

#$AR q \
#${SPPLIB}/obj/libspphook.a \
#${SPPLIB}/obj/spp_hookobj.o
