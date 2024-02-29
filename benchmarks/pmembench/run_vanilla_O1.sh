#!/bin/bash
set -e

cd /SPP/pmdk/src/benchmarks

sed -i 's|/dev/shm/testfile.map|/mnt/pmembench/benchfile.map|' pmembench_map.cfg
sed -i 's|/dev/shm/testfile.tx|/mnt/pmembench/benchfile.tx|' pmembench_tx_spp.cfg

if ! [ -z "$NUMA_CPU_CORES" ]
then
    CPU_PIN="taskset -c $NUMA_CPU_CORES"
fi

echo "----- PMDK MAP -----"
LD_LIBRARY_PATH=../nondebug $CPU_PIN ./pmembench pmembench_map.cfg | tee /results/pmembench_map/vanilla_o1.txt
echo "----- PMDK TX -----"
LD_LIBRARY_PATH=../nondebug $CPU_PIN ./pmembench pmembench_tx_spp.cfg | tee /results/pmembench_tx/vanilla_o1.txt
