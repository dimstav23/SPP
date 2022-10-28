#!/bin/bash
NUMBER_OF_ENTRIES=1000000
NUMBER_OF_READS=10000000
DB_SIZE_GB=10

REPEATS=1

RESULT_PATH=$RESULT_PATH/fill
mkdir -p $RESULT_PATH

# THIS BENCHMARK AIMS TO SHOW THE DIFFERENCE BETWEEN THE SEQUENTIAL AND RANDOM FILL
# IT TESTS THE BEHAVIOR OF DIFFERENT KEY SIZES, VALUE SIZES, AND THREADS

if ! [ -z "$NUMA_CPU_CORES" ]
then
    CPU_PIN="taskset -c $NUMA_CPU_CORES"
fi

# Execute the benchmarks
##### Different Keysizes #####
declare -a benchmarks=(fillrandom fillseq)
for bench in "${benchmarks[@]}"; do
    declare -a files=( )
    # Run multiple times the same test
    for i in $(seq 1 $REPEATS); do
        echo  "##### RUN ${i} of benchmark ${bench} keysize #####"
        create_file=true
        # Execute the actual benchmark operation for different values
        # Delete after each run with one value
        for (( keysize=64; keysize<=256; keysize=keysize*2 )); do
            echo "Running $bench for keysize $keysize"
            if [ $create_file = true ]; then
                (cd $PMEMKVBENCH && PMEM_IS_PMEM_FORCE=1 $CPU_PIN ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --db=$MOUNT_PM/pmemkv --key_size=$keysize --value_size=1024 --db_size_in_gb=$DB_SIZE_GB --benchmarks=$bench) > $RESULT_PATH/${bench}_keysize_$i.csv
                create_file=false
            else
                (cd $PMEMKVBENCH && PMEM_IS_PMEM_FORCE=1 $CPU_PIN ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --db=$MOUNT_PM/pmemkv --key_size=$keysize --value_size=1024 --db_size_in_gb=$DB_SIZE_GB --benchmarks=$bench) | tail -n +2 >>  $RESULT_PATH/${bench}_keysize_$i.csv
            fi
            pmempool rm $MOUNT_PM/pmemkv
        done
        files+=( $RESULT_PATH/${bench}_keysize_$i.csv )
    done
    sleep 2
done


##### Different Valuesizes #####
declare -a benchmarks=(fillrandom fillseq)
for bench in "${benchmarks[@]}"; do
    declare -a files=( )
    # Run multiple times the same test
    for i in $(seq 1 $REPEATS); do
        echo  "##### RUN ${i} of benchmark ${bench} valuesize #####"
        create_file=true
        # Execute the actual benchmark operation for different values
        # Delete after each run with one value
        for (( valuesize=1024; valuesize<=4096; valuesize=valuesize*2 )); do
            echo "Running $bench for valuesize $valuesize"
            if [ $create_file = true ]; then
                (cd $PMEMKVBENCH && PMEM_IS_PMEM_FORCE=1 $CPU_PIN ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --db=$MOUNT_PM/pmemkv --key_size=16 --value_size=$valuesize --db_size_in_gb=$DB_SIZE_GB --benchmarks=$bench) >  $RESULT_PATH/${bench}_valuesize_$i.csv
                create_file=false
            else
                (cd $PMEMKVBENCH && PMEM_IS_PMEM_FORCE=1 $CPU_PIN ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --db=$MOUNT_PM/pmemkv --key_size=16 --value_size=$valuesize --db_size_in_gb=$DB_SIZE_GB --benchmarks=$bench) | tail -n +2 >>  $RESULT_PATH/${bench}_valuesize_$i.csv
            fi
            pmempool rm $MOUNT_PM/pmemkv
        done
        files+=( $RESULT_PATH/${bench}_valuesize_$i.csv )
    done
    sleep 2
done


##### Different Threads #####
declare -a benchmarks=(fillrandom fillseq)
for bench in "${benchmarks[@]}"; do
    declare -a files=( )
    # Run multiple times the same test
    for i in $(seq 1 $REPEATS); do
        echo  "##### RUN ${i} of benchmark ${bench} threads #####"
        create_file=true
        # Execute the actual benchmark operation for different values
        # Delete after each run with one value
        for (( threads=1; threads<=4; threads=threads*2 )); do
            echo "Running $bench for threads $threads"
            if [ $create_file = true ]; then
                echo "Threads: ${threads}" > $RESULT_PATH/${bench}_threads_$i.csv
                (cd $PMEMKVBENCH && PMEM_IS_PMEM_FORCE=1 $CPU_PIN ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --db=$MOUNT_PM/pmemkv --key_size=16 --value_size=1024 --threads=$threads --db_size_in_gb=$DB_SIZE_GB --benchmarks=$bench) >>  $RESULT_PATH/${bench}_threads_$i.csv
                create_file=false
            else
                echo "Threads: ${threads}" >> $RESULT_PATH/${bench}_threads_$i.csv
                (cd $PMEMKVBENCH && PMEM_IS_PMEM_FORCE=1 $CPU_PIN ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --db=$MOUNT_PM/pmemkv --key_size=16 --value_size=1024 --threads=$threads --db_size_in_gb=$DB_SIZE_GB --benchmarks=$bench) >>  $RESULT_PATH/${bench}_threads_$i.csv
            fi
            pmempool rm $MOUNT_PM/pmemkv
        done
        files+=( $RESULT_PATH/${bench}_threads_$i.csv )
    done
    sleep 2
done
