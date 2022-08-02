#!/bin/bash

if ! [ -d "$BENCHMARK_PM_PATH" ]
then
  echo "Please set BENCHMARK_PM_PATH to a valid directory." 2>&1
  exit 1
fi

mkdir -p results
rm -rf ./results/*

docker build -t spp-space_overhead .
docker run -v "$BENCHMARK_PM_PATH:/mnt/space_overhead" -v "$(pwd)/results":/results -t spp-space_overhead bash inner_run.sh