#!/bin/bash

#https://lists.llvm.org/pipermail/llvm-dev/2013-January/058038.html

. /home/poolmoo/tools/clangllvm/miu.2021.llvm.12.0/miu_lib/miu_scripts/myheader.sh

CLANG="${SPP_LLVM_INSTALL}/bin/clang"

$CLANG \
-O1 -flto \
-Xclang -load -Xclang "${SPP_LLVM_INSTALL}/lib/LLVMSPP.so"  \
-include "${SPPLIBSRC}/spp.h" \
"-I${PMDKSRC}/include/" "-L${PMDKSRC}/debug/" \
-Xlinker "${SPPLIBOBJ}/spp_hookobj.o" \
-DTAG_BITS=15 -lpmem -lpmemobj \
"${SPPLIB}/../src/mymalloc.c" "${SPPLIB}/../src/myfree.c" "${SPPLIB}/../src/mystrcpy.c" "${SPPLIB}/../src/main.c" \
-o example

rm -rf /dev/shm/spp_test.pool
LD_LIBRARY_PATH="${PMDKSRC}/nondebug" ./example

#-fuse-ld=/usr/bin/ld.gold \
#-Xlinker -plugin -Xlinker "$SPPLLVMGOLD" \
