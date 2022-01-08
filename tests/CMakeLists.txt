# Original author: Kartal Kaan Bozdoğan (mstniy)
# Source repository: https://github.com/mstniy/safepm

cmake_minimum_required (VERSION 3.1)

#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang++)

function (spp_test target)
	include(CheckIPOSupported)
    add_executable("${target}" "${target}.cpp")
	target_compile_options(
        "${target}" 
        PRIVATE 
        "-flto" "-O2" 
        "-load" "/home/mjnam/.local/spp.llvm.12.0.0/lib/LLVMSPP.so" 
        "-include" "/home/mjnam/tools/clangllvm/spp-pass/runtime/src/spp.h"
        #        -ggdb 
    -v)
    target_link_options(
        "${target}" 
        PRIVATE 
        #-ggdb 
        -fuse-ld=gold 
        -Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp -Wl,-wrap,strncpy -Wl,-wrap,strncmp -Wl,-wrap,memcmp -Wl,-wrap,memchr -Wl,-wrap,strchr -Wl,-wrap,strncat -Wl,-wrap,strtol -Wl,-wrap,strlen -Wl,-wrap,strchrnul 
        /home/mjnam/tools/clangllvm/spp-pass/runtime/obj/wrappers.o 
        -Xlinker /home/mjnam/tools/clangllvm/spp-pass/runtime/obj/spp_hookobj.o 
        -v
    )
	target_link_libraries(
        "${target}" 
        libpmemobj libpmem 
        ${CMAKE_DL_LIBS} 
        ${CMAKE_THREAD_LIBS_INIT} ndctl daxctl)
	target_include_directories(
        "${target}" 
        PUBLIC 
        "${PMDK_INSTALL_DIR}/include")
endfunction()

spp_test(double_free)
spp_test(double_free_ntx)
spp_test(use_after_free)
spp_test(use_after_free_ntx)
spp_test(use_after_realloc)
spp_test(mismatched_free)
spp_test(overflow)
spp_test(tx_add_overflow)
spp_test(overflow_ntx)
spp_test(root_overflow)
spp_test(root_underflow)
spp_test(int32)
spp_test(alloc_tx_abort)
spp_test(free_tx_abort)
spp_test(zalloc)
spp_test(overflow_persistence_helper)
