set -e

declare -a indices=( ctree rtree rbtree hashmap_tx )
declare -a ops=( insert get )

construct_config_file () {
  echo "[global]
    group=pmemobj
    file=/mnt/space_overhead/benchfile.map
    ops-per-thread=1000000
    threads=1
    type=$1
    data-size=1024

    [map_$2]
    bench=map_$2
    repeats=1
    alloc=true
    " > $3
}

cd /pmdk/src/benchmarks
echo "----- MAP BENCHMARK FOR PMDK -----"
for index in "${indices[@]}"
do
    for op in "${ops[@]}"
    do
        echo "PMDK $index $op"
        construct_config_file $index $op /pmdk/src/benchmarks/pmembench_map.cfg
        LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg > /dev/null
        LD_LIBRARY_PATH=../nondebug/ /pmdk/src/tools/pmempool/pmempool info  -s /mnt/space_overhead/benchfile.map | tail -n 2 >> /results/pmdk_${index}_${op}.txt
        
    done
done


cd /spp-pass/pmdk/src/benchmarks
echo "----- MAP BENCHMARK FOR SPP -----"
for index in "${indices[@]}"
do
    for op in "${ops[@]}"
    do
        echo "SPP $index $op"
        construct_config_file $index $op /spp-pass/pmdk/src/benchmarks/pmembench_map.cfg
        LD_LIBRARY_PATH=../nondebug ./pmembench pmembench_map.cfg > /dev/null
        LD_LIBRARY_PATH=../nondebug/ /spp-pass/pmdk/src/tools/pmempool/pmempool info  -s /mnt/space_overhead/benchfile.map | tail -n 2 >> /results/spp_${index}_${op}.txt
    done
done