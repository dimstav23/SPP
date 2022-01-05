#!/bin/bash

#################################################################
#		Graham olden_baseline.sh                             # 
#################################################################
# NOTE (1) Do not combine -flto with -Xclang

source ../../../../script/myheader.sh

CLANG=$(which clang)
CLANGPP=$(which clang++)
OPT_LEVEL=-O2

USER=mjnam

# to be tested
#TEST=bisort

ROOT=/home/${USER}/tools/clangllvm/
SANDBOX_DIR="${ROOT}/tmp/BAR_Baseline/${BENCHMARK}/"
#SPPLIBDIR=${ROOT}/spp-pass/examples/smallexample/spplib/
#SPPLIBSRC=${SPPLIBDIR}/src/
#SPPLIBOBJ=${SPPLIBDIR}/obj/
LLVM_TEST_SUITE_DIR=${ROOT}/llvm-test-suite/test-suite-src/ 
SPPLLVMSRC=${ROOT}/spp-pass/llvm-project/llvm/

# NOTE: Disable hooking malloc below later. 

lnt runtest test_suite \
--sandbox ${SANDBOX_DIR} \
--cc  "$CLANG" \
--cxx "$CLANGPP" \
--cflags    "${OPT_LEVEL}" \
--cxxflags  "${OPT_LEVEL}" \
--cflags    "-flto -fuse-ld=gold" \
--cxxflags  "-flto -fuse-ld=gold" \
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
