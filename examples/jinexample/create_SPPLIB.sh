#!/bin/bash

CLANG="clang"
SPPLIBDIR="./spplib"
LLVM_AR="llvm-ar"

$CLANG -emit-llvm "${SPPLIBDIR}/src/spp.c" \
-c -o "${SPPLIBDIR}/obj/spp_hookobj.o"  

$LLVM_AR q "${SPPLIBDIR}/obj/libspp.a" "${SPPLIBDIR}/obj/spp_hookobj.o"

# llvm-ar is redundant at the moment.
# check symbols with "llvm-nm"

