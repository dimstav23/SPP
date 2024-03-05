# The pmembench benchmark (Duration ~45-60mins)

## Workflow description

We run the vanilla `PMDK`, `SPP` and `SafePM` variants of the phoenix benchmarks using the [already built images](../../utils/docker/packaged_environments/).

The [`run_variants.sh`](./run_variants.sh) script executes the `pmembench` benchmark for each variant and places the results in the created `results` directory and further subdirectories with names that showcase the benchmark name. The respective files inside the subdirectories define the variant.
Note that the `repeats` parameter here is taken from the benchmark configuration; an example can be found [here](https://github.com/dimstav23/SPP-pmdk/blob/f053a52637101e27e27632a94ae36f5a49f7af70/src/benchmarks/pmembench_tx_spp.cfg#L19). 
By default, the amount of repeats is set to 3 for the PM indices ([`pmembench_map.cfg`](https://github.com/dimstav23/SPP-pmdk/blob/f053a52637101e27e27632a94ae36f5a49f7af70/src/benchmarks/pmembench_map.cfg#L11)) and 10 for the PM TX ([`pmembench_tx.cfg`](https://github.com/dimstav23/SPP-pmdk/blob/f053a52637101e27e27632a94ae36f5a49f7af70/src/benchmarks/pmembench_tx_spp.cfg#L19)) operations.

**Note**: If the machine you are using has multiple *NUMA nodes*, it is **mandatory** that you specify the node that the mounted PM is attached. This can be done via the `NUMA_NODE` environment variable. Otherwise, you can ignore this variable.

To run the benchmark, use:
```
$ cd PROJECT_ROOT/benchmarks/phoenix
$ NUMA_NODE=0 BENCHMARK_PM_PATH=<path to PM> ./run_variants.sh
```

For more information about the images and containers, see [here](../..//utils/docker/README.md).

## Plots
To generate bar plots based on the results, simply run:
```
$ cd PROJECT_ROOT/plot_utils
$ ./import_pmembench.sh <path-to-pmembench-results>
$ python3 generate_plots.py plot_config_pmembench_tx.yml
$ python3 generate_plots.py plot_config_maps.yml
```
The first [`import_pmembench.sh`](../../plot_utils/import_pmembench.sh) script mainly copies the results of the original folder and formats it so that they can be used by the plotting scripts.
The [`generate_plots`](../../plot_utils/generate_plots.py) script will generate a bunch of plots for the `pmembench` results which are placed in `PROJECT_ROOT/plot_utils/plots` directory.
For each benchmark, a new subdirectory is created.
The executions above will also print a summary of the analyzed result in the `stdout`. 

For more information about the plotting scripts, see [here](../../plot_utils/README.md).

---

## File description

- [`run_safepm.sh`](./run_safepm.sh): Script that runs inside the newly-built container for SafePM and modifies the appropriate `pmembench` benchmark files to set the correct paths for the PM pools and executes the `pmembench` benchmark.
- [`run_spp.sh](./run_spp.sh): Script that runs inside the newly-built container for SPP and modifies the appropriate `pmembench` benchmark files to set the correct paths for the PM pools and executes the `pmembench` benchmark.
- [`run_vanilla.sh`](./run_vanilla.sh): Script that runs inside the newly-built container for vanilla PMDK and modifies the appropriate `pmembench` benchmark files to set the correct paths for the PM pools and executes the `pmembench` benchmark.
- [`run_variants.sh`](./run_variants.sh): Script that instruments the execution of the `pmembench` benchmark for all the variants and configurations.

**Note**: We further provide scripts to run `pmembench` in additional environments:
- `*_O1.sh`: These scripts (with the suffix `_O1`) run `pmembench` in the same environment but set the optimization level to `O1` using the respective `O1` images, produced by the scripts [here](../../utils/docker/packaged_environments/).
- `*_mock_*.sh`: These scripts run `pmembench` in the SPP environment where we rule out the SPP pointer instrumentation by forcing the SPP injected function to immediately return without performing any action. They use the respective `mock` images, produced by the scripts [here](../../utils/docker/packaged_environments/).
