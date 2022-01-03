#!/bin/bash

#######################################
##   NOTE: SET LLVMROOT PATH BELOW!  ##
#######################################

# LLVMROOT: llvm install dir ################

LLVMROOT=/home/mjnam/.local/spp.llvm.12.0.0/
SPPROOT="../../"
PMDKSRC="${SPPROOT}/pmdk/src/"

SPPLIB="${SPPROOT}/examples/smallexample/spplib/"
SPPLIBSRC="${SPPLIB}/src/"
SPPLIBOBJ="${SPPLIB}/obj/"
TESTSRC="${SPPROOT}/examples/smallexample/src/"

WRAP_LIST="-Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp -Wl,-wrap,strncpy -Wl,-wrap,strncmp -Wl,-wrap,memcmp -Wl,-wrap,memchr -Wl,-wrap,strchr -Wl,-wrap,strncat -Wl,-wrap,strtol -Wl,-wrap,strlen -Wl,-wrap,strchrnul"   

#   Compile spp hook functions      ###################

bash ./create_spplib.sh

#   Building examples  #########################

CLANG=$(which clang)
CLANGPP=$(which clang++)

$CLANG \
-O2 \
-flto \
-fuse-ld=gold \
-Xclang -load -Xclang "${LLVMROOT}/lib/LLVMSPP.so"  \
-include "${SPPLIBSRC}/spp.h" \
"-I${PMDKSRC}/include/" "-L${PMDKSRC}/debug/" \
$WRAP_LIST "${SPPLIBOBJ}/wrappers.o" \
-Xlinker "${SPPLIBOBJ}/spp_hookobj.o" \
-DTAG_BITS=15 -lpmem -lpmemobj \
"${TESTSRC}/mymalloc.c" "${TESTSRC}/myfree.c" "${TESTSRC}/mystrcpy.c" "${TESTSRC}/main.c" \
-o example -v

rm -rf /dev/shm/spp_test.pool
LD_LIBRARY_PATH="${PMDKSRC}/nondebug" ./example

echo ".......... smalltest.sh complete. (exe: example) ...."

#https://lists.llvm.org/pipermail/llvm-dev/2013-January/058038.html
