#!/bin/bash

if [ "$#" -ne 1 ]; then
    if [[ -z "${BENCHMARK_PM_PATH}" ]]; then
        echo "Usage: spp_bugs.sh <path to pm>" >&2
        exit 1
    fi
else
    export BENCHMARK_PM_PATH="$1"
fi

docker run -v "$BENCHMARK_PM_PATH:/mnt/pmembench" -v "$(pwd)/bug1_btree_dbg.sh":/bug1_btree_dbg.sh \
            -v "$(pwd)/change_pmembench_bugs_file_path.sh":/change_pmembench_bugs_file_path.sh \
            -t spp bash bug1_btree_dbg.sh

docker run -v "$BENCHMARK_PM_PATH:/mnt/pmembench" -v "$(pwd)/bug1_btree_rel.sh":/bug1_btree_rel.sh \
            -v "$(pwd)/change_pmembench_bugs_file_path.sh":/change_pmembench_bugs_file_path.sh \
            -t spp bash bug1_btree_rel.sh

docker run -v "$BENCHMARK_PM_PATH:/mnt/phoenix" -v "$(pwd)/bug2_phoenix.sh":/bug2_phoenix.sh \
            -t spp bash bug2_phoenix.sh

docker run -v "$BENCHMARK_PM_PATH:/mnt/array" -v "$(pwd)/bug3_pmdk_array.sh":/bug3_pmdk_array.sh \
            -t spp bash bug3_pmdk_array.sh