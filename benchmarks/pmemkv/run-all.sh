#!/bin/bash
# Specify the parameter before running the actual benchmark
# The values be used in all subsequental benchmarking scripts
PMEMKVBENCH=/pmemkv-bench
PMEMKVBENCH_SCRIPTS=/pmemkv_scripts
# Use this line to run the benchmarks on DRAM. Do not forget to also modify run-variants.sh
#MOUNT_PM=/dev/shm
MOUNT_PM=/mnt/pmem0
RESULT_PATH=/results

printSection (){
    echo -e "###### $1 ######"
}

rm -rf $MOUNT_PM/pmemkv

# No arguments means execute all benchmarks
if [ $# -eq 0 ]; then
    declare -a operations=( "read" "randomreadwrite" "delete" ) # "fill" "numops" )
    for op in "${operations[@]}"; do
        cd ${PMEMKVBENCH_SCRIPTS}
        printSection $op
        (. ./run-pmemkv-bench-${op}.sh)
    done
else
    for op in "$@"; do
        case "$op" in
            "fill"|"read"|"randomreadwrite"|"delete"|"numops"|"test") printSection $op; (. ./run-pmemkv-bench-${op}.sh);;
            *)
                echo "Unknown option: ${op}"
                echo "Available options are: fill, read, randomreadwrite, delete, numops"
                ;;
        esac
    done
fi

exit 0
