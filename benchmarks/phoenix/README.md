# The phoenix benchmark (Duration ~90mins)

## Workflow description

We run the vanilla `PMDK`, `SPP` and `SafePM` variants of the phoenix benchmarks using the [already built images](../../utils/docker/packaged_environments/).
The [`run_variants.sh`](./run_variants.sh) script executes each benchmark for each variant `$REPEATS` times (specified in the script) with different thread setups (currently 2, 4, 8) and places the results in the created `results` directory and further subdirectories
with names that showcase the thread setup. 

The name of the result files highlight the variant and the benchmark name.

The [`run_variants.sh`](./run_variants.sh) script also uses the [`avg.sh`](./avg.sh) script that scans a directory and creates for each benchmark and variant one file with the average for the executed `$REPEATS`. 

**Note**: If the machine you are using has multiple *NUMA nodes*, it is **mandatory** that you specify the node that the mounted PM is attached. This can be done via the `NUMA_NODE` environment variable. Otherwise, you can ignore this variable.

To run the benchmark, use:
```
$ cd PROJECT_ROOT/benchmarks/phoenix
$ NUMA_NODE=0 BENCHMARK_PM_PATH=<path to PM> ./run_variants.sh
```

To manually run the `avg.sh`:
```
$ cd PROJECT_ROOT/benchmarks/phoenix
$ ./avg.sh <directory_of_the_results>
```

For more information about the images and containers, see [here](../..//utils/docker/README.md).

## Plots
To generate bar plots based on the results, simply run:
```
$ cd PROJECT_ROOT/benchmarks/phoenix
$ python3 plot.py <directory_of_the_results>
```
This script will generate 2 plots (1 for the absolute time taken for the exeuction and 1 for the relative performance overhead).
It will also print to `stdout` the relative overheads for SPP and SafePM with vanilla PMDK as the baseline.

---

## File description

- [`avg.sh`](./avg.sh): script that scans a directory and creates for each benchmark and variant one file with the average for the executed `$REPEATS`. 
- [`plot.py`](./plot.py): plotting script for the `phoenix` benchmark results.
- [`run_phoenix_safepm.sh`](./run_phoenix_safepm.sh): Script that runs inside the newly-built container for SafePM and modifies the appropriate `phoenix` benchmark files to set the correct paths for the PM pools and the becnhmark parameters, compiles the phoenix benchmark and executes the `phoenix` benchmark for all the repeats specified in [`run_variants.sh`](./run_variants.sh).
- [`run_phoenix_spp.sh`](./run_phoenix_spp.sh): Script that runs inside the newly-built container for SPP and modifies the appropriate `phoenix` benchmark files to set the correct paths for the PM pools and the becnhmark parameters, compiles the phoenix benchmark, the SPP pmdk with the appropriate TAG_BITS, and executes the `phoenix` benchmark for all the repeats specified in [`run_variants.sh`](./run_variants.sh).
- [`run_phoenix_vanilla.sh`](./run_phoenix_vanilla.sh): Script that runs inside the newly-built container for vanilla PMDK and modifies the appropriate `phoenix` benchmark files to set the correct paths for the PM pools and the becnhmark parameters, compiles the phoenix benchmark and executes the `phoenix` benchmark for all the repeats specified in [`run_variants.sh`](./run_variants.sh).
- [`run_variants.sh`](./run_variants.sh): Script that instruments the execution of the `phoenix` benchmark for all the variants and configurations.