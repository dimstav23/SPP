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

[ -e ${SPPLIB}/obj/wrappers_spp.o ] && rm ${SPPLIB}/obj/wrappers_spp.o
[ -e ${SPPLIB}/obj/spp.o ] && rm ${SPPLIB}/obj/spp.o
#rm ${SPPLIB}/obj/libspphook.a

$CLANG \
${SPPLIB}/src/wrappers_spp.c \
-c -O1 -o \
${SPPLIB}/obj/wrappers_spp.o 

$CLANG \
${SPPLIB}/src/spp.c \
-c -O1 -o \
${SPPLIB}/obj/spp.o 


#$AR q \
#${SPPLIB}/obj/libspphook.a \
#${SPPLIB}/obj/spp.o


