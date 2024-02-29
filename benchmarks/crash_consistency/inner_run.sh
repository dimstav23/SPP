set -e

cd /SPP/pmdk/src/benchmarks
echo "----- CRASH CONSISTENCY MAP BENCHMARK WITH MEMCHECK -----"
LD_LIBRARY_PATH=../nondebug valgrind --log-file="/results/map_memcheck" --tool=memcheck ./pmembench pmembench_map.cfg
echo "----- CRASH CONSISTENCY MAP BENCHMARK WITH PMEMCHECK -----"
LD_LIBRARY_PATH=../nondebug valgrind --log-file="/results/map_pmemcheck" --tool=pmemcheck ./pmembench pmembench_map.cfg

echo "----- CRASH CONSISTENCY TX BENCHMARK WITH MEMCHECK -----"
LD_LIBRARY_PATH=../nondebug valgrind --log-file="/results/tx_memcheck" --tool=memcheck ./pmembench pmembench_tx_spp.cfg
echo "----- CRASH CONSISTENCY TX BENCHMARK WITH PMEMCHECK -----"
LD_LIBRARY_PATH=../nondebug valgrind --log-file="/results/tx_pmemcheck" --tool=pmemcheck ./pmembench pmembench_tx_spp.cfg