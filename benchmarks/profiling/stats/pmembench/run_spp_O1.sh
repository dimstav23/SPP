#!/bin/bash
set -e

cd /spp-pass/runtime/
cd /spp-pass/runtime/
git fetch
git pull
make clean && make SPP_STATS=1

cd /spp-pass/pmdk/src/benchmarks

sed -i 's|/dev/shm/testfile.map|/mnt/pmembench/benchfile.map|' pmembench_map.cfg
sed -i 's|/dev/shm/testfile.tx|/mnt/pmembench/benchfile.tx|' pmembench_tx_spp.cfg

cp /pmembench_stats.cpp ./pmembench.cpp
sed -i 's|INCS += -I$(SPPLIBSRC)/spp.h|PROGFLAGS += -I$(SPPLIBSRC)/|g' Makefile
make clean && make

if ! [ -z "$NUMA_CPU_CORES" ]
then
    CPU_PIN="taskset -c $NUMA_CPU_CORES"
fi

echo "----- SPP O1 MAP -----"
LD_LIBRARY_PATH=../nondebug $CPU_PIN ./pmembench pmembench_map.cfg | tee /results/pmembench_map/spp_o1.txt
echo "----- SPP O1 TX -----"
LD_LIBRARY_PATH=../nondebug $CPU_PIN ./pmembench pmembench_tx_spp.cfg | tee /results/pmembench_tx/spp_o1.txt
