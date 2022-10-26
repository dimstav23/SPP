#!/bin/bash

echo "----------------------- LLVM IR DUMP FOR -O2/O3 & RELEASE CASE -----------------------"
# Optimized - Release case
mkdir -p spp_IRs/pm_indices
mkdir -p spp_IRs/pmembench
mkdir -p spp_IRs/pmemkv
mkdir -p spp_IRs/pmemkv_bench

docker run -v "$(pwd)/spp_IRs":/results -v "$(pwd)/run_spp.sh":/run_spp.sh -t spp bash /run_spp.sh

echo "----------------------- LLVM IR DUMP FOR -O1 & DEBUG CASE -----------------------"
# O1 - Debug case
mkdir -p spp_o1_IRs/pm_indices
mkdir -p spp_o1_IRs/pmembench
mkdir -p spp_o1_IRs/pmemkv
mkdir -p spp_o1_IRs/pmemkv_bench

docker run -v "$(pwd)/spp_o1_IRs":/results -v "$(pwd)/run_spp_O1.sh":/run_spp_O1.sh -t spp_o1 bash /run_spp_O1.sh