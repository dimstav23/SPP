#!/bin/bash
set -e

REPEATS=100
declare -a pmemoids_nums=("100" "1000" "10000" "100000" "1000000")

cd /
make clean
make INTACT=1
cd /build
echo "----- VANILLA RECOVERY -----"
rm -f /results/vanilla
echo "number_of_objects;log_size(KB);pool_size(MB);recovery_time(s)" >> /results/vanilla
for pmemoids_num in "${pmemoids_nums[@]}"
do
    for i in $(seq 1 $REPEATS)
    do
        LD_LIBRARY_PATH=/pmdk/src/nondebug ./recovery_run $pmemoids_num >> /results/vanilla
    done
done

cd /
make clean
make
cd /build
echo "----- SPP RECOVERY -----"
rm -f /results/spp
echo "number_of_objects;log_size(KB);pool_size(MB);recovery_time(s)" >> /results/spp
for pmemoids_num in "${pmemoids_nums[@]}"
do
    for i in $(seq 1 $REPEATS)
    do
        LD_LIBRARY_PATH=/spp-pass/pmdk/src/nondebug ./recovery_run $pmemoids_num >> /results/spp
    done
done

