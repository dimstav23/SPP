#!/bin/bash
NUMBER_OF_ENTRIES=1000000
NUMBER_OF_READS=10000000
DB_SIZE_GB=10

REPEATS=1

RESULT_PATH=$RESULT_PATH/delete
mkdir -p $RESULT_PATH

# THIS BENCHMARK AIMS TO SHOW THE DIFFERENCE BETWEEN THE SEQUENTIAL AND RANDOM DELETE
# IT TESTS THE BEHAVIOR OF DIFFERENT KEY SIZES, VALUE SIZES, AND THREADS

if ! [ -z "$NUMA_CPU_CORES" ]
then
    CPU_PIN="taskset -c $NUMA_CPU_CORES"
fi

# Execute the benchmarks
##### Different Threads #####
declare -a benchmarks=(deleteseq deleterandom)
declare -a num_threads=(1 2 4 8 16 32)
for bench in "${benchmarks[@]}"; do
    declare -a files=( )
    # Run multiple times the same test
    for i in $(seq 1 $REPEATS); do
        echo  "##### RUN ${i} of benchmark ${bench} threads #####"
        create_file=true
        # Execute the actual benchmark operation for different values
        # Delete after each run with one value
        for threads in "${num_threads[@]}"; do
            echo "Running $bench for threads $threads"
            if [ $create_file = true ]; then
                echo "Threads: ${threads}" > $RESULT_PATH/${bench}_threads_$i.csv
                (cd $PMEMKVBENCH && LD_LIBRARY_PATH=$PMDK_PATH $CPU_PIN ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --reads=$NUMBER_OF_READS --db=$MOUNT_PM/pmemkv --key_size=16 --value_size=1024 --threads=$threads --db_size_in_gb=$DB_SIZE_GB --benchmarks=fillseq,$bench) >> $RESULT_PATH/${bench}_threads_$i.csv
                create_file=false
            else
                echo "Threads: ${threads}" >> $RESULT_PATH/${bench}_threads_$i.csv
                (cd $PMEMKVBENCH && LD_LIBRARY_PATH=$PMDK_PATH $CPU_PIN ./pmemkv_bench --num=$NUMBER_OF_ENTRIES --reads=$NUMBER_OF_READS --db=$MOUNT_PM/pmemkv --key_size=16 --value_size=1024 --threads=$threads --db_size_in_gb=$DB_SIZE_GB --benchmarks=fillseq,$bench) >> $RESULT_PATH/${bench}_threads_$i.csv
            fi
            rm -rf $MOUNT_PM/pmemkv
        done
        files+=( $RESULT_PATH/${bench}_threads_$i.csv )
    done
    sleep 2
done
