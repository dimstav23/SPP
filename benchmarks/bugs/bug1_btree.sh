set -e

cd /spp-pass/pmdk/src/benchmarks
cp /change_pmembench_bugs_file_path.sh ./
bash change_pmembench_bugs_file_path.sh
echo "----- SPP MAP BTREE BUG-----"
LD_LIBRARY_PATH=../debug ./pmembench pmembench_map_bug.cfg