#!/bin/bash
set -e

cd /pmdk_safepm/src/benchmarks

# copy to have the same config files
cp /spp-pass/pmdk/src/benchmarks/pmembench_map.cfg ./
cp /spp-pass/pmdk/src/benchmarks/pmembench_tx_spp.cfg ./pmembench_tx_safepm.cfg

sed -i 's|/dev/shm/testfile.map|/mnt/pmembench/benchfile.map|' pmembench_map.cfg
sed -i 's|/dev/shm/testfile.tx|/mnt/pmembench/benchfile.tx|' pmembench_tx_safepm.cfg

if ! [ -z "$NUMA_CPU_CORES" ]
then
    CPU_PIN="taskset -c $NUMA_CPU_CORES"
fi

echo "----- SafePM MAP -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug $CPU_PIN ./pmembench pmembench_map.cfg | tee /results/pmembench_map/safepm_o1.txt
echo "----- SafePM TX -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug $CPU_PIN ./pmembench pmembench_tx_safepm.cfg | tee /results/pmembench_tx/safepm_o1.txt
