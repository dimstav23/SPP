#!/bin/bash

set -e

if ! [ -z "$NUMA_CPU_CORES" ]
then
  CPU_PIN="taskset -c $NUMA_CPU_CORES"
fi

### Change the PM path in the source for the mapped PM in the container
cd /phoenix/PM_phoenix-2.0
sed -i 's|/mnt/pmem0/dimitrios/|/mnt/pmembench/|' tests/histogram/histogram.c
sed -i 's|/mnt/pmem0/dimitrios/|/mnt/pmembench/|' tests/kmeans/kmeans.c
sed -i 's|/mnt/pmem0/dimitrios/|/mnt/pmembench/|' tests/linear_regression/linear_regression.c
sed -i 's|/mnt/pmem0/dimitrios/|/mnt/pmembench/|' tests/matrix_multiply/matrix_multiply.c
sed -i 's|/mnt/pmem0/dimitrios/|/mnt/pmembench/|' tests/pca/pca.c
sed -i 's|/mnt/pmem0/dimitrios/|/mnt/pmembench/|' tests/string_match/string_match.c
sed -i 's|/mnt/pmem0/dimitrios/|/mnt/pmembench/|' tests/word_count/word_count.c

### Compile the phoenix benchmark
cd /phoenix/PM_phoenix-2.0
make clean
make SPP_OFF=1

### Set the env variable that defines the amount of threads to use
export MR_NUMTHREADS=4

### histogram ###
echo "----- Vanilla Histogram -----"
cd /phoenix/PM_phoenix-2.0/tests/histogram
if [ ! -d "histogram_datafiles" ]; then
  wget http://csl.stanford.edu/\~christos/data/histogram.tar.gz 
  tar -xvf histogram.tar.gz
fi
LD_LIBRARY_PATH=/spp-pass/pmdk/src/nondebug $CPU_PIN ./histogram histogram_datafiles/large.bmp | tee /results/phoenix/vanilla_histogram.txt

### kmeans ###
echo "----- Vanilla kmeans -----"
cd /phoenix/PM_phoenix-2.0/tests/kmeans
LD_LIBRARY_PATH=/spp-pass/pmdk/src/nondebug $CPU_PIN ./kmeans | tee /results/phoenix/vanilla_kmeans.txt

### linear_regression ###
echo "----- Vanilla linear_regression -----"
cd /phoenix/PM_phoenix-2.0/tests/linear_regression
if [ ! -d "linear_regression_datafiles" ]; then
  wget http://csl.stanford.edu/\~christos/data/linear_regression.tar.gz
  tar -xvf linear_regression.tar.gz
fi
LD_LIBRARY_PATH=/spp-pass/pmdk/src/nondebug $CPU_PIN ./linear_regression linear_regression_datafiles/key_file_500MB.txt | tee /results/phoenix/vanilla_lr.txt

### matrix_multiply ###
echo "----- Vanilla matrix_multiply -----"
cd /phoenix/PM_phoenix-2.0/tests/matrix_multiply
LD_LIBRARY_PATH=/spp-pass/pmdk/src/nondebug $CPU_PIN ./matrix_multiply 100 100 | tee /results/phoenix/vanilla_mm.txt

### pca ###
echo "----- Vanilla pca -----"
cd /phoenix/PM_phoenix-2.0/tests/pca
LD_LIBRARY_PATH=/spp-pass/pmdk/src/nondebug $CPU_PIN ./pca | tee /results/phoenix/vanilla_pca.txt

### string_match ###
echo "----- Vanilla string_match -----"
cd /phoenix/PM_phoenix-2.0/tests/string_match
if [ ! -d "string_match_datafiles" ]; then
  wget http://csl.stanford.edu/\~christos/data/string_match.tar.gz
  tar -xvf string_match.tar.gz
fi
LD_LIBRARY_PATH=/spp-pass/pmdk/src/nondebug $CPU_PIN ./string_match string_match_datafiles/key_file_500MB.txt | tee /results/phoenix/vanilla_sm.txt

### word_count ###
echo "----- Vanilla word_count -----"
cd /phoenix/PM_phoenix-2.0/tests/word_count
if [ ! -d "word_count_datafiles" ]; then
  wget http://csl.stanford.edu/\~christos/data/word_count.tar.gz
  tar -xvf word_count.tar.gz
fi
LD_LIBRARY_PATH=/spp-pass/pmdk/src/nondebug $CPU_PIN ./word_count word_count_datafiles/word_100MB.txt | tee /results/phoenix/vanilla_wc.txt
