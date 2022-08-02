GCC=gcc
CLANG:=$(shell which clang)
CLANGPP:=$(shell which clang++)

OPT_LEVEL=-O2

ifndef INTACT
# Container specific directories
SPPROOT:=/spp-pass
LLVMROOT=/llvm
PMDKSRC=/spp-pass/pmdk/src

SPPLIB=${SPPROOT}/runtime
SPPLIBSRC=${SPPLIB}/src
SPPLIBOBJ=${SPPLIB}/obj
TESTSRC=${SPPROOT}/examples

WRAP_LIST=-Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp \
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
-flto \
-fuse-ld=gold \
-Xclang -load -Xclang ${LLVMROOT}/lib/LLVMSPP.so  \
-include ${SPPLIBSRC}/spp.h \
-I${PMDKSRC}/include/ -L${PMDKSRC}/nondebug/ \
${WRAP_LIST} ${SPPLIBOBJ}/wrappers_spp.o \
-Xlinker ${SPPLIBOBJ}/spp.o \
-DTAG_BITS=26

else
LDFLAGS+=-L/pmdk/src/nondebug/
CFLAGS=${OPT_LEVEL} -I/pmdk/src/include
endif

all: build/recovery_run

build/recovery_run: ./source/recovery_run.c
	mkdir -p ./build
	${CLANG} ${CFLAGS} ${LDFLAGS} ./source/recovery_run.c -o ./build/recovery_run -lpmemobj -lpmem

clean:
	rm -f ./build/*