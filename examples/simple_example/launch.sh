#!/bin/bash

echo ">>>>>>> SPP launch.sh starts (for example.c)"

SPPROOT="$(realpath "$(dirname "$0")")/../.."
LLVMROOT="${SPPROOT}/llvm-project"
PMDKSRC="${SPPROOT}/pmdk/src"

OPT_LEVEL="-O1"

SPPLIB="${SPPROOT}/runtime"
SPPLIBSRC="${SPPLIB}/src"
SPPLIBOBJ="${SPPLIB}/obj"
TESTSRC="${SPPROOT}/examples/simple_example"

WRAP_LIST="-Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp \
           -Wl,-wrap,strncpy -Wl,-wrap,strncmp -Wl,-wrap,memcmp \
           -Wl,-wrap,memchr -Wl,-wrap,strchr -Wl,-wrap,strncat \
           -Wl,-wrap,strtol -Wl,-wrap,strlen -Wl,-wrap,strchrnul \
           -Wl,-wrap,strcat -Wl,-wrap,snprintf \
           -Wl,-wrap,memcpy -Wl,-wrap,memset -Wl,-wrap,memmove \
           -Wl,-wrap,pmem_memmove_persist -Wl,-wrap,pmem_memcpy_persist \
           -Wl,-wrap,pmem_memmove_nodrain -Wl,-wrap,pmem_memcpy_nodrain \
           -Wl,-wrap,pmem_memmove -Wl,-wrap,pmem_memcpy \
           -Wl,-wrap,pmem_memset_nodrain -Wl,-wrap,pmem_memset \
           -Wl,-wrap,pmem_memset_persist -Wl,-wrap,pmemobj_memcpy \
           -Wl,-wrap,pmemobj_memcpy_persist -Wl,-wrap,pmemobj_memmove \
           -Wl,-wrap,pmemobj_memset -Wl,-wrap,pmemobj_memset_persist"

echo ">>>>>>> Compile spp hook functions"
pushd .
cd ${SPPLIB}
./create_spplib.sh
popd    

echo ">>>>>>> Build example"
CLANG=$(which clang)
CLANGPP=$(which clang++)

$CLANG \
$OPT_LEVEL \
-U_FORTIFY_SOURCE \
-D_FORTIFY_SOURCE=0 \
-Wno-array-bounds \
-flto \
-fuse-ld=gold \
-Xclang -load -Xclang "${LLVMROOT}/build/lib/LLVMSPP.so"  \
-include "${SPPLIBSRC}/spp.h" \
"-I${PMDKSRC}/include/" "-L${PMDKSRC}/nondebug/" \
$WRAP_LIST "${SPPLIBOBJ}/wrappers_spp.o" \
-Xlinker "${SPPLIBOBJ}/spp.o" \
-DTAG_BITS=26 -lpmem -lpmemobj \
-rdynamic "${TESTSRC}/example.c" \
-ggdb -g \
-o example #-v

echo ">>>>>>> Run example"
rm -rf /dev/shm/spp_test.pool_dim
LD_LIBRARY_PATH="${PMDKSRC}/nondebug" ./example

$CLANG $OPT_LEVEL -Wno-array-bounds -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -S -I${PMDKSRC}/include/ -emit-llvm example.c

opt $OPT_LEVEL -load "${LLVMROOT}/build/lib/LLVMSPP.so" -S example.ll -o example_transformed.ll #produce transformed .ll pass sequence plays a role here

