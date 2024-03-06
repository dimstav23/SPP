echo "----- SPP Phoenix benchmark (string_match) BUG -----"
### For SPP: Ensure that both the runtime library and the PMDK was compiled with TAG_BITS=31
cd /SPP/pmdk/
make clobber -j$(nproc) > /dev/null 2>&1
make clean -j$(nproc) > /dev/null 2>&1
make -j$(nproc) TAG_BITS=31 > /dev/null 2>&1
cd /SPP/runtime/
make clean > /dev/null 2>&1
make TAG_BITS=31 > /dev/null 2>&1

### Compile the phoenix benchmark
cd /phoenix/PM_phoenix-2.0
# Change the path of the PM pool directory
sed -i 's|/mnt/pmem0/dimitrios/|/mnt/phoenix/|' tests/string_match/string_match.c
# Revert the mem_alloc into its original size inside the intact phoenix benchmark (https://github.com/dimstav23/phoenix/blob/b8aa166e15220c9a31d43efd13067478c7114f34/phoenix-2.0/tests/string_match/string_match.c#L259)
# We identified this bug and changed it in our PM version (https://github.com/dimstav23/phoenix/blob/b8aa166e15220c9a31d43efd13067478c7114f34/PM_phoenix-2.0/tests/string_match/string_match.c#L276)
echo "----- Reverting SPP phoenix string_match allocation to its original size -----"
sed -i 's|finfo_keys.st_size + 1|finfo_keys.st_size|' tests/string_match/string_match.c
make clean > /dev/null 2>&1
make TAG_BITS=31 > /dev/null 2>&1

echo "----- Executing SPP phoenix string_match -----"
cd /phoenix/PM_phoenix-2.0/tests/string_match
if [ ! -d "string_match_datafiles" ]; then
  wget http://csl.stanford.edu/\~christos/data/string_match.tar.gz
  tar -xvf string_match.tar.gz
fi

# Run the erroneous phoenix application
LD_LIBRARY_PATH=/SPP/pmdk/src/nondebug ./string_match string_match_datafiles/key_file_500MB.txt
