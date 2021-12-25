#!/bin/bash
#################################################################
#                       applyspp.sh                             # 
#################################################################

#   PATH setting: consider myheader.sh    #
#. ~/tools/clangllvm/miu.2021.llvm.12.0/miu_lib/miu_scripts/myheader.sh

#   Activate virtualenv
echo "$MYMSG activate virtualenv"
source ~/tools/mysandbox/bin/activate

SANDBOX_DIR="${SANDBOX_HOME}/"
#SANDBOX_DIR="${SANDBOX_HOME}/${BENCHMARK}/"

CMAKE="/home/poolmoo/.local/bin/cmake"

echo "$MYMSG TEST-SUITE PATH: $LLVM_TEST_SUITE_DIR"
echo "$MYMSG run_lnt"

#CMAKE="/home/poolmoo/.local/clangllvm.12.0.0/bin/cmake"

lnt runtest test_suite \
--sandbox "${SANDBOX_DIR}" \
--cc  "$CLANG" \
--cxx "$CLANGPP" \
--cflags    "${OPT_LEVEL}" \
--cxxflags  "${OPT_LEVEL}" \
--cflags    "-Xclang -flto -fuse-ld=/usr/bin/ld.gold" \
--cxxflags  "-Xclang -flto -fuse-ld=/usr/bin/ld.gold" \
--cflags    "-include ${SPPLIB}/src/spp.h -I/home/poolmoo/.local/clangllvm.12.0.0/lib/clang/12.0.0/include/" \
--cxxflags  "-include ${SPPLIB}/src/spp.h -I/home/poolmoo/.local/clangllvm.12.0.0/lib/clang/12.0.0/include/" \
--cflags    "-Xclang -load -Xclang ${SPP_LLVM_INSTALL}/lib/LLVMSPP.so" \
--cxxflags  "-Xclang -load -Xclang ${SPP_LLVM_INSTALL}/lib/LLVMSPP.so" \
--cflags    "-Xlinker ${SPPLIBOBJ}/spp_hookobj.o" \
--cxxflags  "-Xlinker ${SPPLIBOBJ}/spp_hookobj.o " \
--cflags    "-Xlinker -lm" \
--cxxflags  "-Xlinker -lm" \
--cflags    "-Xlinker -plugin -Xlinker ${SPPLLVMGOLD}" \
--cxxflags  "-Xlinker -plugin -Xlinker ${SPPLLVMGOLD}" \
--test-suite "${LLVM_TEST_SUITE_DIR}" \
--use-cmake="$CMAKE" \
--use-lit="/home/poolmoo/tools/clangllvm/clangllvm.12.0.0/llvm-project/llvm/utils/lit/lit.py" \
--benchmarking-only \
--only-test "MultiSource/Benchmarks/Olden/bh" \
--use-perf=all \
--cmake-cache Debug \
-j 1;
#
#   Deactivate virtualenv
#
#--test-externals "$SPEC_HOME" \

deactivate

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

