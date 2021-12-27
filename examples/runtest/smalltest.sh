#!/bin/bash


#######################################
##   NOTE: SET LLVMROOT PATH BELOW!  ##
#######################################

#LLVMROOT=/PATH/TO/CLANGLLVM/INSTALL
LLVMROOT=~/.local/spp.llvm.12.0.0/

#CLANG="${LLVMROOT}/bin/clang"
CLANG=$(which clang)
OPTLEV="-O2"

SPPROOT="../../"
PMDKSRC="${SPPROOT}/pmdk/src/"

TESTSRC="${SPPROOT}/examples/smallexample/src/"
SPPLIB="${SPPROOT}/examples/smallexample/spplib/"
SPPLIBSRC="${SPPLIB}/src/"
SPPLIBOBJ="${SPPLIB}/obj/"

WRAP_LIST="-Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp -Wl,-wrap,strncpy -Wl,-wrap,strncmp -Wl,-wrap,memcmp -Wl,-wrap,memchr -Wl,-wrap,strchr -Wl,-wrap,strncat -Wl,-wrap,strtol -Wl,-wrap,strlen -Wl,-wrap,strchrnul"   

echo "--- SPPLIB: $SPPLIB"
echo "-   CLANG: $CLANG"


#   Compile spp hook functions      ###################

rm "${SPPLIBOBJ}/spp_hookobj.o"
rm "${SPPLIBOBJ}/wrappers.o"

# compiling spp.c

$CLANG -flto \
"${SPPLIBSRC}/spp.c" \
-c -o \
"${SPPLIBOBJ}/spp_hookobj.o"

# compiling wrappers.c

$(which gcc) \
-include "${SPPLIBSRC}/spp.h" \
"${SPPLIBSRC}/wrappers_spp.c" \
-c -o \
"${SPPLIBOBJ}/wrappers.o" 

echo "----- Hook functions compiled ----------"

#"-I${LLVMROOT}/lib/clang/12.0.0/include/" \

#   Building examples  #########################

$CLANG \
$OPTLEV \
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

echo "----- runtest done. exe: example"

#https://lists.llvm.org/pipermail/llvm-dev/2013-January/058038.html
