set -e

if ! [ -d "$BENCHMARK_PM_PATH" ]
then
  echo "Please set BENCHMARK_PM_PATH to a valid directory." 2>&1
  exit 1
fi

# declare -a variants=("vanilla_pmdk" "spp" "safepm")
# declare -a variants=("vanilla_pmdk" "spp")
declare -a variants=("spp")

for variant in "${variants[@]}"
do
  echo -e "###### Starting pmemkv for ${variant} ######"
  mkdir -p "results/${variant}"
  docker run -v "$BENCHMARK_PM_PATH:/mnt/pmem0/" -v "$(pwd)/results/${variant}:/results" -v "$(pwd):/pmemkv_scripts" -t --shm-size=2g $variant ./pmemkv_scripts/run-all.sh
done
