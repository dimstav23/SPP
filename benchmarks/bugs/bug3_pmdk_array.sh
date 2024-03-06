echo "----- PMDK array example BUG -----"

cd /SPP/pmdk/
make clobber -j$(nproc) > /dev/null 2>&1
make clean -j$(nproc) > /dev/null 2>&1
echo "----- Patching PMDK to compile the examples instrumented with SPP -----"
sed -i 's|PROGS = manpage btree|PROGS = manpage btree pi lists setjmp buffons_needle_problem|' src/examples/libpmemobj/Makefile
sed -i 's|DIRS = hashmap tree_map map array list_map|DIRS = pminvaders pmemlog pmemblk string_store string_store_tx string_store_tx_type hashmap tree_map map array linkedlist list_map slab_allocator queue|' src/examples/libpmemobj/Makefile
make -j$(nproc) > /dev/null 2>&1

cd /SPP/runtime/
make clean > /dev/null 2>&1
make > /dev/null 2>&1

echo "----- Executing the PMDK array example instrumented with SPP -----"
cd /SPP/pmdk/src/examples/libpmemobj/array
echo "----- PMDK Array allocation -----"
LD_LIBRARY_PATH=/SPP/pmdk/src/nondebug ./array /mnt/array/array__testpool alloc test_array 100 int
echo "----- PMDK Array Reallocation (should break) -----"
LD_LIBRARY_PATH=/SPP/pmdk/src/nondebug ./array /mnt/array/array__testpool realloc test_array 100000
