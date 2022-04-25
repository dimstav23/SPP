#!/bin/bash
set -e

cd /pmdk_safepm/src/benchmarks

sed -i 's|/dev/shm/testfile.map|/mnt/pmembench/benchfile.map|' pmembench_map.cfg
sed -i 's|/dev/shm/testfile.tx|/mnt/pmembench/benchfile.tx|' pmembench_tx_safepm.cfg

echo "----- SafePM MAP -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg | tee /results/pmembench_map/safepm.txt
echo "----- SafePM TX -----"
ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_tx_safepm.cfg | tee /results/pmembench_tx/safepm.txt
