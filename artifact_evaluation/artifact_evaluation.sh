#!/bin/bash

THIS_DIR=$(dirname "$(readlink -f "$0")")
PROJECT_ROOT=$THIS_DIR/../

if ! [ -d "$BENCHMARK_PM_PATH" ]
then
  echo "Please set BENCHMARK_PM_PATH to a valid directory." 2>&1
  exit 1
fi

if [[ $(lscpu | grep "NUMA node(s)" | awk '{print $3}') > 1 ]]
then
  if [ -z "$NUMA_NODE" ]
  then
    echo "The current system has multiple NUMA nodes." 2>&1
    echo "Please set NUMA_NODE to the node where the mounted PM in $BENCHMARK_PM_PATH is attached." 2>&1
    echo "e.g. NUMA_NODE=0"
    exit 1
  fi
  NUMA_CPU_CORES="$(lscpu | grep "NUMA node$NUMA_NODE" | awk '{print $4}')"
  echo "CPU Cores to pin the benchmarks: $NUMA_CPU_CORES"
fi

# Image creation
cd $PROJECT_ROOT/utils/docker/compiler_images
./create_clang_12_image.sh
./create_spp_image.sh
cd $PROJECT_ROOT/utils/docker/packaged_environments
./create_env_pmdk.sh
./create_env_safepm.sh
./create_env_spp.sh

# pmembench
## execution
cd $PROJECT_ROOT/benchmarks/pmembench
NUMA_NODE=0 BENCHMARK_PM_PATH=/mnt/pmem0/ ./run_variants.sh
## visualize results
cd $PROJECT_ROOT/plot_utils
./import_pmembench.sh $PROJECT_ROOT/benchmarks/pmembench/results
python3 generate_plots.py plot_config_pmembench_tx.yml
python3 generate_plots.py plot_config_maps.yml
cp $PROJECT_ROOT/plot_utils/plots/pmembench_map/pmembench_map_plot.png $THIS_DIR/figure_4-pmembench_indices.png
cp $PROJECT_ROOT/plot_utils/plots/pmembench_map/pmembench_map_plot.pdf $THIS_DIR/figure_4-pmembench_indices.pdf
cp $PROJECT_ROOT/plot_utils/plots/pmembench_tx/pmembench_tx_plot_single_vertical.png $THIS_DIR/figure_7-pmembench_PM_ops.png
cp $PROJECT_ROOT/plot_utils/plots/pmembench_tx/pmembench_tx_plot_single_vertical.pdf $THIS_DIR/figure_7-pmembench_PM_ops.pdf

# pmemkv
## execution
cd $PROJECT_ROOT/benchmarks/pmemkv
NUMA_NODE=0 BENCHMARK_PM_PATH=/mnt/pmem0 ./run-variants.sh
## visualize results
cd $PROJECT_ROOT/plot_utils
./import_pmemkv.sh $PROJECT_ROOT/benchmarks/pmemkv/results
python3 generate_plots.py plot_config_pmemkv.yml
cp $PROJECT_ROOT/plot_utils/plots/pmemkv/pmemkv_plot_overhead.png $THIS_DIR/figure_5-pmemkv.png
cp $PROJECT_ROOT/plot_utils/plots/pmemkv/pmemkv_plot_overhead.pdf $THIS_DIR/figure_5-pmemkv.pdf

# phoenix
## execution
cd $PROJECT_ROOT/benchmarks/phoenix
NUMA_NODE=0 BENCHMARK_PM_PATH=/mnt/pmem0 ./run_variants.sh
## visualize results
cd $PROJECT_ROOT/benchmarks/phoenix
python3 plot.py $PROJECT_ROOT/benchmarks/phoenix/results/phoenix_8_threads
cp $PROJECT_ROOT/benchmarks/phoenix/results/phoenix_8_threads/phoenix_bar_plot_overhead.png $THIS_DIR/figure_6-phoenix.png
cp $PROJECT_ROOT/benchmarks/phoenix/results/phoenix_8_threads/phoenix_bar_plot_overhead.pdf $THIS_DIR/figure_6-phoenix.pdf

# recovery time
## execution
cd $PROJECT_ROOT/benchmarks/recovery_time
BENCHMARK_PM_PATH=/mnt/pmem0 ./run-variants.sh
## visualize results
cd $PROJECT_ROOT/benchmarks/recovery_time
./recovery_table.sh > $THIS_DIR/table_2-recovery_time.txt

# space overhead
## execution
cd $PROJECT_ROOT/benchmarks/space_overhead
BENCHMARK_PM_PATH=/mnt/pmem0 ./run-variants.sh
## visualize results
cd $PROJECT_ROOT/benchmarks/space_overhead
./space_overhead_results.sh > $THIS_DIR/table_3-space_overhead.txt

# crash consistency
## execution
cd $PROJECT_ROOT/benchmarks/crash_consistency
BENCHMARK_PM_PATH=/mnt/pmem0 ./run-variants.sh
## visualize results
cd $PROJECT_ROOT/benchmarks/crash_consistency
./crash-consistency-res.sh > $THIS_DIR/crash_consistency_results.txt

# ripe
## execution
cd $PROJECT_ROOT/benchmarks/ripe
BENCHMARK_PM_PATH=/mnt/pmem0 ./run-variants.sh
## visualize results
cd $PROJECT_ROOT/benchmarks/ripe
./ripe_table.sh > $THIS_DIR/table_4-ripe.txt

# bugs
## execution
cd $PROJECT_ROOT/benchmarks/bugs
BENCHMARK_PM_PATH=/mnt/pmem0 ./spp_bugs.sh
