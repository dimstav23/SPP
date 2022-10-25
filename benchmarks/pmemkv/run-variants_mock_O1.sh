set -e

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

declare -a variants=("spp_mock_o1")

for variant in "${variants[@]}"
do
  if [ "${variant}" == "safepm_o1" ]; then
    PMDK_PATH=/pmdk_safepm/src/nondebug
  else
    PMDK_PATH=/spp-pass/pmdk/src/nondebug
  fi
  echo -e "###### Starting pmemkv for ${variant} ######"
  mkdir -p "results/${variant}"
  docker run -v "$BENCHMARK_PM_PATH:/mnt/pmem0/" -v "$(pwd)/results/${variant}:/results" -v "$(pwd):/pmemkv_scripts" -e NUMA_CPU_CORES=$NUMA_CPU_CORES -t --shm-size=2g ${variant} ./pmemkv_scripts/run-all.sh $PMDK_PATH
done