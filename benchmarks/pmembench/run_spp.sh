#!/bin/bash
set -e

cd /spp-pass/pmdk/src/benchmarks

sed -i 's|/dev/shm/testfile.map|/mnt/pmembench/benchfile.map|' pmembench_map.cfg
sed -i 's|/dev/shm/testfile.tx|/mnt/pmembench/benchfile.tx|' pmembench_tx_spp.cfg

echo "----- SPP MAP -----"
LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/pmembench_map/spp.txt
echo "----- SPP TX -----"
LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx_spp.cfg | tee /results/pmembench_tx/spp.txt
