#!/bin/bash

#################################################################
#		Graham olden_spp.sh                             # 
#################################################################

MYMSG="!> spp_msg:: "

source ../../../../script/myheader.sh

WRAP_LIST="-Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp -Wl,-wrap,strncpy -Wl,-wrap,strncmp -Wl,-wrap,memcmp -Wl,-wrap,memchr -Wl,-wrap,strchr -Wl,-wrap,strncat -Wl,-wrap,strtol -Wl,-wrap,strlen -Wl,-wrap,strchrnul"

# NOTE (1) Do not combine -flto with -Xclang

ROOT=/home/mjnam/tools/clangllvm/
SANDBOX_DIR="${ROOT}/tmp/BAR/${BENCHMARK}/"
SPPLIBDIR=${ROOT}/spp-pass/examples/smallexample/spplib/
#SPPLIBDIR=../smallexample/spplib/
echo "$MYMSG SPPLIBDIR is $SPPLIBDIR .."

# compile runtimelibs

SPPLIBSRC=${SPPLIBDIR}/src/
SPPLIBOBJ=${SPPLIBDIR}/obj/
LLVM_TEST_SUITE_DIR=${ROOT}/llvm-test-suite/test-suite-src/ 
SPPLLVMSRC=${ROOT}/spp-pass/llvm-project/llvm/

CLANG=$(which clang)
CLANGPP=$(which clang++)
GCC=$(which gcc)
echo "GCC: $GCC"
echo "llvm-ar: $(which llvm-ar)"

TEMP=../smallexample/spplib/

#rm ${TEMP}/obj/spp_hookobj.o
#rm ${TEMP}/obj/libspphook.a
#rm ${TEMP}/obj/wrappers.o

#$CLANG -emit-llvm  \
#${TEMP}/src/spp.c \
#-c -o ${TEMP}/obj/spp_hookobj.o 

#$LLVMAR q ${TEMP}/obj/libspphook.a ${OBJ}/spp_hookobj.o

#$CLANG -flto $OPT_LEVEL \
#${SPPLIBSRC}/spp.c \
#-c -o ${SPPLIBOBJ}/spp_hookobj.o 

#$GCC \
#-include "${TEMP}/src/spp.h" \
#${TEMP}/src/wrappers_spp.c \
#-c -o ${TEMP}/obj/wrappers.o

#$GCC \
#-include "${SPPLIBSRC}/spp.h" \
#${SPPLIBSRC}/wrappers_spp.c \
#-c -o ${SPPLIBOBJ}/wrappers.o

echo "$MYMSG PWD: $PWD"
echo "$MYMSG CLANG is $CLANG .."
echo "$MYMSG CLANGPP is $CLANGPP .."
echo "$MYMSG SPPLIBDIR is $SPPLIBDIR .."
echo "$MYMSG SPPLIBOBJ is $SPPLIBOBJ .."
echo "$MYMSG test-suite is $LLVM_TEST_SUITE_DIR .."
echo "$MYMSG OPT_LEVEL is $OPT_LEVEL .."

#   Activate virtualenv
#echo "$MYMSG activate virtualenv"
#source ~/tools/mysandbox/bin/activate

#SANDBOX_DIR="${SANDBOX_HOME}/${BENCHMARK}/"


echo "$MYMSG run_lnt ......... "

lnt runtest test_suite \
--sandbox ${SANDBOX_DIR} \
--cc  "$CLANG" \
--cxx "$CLANGPP" \
--cflags    "${OPT_LEVEL}" \
--cxxflags  "${OPT_LEVEL}" \
--cflags    "-flto -fuse-ld=gold" \
--cxxflags  "-flto -fuse-ld=gold" \
--cflags    "-Xclang -load -Xclang ${SPPLLVMPASS}"  \
--cxxflags  "-Xclang -load -Xclang ${SPPLLVMPASS}"  \
--cflags    "-Xclang -include -Xclang ${SPPLIBSRC}/spp.h" \
--cxxflags  "-Xclang -include -Xclang ${SPPLIBSRC}/spp.h" \
--cflags    "-L${SPPLIBOBJ}" \
--cxxflags  "-L${SPPLIBOBJ}" \
--cflags    "$WRAP_LIST ${SPPLIBOBJ}/wrappers.o" \
--cxxflags  "$WRAP_LIST ${SPPLIBOBJ}/wrappers.o" \
--cflags    "-Xlinker -lspphook" \
--cxxflags  "-Xlinker -lspphook" \
--cflags    "-Xlinker -lm" \
--cxxflags  "-Xlinker -lm" \
--test-suite "${LLVM_TEST_SUITE_DIR}" \
--use-cmake="$(which cmake)" \
--use-lit="${SPPLLVMSRC}/utils/lit/lit.py" \
--benchmarking-only \
--only-test "MultiSource/Benchmarks/Olden/bh" \
--use-perf=all \
--cmake-cache Debug \
-j 10;


#"-I${PMDKSRC}/include/" "-L${PMDKSRC}/debug/" \
#-DTAG_BITS=15 -lpmem -lpmemobj \
#--cflags    "-Xlinker -plugin -Xlinker ${SPPLLVMGOLD}" \
#--cxxflags  "-Xlinker -plugin -Xlinker ${SPPLLVMGOLD}" \

#--cflags    "-Xlinker  ${SPPLIBOBJ}/spp_hookobj.o" \
#--cxxflags  "-Xlinker  ${SPPLIBOBJ}/spp_hookobj.o" \
#
#   Deactivate virtualenv
#
#--test-externals "$SPEC_HOME" \

#deactivate

#--cflags    "-include ${SPPLIBSRC}/spp.h" \
#--cxxflags  "-include ${SPPLIBSRC}/spp.h" \
#--cflags    "-include ${SPPLIB}/src/spp.h -I/home/poolmoo/.local/clangllvm.12.0.0/lib/clang/12.0.0/include/" \
#--cxxflags  "-include ${SPPLIB}/src/spp.h -I/home/poolmoo/.local/clangllvm.12.0.0/lib/clang/12.0.0/include/" \
#--cflags    "-Xlinker -plugin -Xlinker ${SPPLLVMGOLD}" \
#--cxxflags  "-Xlinker -plugin -Xlinker ${SPPLLVMGOLD}" \

#--cc  "${BUILD_SPEC_SH}/lnt_fwd.sh" \
#--cxx "${BUILD_SPEC_SH}/lnt_fwd_cpp.sh" \
#--cflags    "-Xlinker -wrap -Xlinker malloc ${OBJ}/wrappers.o" \
#--cxxflags  "-Xlinker -wrap -Xlinker malloc ${OBJ}/wrappers.o" \
#--cflags    "-include ${SPPLIB}/src/spp.h -I${PMDKSRC}/include/ -I/home/poolmoo/.local/clangllvm.12.0.0/lib/clang/12.0.0/include/" \
#--cxxflags  "-include ${SPPLIB}/src/spp.h -I${PMDKSRC}/include/ -I/home/poolmoo/.local/clangllvm.12.0.0/lib/clang/12.0.0/include/" \

#
#

#--cflags    "${OPT_LEVEL} -ld-path=${LD_GOLD}" \
#--cxxflags  "${OPT_LEVEL} -ld-path=${LD_GOLD}" \
#--cflags   "-Xlinker ${NIO_LLVM_BUILD_DIR}/miu/hook_globals_defs.o" \
#--cxxflags "-Xlinker ${NIO_LLVM_BUILD_DIR}/miu/hook_globals_defs.o" \
###################################################################
###  README  ######################################################
### Setting. FRAMER. Only tag-cleaning at memory access.
## NO instrumenting allocation 
#   global/alloca intrumentation disabled by pass, 
#   heap disabled by pass, linker, and running script. 
#   (heap interposed at compile time for type detection.
## NO instrumenting bitcast
## Intrumenting memory access, with FRAMER_clean_tag
#   load/store hooked by pass.
#   string funcs hooked by linker (wrappers.o)
#       -except mem intrinsic (memset,memcpy,memmov)
###################################################################

#-Wl,-wrap,malloc -Wl,-wrap,realloc -Wl,-wrap,calloc -Wl,-wrap,free 

#--cflags "-Wl,-wrap,malloc -Wl,-wrap,realloc -Wl,-wrap,calloc -Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp -Wl,-wrap,strncpy -Wl,-wrap,strncmp -Wl,-wrap,memcmp -Wl,-wrap,memchr -Wl,-wrap,strchr -Wl,-wrap,strncat -Wl,-wrap,strtol -Wl,-wrap,strlen  -Wl,-wrap,strchrnul ${NIO_LLVM_BUILD_DIR}/miu/wrappers.o" \
#--cxxflags "-Wl,-wrap,malloc -Wl,-wrap,realloc -Wl,-wrap,calloc -Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp -Wl,-wrap,strncpy -Wl,-wrap,strncmp -Wl,-wrap,memcmp -Wl,-wrap,memchr -Wl,-wrap,strchr -Wl,-wrap,strncat -Wl,-wrap,strtol -Wl,-wrap,strlen  -Wl,-wrap,strchrnul ${NIO_LLVM_BUILD_DIR}/miu/wrappers.o" \

