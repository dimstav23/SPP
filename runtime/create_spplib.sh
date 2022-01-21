#!/bin/bash

####  Compile spp hooks  ############

CLANG=$(which clang)
CLANGPP=$(which clang++)
GCC=$(which gcc)
AR=$(which llvm-ar)

echo "__CLANG: $CLANG"
echo "__ld.gold: $(which ld.gold)"

SPPLIB="$(realpath "$(dirname "$0")")"

mkdir -p ${SPPLIB}/obj

[ -e ${SPPLIB}/obj/wrappers.o ] && rm ${SPPLIB}/obj/wrappers.o
[ -e ${SPPLIB}/obj/spp_hookobj.o ] && rm ${SPPLIB}/obj/spp_hookobj.o
#rm ${SPPLIB}/obj/libspphook.a

$CLANG \
${SPPLIB}/src/wrappers_spp.c \
-c -o \
${SPPLIB}/obj/wrappers.o 

$CLANG -emit-llvm \
${SPPLIB}/src/spp.c \
-c -o \
${SPPLIB}/obj/spp_hookobj.o 


#$AR q \
#${SPPLIB}/obj/libspphook.a \
#${SPPLIB}/obj/spp_hookobj.o


