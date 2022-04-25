#!/bin/bash

if ! [ -d "$BENCHMARK_PM_PATH" ]
then
  echo "Please set BENCHMARK_PM_PATH to a valid directory." 2>&1
  exit 1
fi

mkdir -p results/pmembench_map
mkdir -p results/pmembench_tx

docker run -v "$BENCHMARK_PM_PATH:/mnt/pmembench" -v "$(pwd)/results":/results -v "$(pwd)/run_spp.sh":/run_spp.sh -t spp bash /run_spp.sh

docker run -v "$BENCHMARK_PM_PATH:/mnt/pmembench" -v "$(pwd)/results":/results -v "$(pwd)/run_vanilla.sh":/run_vanilla.sh -t vanilla_pmdk bash /run_vanilla.sh

docker run -v "$BENCHMARK_PM_PATH:/mnt/pmembench" -v "$(pwd)/results":/results -v "$(pwd)/run_safepm.sh":/run_safepm.sh -t safepm bash /run_safepm.sh
