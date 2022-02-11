#!/bin/bash

echo ">>>>>>> SPP. smalltest.sh starts.. (exe: example) ...."

#######################################
##   NOTE: SET LLVMROOT PATH BELOW!  ##
#######################################

##   Jin: This test instruments malloc for tag testing.

# LLVMROOT: llvm install dir ################

SPPBASE="$(realpath "$(dirname "$0")")/../.."

LLVMROOT="${SPPBASE}/llvm-project"
SPPROOT="${SPPBASE}"
PMDKSRC="${SPPROOT}/pmdk/src/"

OPT_LEVEL="-O2"

SPPLIB="${SPPROOT}/examples/smallexample/spplib/"
SPPLIBSRC="${SPPLIB}/src/"
SPPLIBOBJ="${SPPLIB}/obj/"
TESTSRC="${SPPROOT}/examples/smallexample/src/"

WRAP_LIST="-Wl,-wrap,malloc -Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp -Wl,-wrap,strncpy -Wl,-wrap,strncmp -Wl,-wrap,memcmp -Wl,-wrap,memchr -Wl,-wrap,strchr -Wl,-wrap,strncat -Wl,-wrap,strtol -Wl,-wrap,strlen -Wl,-wrap,strchrnul"   

#   Compile spp hook functions      ###################

. ./local_create_spplib.sh

rm -rf ${TESTSRC}/../obj/*.o;

#   Building examples  #########################


CLANG=$(which clang)
CLANGPP=$(which clang++)

$CLANG \
$OPT_LEVEL \
-flto \
-fuse-ld=gold \
-Xclang -load -Xclang "${LLVMROOT}/build/lib/LLVMSPP.so"  \
-include "${SPPLIBSRC}/spp.h" \
"-I${PMDKSRC}/include/" "-L${PMDKSRC}/debug/" \
$WRAP_LIST "${SPPLIBOBJ}/wrappers.o" \
-Xlinker "${SPPLIBOBJ}/spp_hookobj.o" \
-DTAG_BITS=15 -lpmem -lpmemobj \
"${TESTSRC}/mymalloc.c" "${TESTSRC}/myfree.c" "${TESTSRC}/mymemcpy.c" "${TESTSRC}/main.c" \
-o example -v

rm -rf /dev/shm/spp_test.pool
LD_LIBRARY_PATH="${PMDKSRC}/nondebug" ./example


#https://lists.llvm.org/pipermail/llvm-dev/2013-January/058038.html
