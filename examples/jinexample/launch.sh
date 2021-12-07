#!/bin/bash


# PATHs

CLANG="/home/poolmoo/.local/clangllvm.12.0.0/bin/clang"
LLVMGOLD="/home/poolmoo/.local/spp.llvm.12.0.0/lib/LLVMgold.so"
GOLD_LD="/usr/bin/ld.gold"

#SRCDIR="${EX_DIR}/src/"
#OBJDIR="${EX_DIR}/obj/"

SRCDIR="./src/"
OBJDIR="./obj/"

"$CLANG" -flto "${SRCDIR}/main.c" -c -o "${OBJDIR}/main.c.o"
"$CLANG" -flto "${SRCDIR}/mymalloc.c" -c -o "${OBJDIR}/mymalloc.c.o"
"$CLANG" -flto "${SRCDIR}/myfree.c" -c -o "${OBJDIR}/myfree.c.o"

"$CLANG" \
-Xlinker -plugin -Xlinker ${LLVMGOLD} \
"${OBJDIR}/mymalloc.c.o" "${OBJDIR}/myfree.c.o" "${OBJDIR}/main.c.o" \
-o spptest


#-O -I../pmdk/src/include/ -emit-llvm example.c -c -o example.bc #produce bitcode
#"$CLANG" -S -I../pmdk/src/include/ -emit-llvm example.c #produce initial .ll

#cd ../pass
#make

#cd ../examples

#clang -O -I../pmdk/src/include/ -emit-llvm example.c -c -o example.bc #produce bitcode
#clang -S -I../pmdk/src/include/ -emit-llvm example.c #produce initial .ll
#opt -load ../pass/spp.so -spp_tag_cleaning -spp -S example.ll -o example_transformed.ll #produce transformed .ll pass sequence plays a role here
#llc example_transformed.ll -o example.s #produce transformed .s file -- https://subscription.packtpub.com/book/application-development/9781785285981/1/ch01lvl1sec16/transforming-llvm-ir

#rm -rf example

##clang -g -O0 -Xclang -load -Xclang ../pass/spp.so -I../pmdk/src/include/ -L../pmdk/src/debug/ -lpmem -lpmemobj -DSPP_OFF example.c -o example
##clang -g -O0 -I../pmdk/src/include/ -L../pmdk/src/nondebug/ -DSPP_OFF -lpmem -lpmemobj example.c -o example 

##https://lists.llvm.org/pipermail/llvm-dev/2013-January/058038.html
#clang -Xclang -load -Xclang ../pass/spp.so  -O2 -I../pmdk/src/include/ -L../pmdk/src/debug/ -DTAG_BITS=15 -lpmem -lpmemobj example.c -o example

#rm -rf /dev/shm/spp_test.pool
#LD_LIBRARY_PATH=../pmdk/src/nondebug ./example
