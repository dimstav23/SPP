set -e

if ! [ -d "$BENCHMARK_PM_PATH" ]
then
  echo "Please set BENCHMARK_PM_PATH to a valid directory." 2>&1
  exit 1
fi

# declare -a variants=("vanilla_pmdk" "spp" "safepm")
declare -a variants=("safepm")

for variant in "${variants[@]}"
do
  if [ "${variant}" == "safepm" ]; then
    PMDK_PATH=/pmdk_safepm/src/nondebug
  else
    PMDK_PATH=/spp-pass/pmdk/src/nondebug
  fi
  echo -e "###### Starting pmemkv for ${variant} ######"
  mkdir -p "results/${variant}"
  docker run -v "$BENCHMARK_PM_PATH:/mnt/pmem0/" -v "$(pwd)/results/${variant}:/results" -v "$(pwd):/pmemkv_scripts" -t --shm-size=2g $variant ./pmemkv_scripts/run-all.sh $PMDK_PATH
done