set -e

if ! [ -d "$BENCHMARK_PM_PATH" ]
then
  echo "Please set BENCHMARK_PM_PATH to a valid directory." 2>&1
  exit 1
fi

# declare -a variants=( intact asan_system_heap asan_pmemobj_heap safepm memcheck )
declare -a variants=( intact spp )

for variant in "${variants[@]}"
do
  docker build -t ripe64-$variant -f=Dockerfile_$variant .
done

mkdir -p /spp-pass/benchmarks/ripe/results

for variant in "${variants[@]}"
do
  echo
  echo
  echo "--------------------------"
  echo "|Running variant $variant|"
  echo "--------------------------"
  echo
  echo

  if [ "$variant" = "memcheck" ]
  then
    # We trick valgrind into not producing (large) core dumps by creating a read-only directory and running the tests in that. But for that to work, we must run as a non-root user in the container
    docker run -v "$BENCHMARK_PM_PATH:/mnt/ripe" ripe64-$variant rm -f /mnt/ripe/vmem_dummy.pool # The non-root user in the container cannot overwrite the pool created by the root user during the previous runs
    docker run -v "$BENCHMARK_PM_PATH:/mnt/ripe" -v "$(pwd)/results:/results" -t --shm-size=2g --user safepm_user ripe64-$variant ./ripe_tester.py both 3 gcc -o /results/$variant
  else
	  docker run -v "$BENCHMARK_PM_PATH:/mnt/ripe" -v "$(pwd)/results:/spp-pass/benchmarks/ripe/results" -t --shm-size=2g ripe64-$variant ./ripe_tester.py both 1 clang -o /spp-pass/benchmarks/ripe/results/$variant
    #docker run -v "$BENCHMARK_PM_PATH:/mnt/ripe" -t --shm-size=2g -it ripe64-$variant bash
  fi
done
