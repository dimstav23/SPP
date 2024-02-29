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

# Number of executions for each benchmark
REPEATS=20
# Run for various number of threads
for THREADS in 2 4 8
do
  mkdir -p results/phoenix_"$THREADS"_threads
  docker run -v "$BENCHMARK_PM_PATH:/mnt/phoenix" -v "$(pwd)/results":/results -v "$(pwd)/run_phoenix_spp.sh":/run_phoenix_spp.sh -e NUMA_CPU_CORES=$NUMA_CPU_CORES -e REPEATS=$REPEATS -e THREADS=$THREADS -t spp bash /run_phoenix_spp.sh
  docker run -v "$BENCHMARK_PM_PATH:/mnt/phoenix" -v "$(pwd)/results":/results -v "$(pwd)/run_phoenix_vanilla.sh":/run_phoenix_vanilla.sh -e NUMA_CPU_CORES=$NUMA_CPU_CORES -e REPEATS=$REPEATS -e THREADS=$THREADS -t vanilla_pmdk bash /run_phoenix_vanilla.sh
  docker run -v "$BENCHMARK_PM_PATH:/mnt/phoenix" -v "$(pwd)/results":/results -v "$(pwd)/run_phoenix_safepm.sh":/run_phoenix_safepm.sh -e NUMA_CPU_CORES=$NUMA_CPU_CORES -e REPEATS=$REPEATS -e THREADS=$THREADS -t safepm bash /run_phoenix_safepm.sh
  # Get the average per variant and per experiment among the executed runs
  bash avg.sh results/phoenix_"$THREADS"_threads/
done
