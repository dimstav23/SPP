## The phoenix benchmark

To run the benchmark, use `BENCHMARK_PM_PATH=<path to PM> ./run_variants.sh`. 
This will run the vanilla `PMDK`, `SPP` and `SafePM` variants of the phoenix benchmarks in the already built containers.
This `run_variants.sh` script executes each benchmark for each variant `$REPEATS` times (specified in the script) with 
different thread setups (currently 2, 4, 8) and places the results in the created `results` directory and further subdirectories
with names that showcase the thread setup. The name of the result files highlight the variant and the benchmark name.
The `run_variants.sh` script also uses the `avg.sh` script that scans a directory and create for each benchmark and variant
one file with the average for the executed `$REPEATS`. 
To manually run the `avg.sh`:
```
$ ./avg.sh <directory_of_the_results>
```

**Note**: If the machine you are using has multiple *NUMA nodes*, it is mandatory that you specify the node that the mounted PM is attached.
This can be done via the `NUMA_NODE` environment variable.
e.g. `NUMA_NODE=<node_id> BENCHMARK_PM_PATH=<path to PM> ./run_variants.sh`.

For more information about the images and containers, see [here](/utils/docker/README.md).

### Plots
To generate bar plots based on the results, simply run:
```
$ python3 plot.py <directory_of_the_results>
```
This script will generate 2 plots (1 for the absolute time and 1 for the relative performance overhead).