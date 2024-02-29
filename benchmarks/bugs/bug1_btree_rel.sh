set -e

echo "----- SPP MAP BTREE BUG (Release)-----"
cd /SPP/runtime
make clean > /dev/null && make > /dev/null 2>&1
cd /SPP/pmdk/src/benchmarks
make clean > /dev/null && make > /dev/null 2>&1
cp /change_pmembench_bugs_file_path.sh ./
bash change_pmembench_bugs_file_path.sh
LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map_bug.cfg
