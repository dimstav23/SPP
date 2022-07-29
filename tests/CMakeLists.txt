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
        "-flto" 
	"-O2" 
	"-Xclang -load -Xclang" "${SPPBASE}/llvm-project/build/lib/LLVMSPP.so" 
	"-include" "${SPPBASE}/runtime/src/spp.h"
    	-v)
    	
	target_link_options(
        "${target}" 
        PRIVATE 
        -fuse-ld=gold
        -Wl,-wrap,free -Wl,-wrap,strcpy -Wl,-wrap,strcmp
        -Wl,-wrap,strncpy -Wl,-wrap,strncmp -Wl,-wrap,memcmp
        -Wl,-wrap,memchr -Wl,-wrap,strchr -Wl,-wrap,strncat
        -Wl,-wrap,strtol -Wl,-wrap,strlen -Wl,-wrap,strchrnul
        -Wl,-wrap,strcat -Wl,-wrap,snprintf
        -Wl,-wrap,memcpy -Wl,-wrap,memset -Wl,-wrap,memmove
        -Wl,-wrap,pmem_memmove_persist -Wl,-wrap,pmem_memcpy_persist
        -Wl,-wrap,pmem_memmove_nodrain -Wl,-wrap,pmem_memcpy_nodrain
        -Wl,-wrap,pmem_memmove -Wl,-wrap,pmem_memcpy
        -Wl,-wrap,pmem_memset_nodrain -Wl,-wrap,pmem_memset
        -Wl,-wrap,pmem_memset_persist -Wl,-wrap,pmemobj_memcpy
        -Wl,-wrap,pmemobj_memcpy_persist -Wl,-wrap,pmemobj_memmove
        -Wl,-wrap,pmemobj_memset -Wl,-wrap,pmemobj_memset_persist
        ${SPPBASE}/runtime/obj/wrappers_spp.o 
        -Xlinker ${SPPBASE}/runtime/obj/spp.o 
        -v)

	target_link_libraries(
        "${target}" 
        libpmemobj libpmem 
        ${CMAKE_DL_LIBS} 
        ${CMAKE_THREAD_LIBS_INIT} 
	ndctl daxctl)
	
	target_include_directories(
        "${target}" 
        PUBLIC 
        "${PMDK_INSTALL_DIR}/include")
endfunction()

spp_test(overflow)
spp_test(overflow_ntx)
spp_test(root_overflow)
spp_test(root_underflow)
spp_test(type_overflow)
spp_test(overflow_persistence_helper)
spp_test(free_tx_abort)
spp_test(tx_add_overflow)
spp_test(zalloc)
