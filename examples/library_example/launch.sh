#!/bin/bash

SPPROOT="$(realpath "$(dirname "$0")")/../.."
LLVMROOT="${SPPROOT}/llvm-project"
PMDKSRC="${SPPROOT}/pmdk/src"

OPT_LEVEL="-O1"

SPPLIB="${SPPROOT}/runtime"
SPPLIBSRC="${SPPLIB}/src"
SPPLIBOBJ="${SPPLIB}/obj"
TESTSRC="${SPPROOT}/examples/library_example"

WRAP_LIST="-Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp \
           -Wl,-wrap,strncpy -Wl,-wrap,strncmp -Wl,-wrap,memcmp \
           -Wl,-wrap,memchr -Wl,-wrap,strchr -Wl,-wrap,strncat \
           -Wl,-wrap,strtol -Wl,-wrap,strlen -Wl,-wrap,strchrnul \
           -Wl,-wrap,strcat -Wl,-wrap,snprintf \
           -Wl,-wrap,memcpy -Wl,-wrap,memset -Wl,-wrap,memmove"

echo ">>>>>>> Compile spp hook functions"
pushd .
cd ${SPPLIB}
./create_spplib.sh
popd    

CLANG=$(which clang)
CLANGPP=$(which clang++)
AR=$(which llvm-ar)

echo ">>>>>>> Build shared library"
$CLANG \
-flto \
-shared \
-fPIC \
$OPT_LEVEL \
-U_FORTIFY_SOURCE \
-D_FORTIFY_SOURCE=0 \
-Xclang -load -Xclang "${LLVMROOT}/build/lib/LLVMSPP.so"  \
-include "${SPPLIBSRC}/spp.h" \
"-I${PMDKSRC}/include/" \
$WRAP_LIST "${SPPLIBOBJ}/wrappers_spp.o" \
"${TESTSRC}/libfuncs.c" \
-ggdb -g \
-fno-builtin \
-o libfuncs.so

echo ">>>>>>> Build static library"
$CLANG \
-c \
-flto \
-fPIC \
$OPT_LEVEL \
-U_FORTIFY_SOURCE \
-D_FORTIFY_SOURCE=0 \
-Xclang -load -Xclang "${LLVMROOT}/build/lib/LLVMSPP.so"  \
-include "${SPPLIBSRC}/spp.h" \
"-I${PMDKSRC}/include/" \
"${TESTSRC}/libfuncs.c" \
-ggdb -g \
-fno-builtin \
-o libfuncs.o

$AR \
rcs \
libfuncs.a \
libfuncs.o

echo ">>>>>>> Build dynamically linked driver"
$CLANG \
$OPT_LEVEL \
-U_FORTIFY_SOURCE \
-D_FORTIFY_SOURCE=0 \
-flto \
-fuse-ld=gold \
-Xclang -load -Xclang "${LLVMROOT}/build/lib/LLVMSPP.so"  \
-include "${SPPLIBSRC}/spp.h" \
"-I./" "-L./" \
"-I${PMDKSRC}/include/" "-L${PMDKSRC}/nondebug/" \
$WRAP_LIST "${SPPLIBOBJ}/wrappers_spp.o" \
-Xlinker "${SPPLIBOBJ}/spp.o" \
-DTAG_BITS=26 -lfuncs -lpmem -lpmemobj \
"${TESTSRC}/driver.c" \
-ggdb -g \
-fno-builtin \
-o driver_shared #-v

echo ">>>>>>> Build statically linked driver"
$CLANG \
$OPT_LEVEL \
-U_FORTIFY_SOURCE \
-D_FORTIFY_SOURCE=0 \
-flto \
-Xclang -load -Xclang "${LLVMROOT}/build/lib/LLVMSPP.so"  \
-include "${SPPLIBSRC}/spp.h" \
"-I./" \
"-I${PMDKSRC}/include/" "-L${PMDKSRC}/nondebug/" \
$WRAP_LIST "${SPPLIBOBJ}/wrappers_spp.o" \
-Xlinker "${SPPLIBOBJ}/spp.o" \
-DTAG_BITS=26 -lpmem -lpmemobj \
"${TESTSRC}/driver.c" \
-ggdb -g \
-fno-builtin \
-o driver_static \
./libfuncs.a

echo ">>>>>>> Build llvm IRs"
$CLANG $OPT_LEVEL -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -fno-builtin -S -I${PMDKSRC}/include/ -emit-llvm libfuncs.c
opt $OPT_LEVEL -load "${LLVMROOT}/build/lib/LLVMSPP.so" -S libfuncs.ll -o libfuncs_transformed.ll
$CLANG $OPT_LEVEL -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -fno-builtin -S -I${PMDKSRC}/include/ -emit-llvm driver.c
opt $OPT_LEVEL -load "${LLVMROOT}/build/lib/LLVMSPP.so" -S driver.ll -o driver_transformed.ll #produce transformed .ll pass sequence plays a role here

echo ">>>>>>> Run driver shared"
rm -rf /dev/shm/spp_test.pool_lib
LD_LIBRARY_PATH="${PMDKSRC}/nondebug:./" ./driver_shared

echo ">>>>>>> Run driver static"
rm -rf /dev/shm/spp_test.pool_lib
LD_LIBRARY_PATH="${PMDKSRC}/nondebug" ./driver_static


