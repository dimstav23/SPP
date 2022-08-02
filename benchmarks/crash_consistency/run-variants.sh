#!/bin/bash

if ! [ -d "$BENCHMARK_PM_PATH" ]
then
  echo "Please set BENCHMARK_PM_PATH to a valid directory." 2>&1
  exit 1
fi

mkdir -p results

docker build -t spp-crash_consistency .
docker run -v "$BENCHMARK_PM_PATH:/mnt/crash_consistency" -v "$(pwd)/results":/results -t spp-crash_consistency bash inner_run.sh