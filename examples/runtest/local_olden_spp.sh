#!/bin/bash

#################################################################
#		Graham olden_spp.sh                             # 
#################################################################
# NOTE (1) Do not combine -flto with -Xclang

source ../../../../script/myheader.sh

CLANG=$(which clang)
CLANGPP=$(which clang++)
OPT_LEVEL=-O2

USER=mjnam

# to be tested
#TEST=bisort
#   compile hooks 
. ./local_create_spplib.sh

ROOT=/home/${USER}/tools/clangllvm/
SANDBOX_DIR="${ROOT}/tmp/BAR/${BENCHMARK}/"
SPPLIBDIR=${ROOT}/spp-pass/examples/smallexample/spplib/
SPPLIBSRC=${SPPLIBDIR}/src/
SPPLIBOBJ=${SPPLIBDIR}/obj/
LLVM_TEST_SUITE_DIR=${ROOT}/llvm-test-suite/test-suite-src/ 
SPPLLVMSRC=${ROOT}/spp-pass/llvm-project/llvm/

# NOTE: Disable hooking malloc below later. 

WRAP_LIST="-Wl,-wrap,malloc -Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp -Wl,-wrap,strncpy -Wl,-wrap,strncmp -Wl,-wrap,memcmp -Wl,-wrap,memchr -Wl,-wrap,strchr -Wl,-wrap,strncat -Wl,-wrap,strtol -Wl,-wrap,strlen -Wl,-wrap,strchrnul"

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
--cflags    "$WRAP_LIST ${SPPLIBOBJ}/wrappers.o" \
--cxxflags  "$WRAP_LIST ${SPPLIBOBJ}/wrappers.o" \
--cflags    "${SPPLIBOBJ}/spp_hookobj.o" \
--cxxflags  "${SPPLIBOBJ}/spp_hookobj.o" \
--cflags    "-Xlinker -lm" \
--cxxflags  "-Xlinker -lm" \
--test-suite "${LLVM_TEST_SUITE_DIR}" \
--use-cmake="$(which cmake)" \
--use-lit="${SPPLLVMSRC}/utils/lit/lit.py" \
--benchmarking-only \
--only-test "MultiSource/Benchmarks/Olden/${TEST}" \
--use-perf=all \
--cmake-cache Debug \
-j 20;

#--cflags    "-Xlinker -lspphook" \
#--cxxflags  "-Xlinker -lspphook" \
#--cflags    "-L${SPPLIBOBJ}" \
#--cxxflags  "-L${SPPLIBOBJ}" \
