#!/bin/bash

if ! [ -d "$BENCHMARK_PM_PATH" ]
then
  echo "Please set BENCHMARK_PM_PATH to a valid directory." 2>&1
  exit 1
fi

if [[ $(lscpu | grep "NUMA node(s)" | awk '{print $3}') > 1 ]]
then
  if [ -z "$NUMA_NODE" ]
  then
    echo "The current system has multiple NUMA nodes." 2>&1
    echo "Please set NUMA_NODE to the node where the mounted PM in $BENCHMARK_PM_PATH is attached." 2>&1
    echo "e.g. NUMA_NODE=0"
    exit 1
  fi
  NUMA_CPU_CORES="$(lscpu | grep "NUMA node$NUMA_NODE" | awk '{print $4}')"
  echo "CPU Cores to pin the benchmarks: $NUMA_CPU_CORES"
fi

mkdir -p results/pmembench_map
mkdir -p results/pmembench_tx

docker run -v "$BENCHMARK_PM_PATH:/mnt/pmembench" -v "$(pwd)/results":/results -v "$(pwd)/run_spp.sh":/run_spp.sh -e NUMA_CPU_CORES=$NUMA_CPU_CORES -t spp bash /run_spp.sh

docker run -v "$BENCHMARK_PM_PATH:/mnt/pmembench" -v "$(pwd)/results":/results -v "$(pwd)/run_vanilla.sh":/run_vanilla.sh -e NUMA_CPU_CORES=$NUMA_CPU_CORES -t vanilla_pmdk bash /run_vanilla.sh

docker run -v "$BENCHMARK_PM_PATH:/mnt/pmembench" -v "$(pwd)/results":/results -v "$(pwd)/run_safepm.sh":/run_safepm.sh -e NUMA_CPU_CORES=$NUMA_CPU_CORES -t safepm bash /run_safepm.sh
