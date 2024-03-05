# The pmemkv benchmark (Duration ~240mins)

Adapted from [SafePM](https://github.com/TUM-DSE/safepm/tree/master/benchmarks/pmemkv) and [Robert Jandow's repo](https://github.com/RobertJndw/PMDK-performance-evaluation/tree/main/pmemkv-bench)

## Workflow description

We run the vanilla `PMDK`, `SPP` and `SafePM` variants of the `pmemkv-bench` benchmarks using the [already built images](../../utils/docker/packaged_environments/).

The [`run-variants.sh`](./run-variants.sh) script executes the `pmemkv-bench` benchmark for each variant and places the results in the created `results` directory and further subdirectories with names that showcase the variant. The respective files inside the subdirectories define the benchmark type. 
Note that the `repeats` parameter here is determined in the benchmark scripts that run inside the container; an example can be found [here](https://github.com/dimstav23/SPP/blob/be777291e625b14300a3e97efc51b2f770539204/benchmarks/pmemkv/run-pmemkv-bench-read.sh#L6). By default, the amount of repeats is set to 3.

**Note**: If the machine you are using has multiple *NUMA nodes*, it is **mandatory** that you specify the node that the mounted PM is attached. This can be done via the `NUMA_NODE` environment variable. Otherwise, you can ignore this variable.

To run the benchmark, use:
```
$ cd PROJECT_ROOT/benchmarks/pmemkv
$ NUMA_NODE=0 BENCHMARK_PM_PATH=<path to PM> ./run-variants.sh
```

For more information about the images and containers, see [here](../..//utils/docker/README.md).

## Plots
To generate the plots based on the results, simply run:
```
$ cd PROJECT_ROOT/plot_utils
$ ./import_pmemkv.sh <path-to-pmemkv-results>
$ python3 generate_plots.py plot_config_pmemkv.yml
```
The first [`import_pmemkv.sh`](../../plot_utils/import_pmemkv.sh) script mainly copies the results of the original folder and formats it so that they can be used by the plotting scripts.
The [`generate_plots`](../../plot_utils/generate_plots.py) script will generate a bunch of plots for the `pmemkv` results which are placed in `PROJECT_ROOT/plot_utils/plots` directory.
For each benchmark, a new subdirectory is created.
The executions above will also print a summary of the analyzed result in the `stdout`. 

For more information about the plotting scripts, see [here](../../plot_utils/README.md).

---

## File description

- [`avg.py`](./avg.py): Helper scripts to calculate the average for the executed benchmark repeats. It is embedded in the scripts that run inside the containers and trigger the benchmark execution.
- [`run-all.sh`](./run-all.sh): Scripts that runs inside the container and triggers the run of the benchmarks. It takes as a first argument the path where the PMDK libraries are located. If it is not provided with any additional arguments (specific benchmark), it executes the `read`, `randomreadwrite` and `delete` benchmarks of `pmemkv-bench`.
- [`run-pmemkv-bench-delete.sh`](./run-pmemkv-bench-delete.sh): Defines the parameters for the persistent KV store and runs the `deleteseq` and `deleterandom` benchmarks for the defined repeats and various thread counts and dumps the results in the appropriate `csv` file.
- [`run-pmemkv-bench-fill.sh`](./run-pmemkv-bench-fill.sh): Defines the parameters for the persistent KV store and runs the `fillseq` and `fillrandom` benchmarks for the defined repeats and various thread counts and dumps the results in the appropriate `csv` file.
- [`run-pmemvk-bench-numops.sh`](./run-pmemkv-bench-numops.sh): Defines the parameters for the persistent KV store and runs the `readseq` and `readrandom` benchmarks for the defined repeats and various thread counts and dumps the results in the appropriate `csv` file. (**STALE and UNUSED**)
- [`run-pmemkv-bench-randomreadwrite.sh`](./run-pmemkv-bench-randomreadwrite.sh): Defines the parameters for the persistent KV store and runs the `randomreadwrite` benchmark for the defined repeats, different read percentages and various thread counts and dumps the results in the appropriate `csv` file.
- [`run-pmemkv-bench-read.sh`](./run-pmemkv-bench-read.sh): Defines the parameters for the persistent KV store and runs the `readseq` and `readrandom` benchmarks for the defined repeats and various thread counts and dumps the results in the appropriate `csv` file.
- [`run-variants.sh`](./run-variants.sh): Script that instruments the execution of the `pmemkv` benchmark for all the variants (vanilla PMDK, SPP and SafePM), benchmarks and configurations.

**Note**: We further provide scripts to run `pmemkv` in additional environments:
- [`run_variants_O1.sh`](./run-variants_O1.sh): This script (with the suffix `_O1`) runs `pmemkv` in the same environment but set the optimization level to `O1` using the respective `O1` images, produced by the scripts [here](../../utils/docker/packaged_environments/).
- [`run_variants_mock.sh`](./run-variants_mock.sh) and [`run_variants_mock_O1.sh`](./run-variants_mock_O1.sh): These scripts run `pmemkv` in the SPP environment where we rule out the SPP pointer instrumentation by forcing the SPP injected function to immediately return without performing any action. They use the respective `mock` images, produced by the scripts [here](../../utils/docker/packaged_environments/). The suffix defines the optimization level that is used.
