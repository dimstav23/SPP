GCC=gcc
CLANG:=$(shell which clang)
CLANGPP:=$(shell which clang++)
# Add debug info and enable ASan
# CFLAGS=-g -fsanitize=address


SPPROOT:=$(shell pwd)/../..
LLVMROOT=${SPPROOT}/llvm-project
PMDKSRC=${SPPROOT}/pmdk/src

OPT_LEVEL=-O1

SPPLIB=${SPPROOT}/runtime
SPPLIBSRC=${SPPLIB}/src
SPPLIBOBJ=${SPPLIB}/obj
TESTSRC=${SPPROOT}/examples

WRAP_LIST:=-Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp \
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
           -Wl,-wrap,pmemobj_memset -Wl,-wrap,pmemobj_memset_persist

LFLAGS=-lpmem -lpmemobj
INCLUDEDIR=-I${PMDKSRC}/include/
LDFLAGS=-L${PMDKSRC}/nondebug/

CFLAGS=${OPT_LEVEL} \
-U_FORTIFY_SOURCE \
-flto \
-fuse-ld=gold \
-Xclang -load -Xclang ${LLVMROOT}/build/lib/LLVMSPP.so  \
-include ${SPPLIBSRC}/spp.h \
-I${PMDKSRC}/include/ -L${PMDKSRC}/nondebug/ \
${WRAP_LIST} ${SPPLIBOBJ}/wrappers_spp.o \
-Xlinker ${SPPLIBOBJ}/spp.o \
-fno-builtin \
-DTAG_BITS=26


all: build/gcc_attack_gen_system_heap build/clang_attack_gen_system_heap build/clang_attack_gen_pmemobj_heap build/gcc_attack_gen_pmemobj_heap

build/gcc_attack_gen_system_heap: ./source/attack_gen_system_heap.c
	${GCC} -g ./source/attack_gen_system_heap.c -o ./build/gcc_attack_gen_system_heap

build/clang_attack_gen_system_heap: ./source/attack_gen_system_heap.c
	${CLANG} -U_FORTIFY_SOURCE ./source/attack_gen_system_heap.c -o ./build/clang_attack_gen_system_heap

build/gcc_attack_gen_pmemobj_heap: ./source/attack_gen_pmemobj_heap.c
	${GCC} ./source/attack_gen_pmemobj_heap.c ${INCLUDEDIR} ${LDFLAGS} ${LFLAGS} -o ./build/gcc_attack_gen_pmemobj_heap 

build/clang_attack_gen_pmemobj_heap: ./source/attack_gen_pmemobj_heap.c
	${CLANG} ${OPT_LEVEL} -U_FORTIFY_SOURCE -S -I${PMDKSRC}/include/ -emit-llvm ./source/attack_gen_pmemobj_heap.c -o ./build/attack_gen_pmemobj_heap.ll
	opt ${OPT_LEVEL} -load "${LLVMROOT}/build/lib/LLVMSPP.so" -S ./build/attack_gen_pmemobj_heap.ll -o ./build/attack_gen_pmemobj_heap_transformed.ll
	${CLANG} -ggdb ${CFLAGS} ./source/attack_gen_pmemobj_heap.c -o ./build/clang_attack_gen_pmemobj_heap ${LFLAGS}

clean:
	rm -rf ./build/*
