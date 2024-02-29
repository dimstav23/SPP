#!/bin/bash
set -e

cd /SPP/pmdk/src/benchmarks

sed -i 's|/dev/shm/testfile.map|/mnt/pmembench/benchfile.map|' pmembench_map.cfg
sed -i 's|/dev/shm/testfile.tx|/mnt/pmembench/benchfile.tx|' pmembench_tx_spp.cfg

cp /pmembench_perf.cpp ./pmembench.cpp
make

if ! [ -z "$NUMA_CPU_CORES" ]
then
    CPU_PIN="taskset -c $NUMA_CPU_CORES"
fi

echo "----- SPP O1 MAP -----"
LD_LIBRARY_PATH=../nondebug $CPU_PIN ./pmembench pmembench_map.cfg | tee /results/pmembench_map/spp_o1.txt
echo "----- SPP O1 TX -----"
LD_LIBRARY_PATH=../nondebug $CPU_PIN ./pmembench pmembench_tx_spp.cfg | tee /results/pmembench_tx/spp_o1.txt
