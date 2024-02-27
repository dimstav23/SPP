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
echo "----- SafePM Histogram -----"
cd /phoenix/PM_phoenix-2.0/tests/histogram
if [ ! -d "histogram_datafiles" ]; then
  wget http://csl.stanford.edu/\~christos/data/histogram.tar.gz
  tar -xvf histogram.tar.gz
fi
for ((i=1; i<=$REPEATS; i++)); do
  ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=/pmdk_safepm/src/nondebug $CPU_PIN ./histogram histogram_datafiles/large.bmp 2> /results/phoenix/safepm_histogram_"$i".txt
done

### kmeans ###
echo "----- SafePM kmeans -----"
cd /phoenix/PM_phoenix-2.0/tests/kmeans
for ((i=1; i<=$REPEATS; i++)); do
  ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=/pmdk_safepm/src/nondebug $CPU_PIN ./kmeans 2> /results/phoenix/safepm_kmeans_"$i".txt
done

### linear_regression ###
echo "----- SafePM linear_regression -----"
cd /phoenix/PM_phoenix-2.0/tests/linear_regression
if [ ! -d "linear_regression_datafiles" ]; then
  wget http://csl.stanford.edu/\~christos/data/linear_regression.tar.gz
  tar -xvf linear_regression.tar.gz
fi
for ((i=1; i<=$REPEATS; i++)); do
  ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=/pmdk_safepm/src/nondebug $CPU_PIN ./linear_regression linear_regression_datafiles/key_file_500MB.txt 2> /results/phoenix/safepm_lr_"$i".txt
done

### matrix_multiply ###
echo "----- SafePM matrix_multiply -----"
cd /phoenix/PM_phoenix-2.0/tests/matrix_multiply
for ((i=1; i<=$REPEATS; i++)); do
  ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=/pmdk_safepm/src/nondebug $CPU_PIN ./matrix_multiply 100 100 2> /results/phoenix/safepm_mm_"$i".txt
done

### pca ###
echo "----- SafePM pca -----"
cd /phoenix/PM_phoenix-2.0/tests/pca
for ((i=1; i<=$REPEATS; i++)); do
  ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=/pmdk_safepm/src/nondebug $CPU_PIN ./pca 2> /results/phoenix/safepm_pca_"$i".txt
done

### string_match ###
echo "----- SafePM string_match -----"
cd /phoenix/PM_phoenix-2.0/tests/string_match
if [ ! -d "string_match_datafiles" ]; then
  wget http://csl.stanford.edu/\~christos/data/string_match.tar.gz
  tar -xvf string_match.tar.gz
fi
for ((i=1; i<=$REPEATS; i++)); do
  ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=/pmdk_safepm/src/nondebug $CPU_PIN ./string_match string_match_datafiles/key_file_500MB.txt 2> /results/phoenix/safepm_sm_"$i".txt
done

### word_count ###
echo "----- SafePM word_count -----"
cd /phoenix/PM_phoenix-2.0/tests/word_count
if [ ! -d "word_count_datafiles" ]; then
  wget http://csl.stanford.edu/\~christos/data/word_count.tar.gz
  tar -xvf word_count.tar.gz
fi
for ((i=1; i<=$REPEATS; i++)); do
  ASAN_OPTIONS=detect_leaks=0 LD_LIBRARY_PATH=/pmdk_safepm/src/nondebug $CPU_PIN ./word_count word_count_datafiles/word_100MB.txt 2> /results/phoenix/safepm_wc_"$i".txt
done
