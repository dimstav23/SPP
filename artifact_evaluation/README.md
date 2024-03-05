# Artifact evaluation

*Note:* All the timings listed below refer to our testbed. They may vary depending on the computing power of the used machine.

## System Set-up
To reproduce the results of SPP, a machine equipped with a physical persistent memory module is required. SPP requires a DAX-enabled file system, such as EXT4-DAX. To format and mount the drive (assuming the device name is `/dev/pmem0`), follow the instructions below:
```
$ sudo mkdir /mnt/pmem0
$ sudo mkfs.ext4 /dev/pmem0
$ sudo mount -t ext4 -o dax /dev/pmem0 /mnt/pmem0
$ sudo chmod -R 777 /mnt/pmem0
 ```
In case you do not have a machine with persistent memory, you can execute the experiments by [emulating](https://pmem.io/blog/2016/02/how-to-emulate-persistent-memory/) it with DRAM, e.g., with the `/dev/shm/` directory. Note that, this results in considerable differences in the final performance measurements.

Further, you need to get the source code of SPP and set up the [native environment](../README.md#automated-project-build).
It takes some minutes depending on the computing power of your machine because it builds LLVM. In our case, it lasted ~20mins. 
To do so, run the following:
```
$ git clone git@github.com:dimstav23/SPP.git
$ git checkout spp_lto-pm_ptr_bit
$ git submodule --update init
$ ./install_dependencies.sh
$ nix-shell
```

## Automated Artifact Evaluation (approx. 8-10hrs)

Our automated scripts create the appropriate docker images and carry out each experiment with the appropriate configuration inside a dedicated docker container.

**IMPORTANT**: In the following sections, we assume that you are inside the [nix-shell](../README.md#automated-project-build) and have `/mnt/pmem0/` as the directory where PM is mounted. We also assume that the mounted PM device is attached in NUMA node with `id=0`.
If the machine you are using has multiple *NUMA nodes*, it is **mandatory** that you specify the node that the mounted PM is attached. This can be done via the `NUMA_NODE` environment variable. Otherwise, you can ignore this variable. Additionally, if you have PM mounted elsewhere, simply replace the path in the following commands.

To execute **all** the Artifact Evaluation steps, simply run:
```
$ cd SPP/artifact_evaluation
$ NUMA_NODE=0 BENCHMARK_PM_PATH=/mnt/pmem0/ ./artifact_evaluation.sh
```

The above script will do the following:
1. Create the appropriate docker images
2. Execute all the experiments under all the configurations
3. Gather their results and place them in this directory, as follows:
  - `figure_4-pmembench_indices.png` and `figure_4-pmembench_indices.pdf`
  - `figure_5-pmemkv.png` and `figure_5-pmemkv.pdf`
  - `figure_6-phoenix.png` and `figure_6-phoenix.pdf`
  - `figure_7-pmembench_PM_ops.png` and `figure_7-pmembench_PM_ops.pdf`
  - `table_2-recovery_time.txt`
  - `table_3-space_overhead.txt`
  - `table_4-ripe.txt`
  - `crash_consistency_results.txt`
4. The reproduction of the `btree` bug is left last, so that its output will be presented last in the `stdout` of the `artifact_evaluation.sh` script. For more information about this benchmark, please see [here](../benchmarks/bugs/README.md).

## Manual Artifact Evaluation

### 1. Image creation (approx. 30 mins)

1. Create the docker images that contain the default `clang-12` compiler and the SPP `clang-12` version.

```
$ cd SPP/utils/docker/compiler_images
$ ./create_clang_12_image.sh
$ ./create_spp_image.sh
```

2. Build the base images for the vanilla PMDK, SPP and SafePM experiments.

```
$ cd SPP/utils/docker/packaged_environments
$ ./create_env_pmdk.sh
$ ./create_env_safepm.sh
$ ./create_env_spp.sh
```

### 2. Experiments execution

**IMPORTANT**: In the following sections, we assume that you are inside the [nix-shell](../README.md#automated-project-build) and have `/mnt/pmem0/` as the directory where PM is mounted. If you have PM mounted elsewhere, simply replace the path in the following commands.
Additionally, all the timings listed below refer to our testbed. They may vary depending on the computing power of the used machine.

#### pmembench (approx. 45-60 mins)
To run the `pmembench` experiments for all the variants:
```
$ cd SPP/benchmarks/pmembench
$ NUMA_NODE=0 BENCHMARK_PM_PATH=/mnt/pmem0/ ./run_variants.sh
```
For more information about `pmembench`, please see [here](../benchmarks/pmembench/README.md).

#### pmemkv (approx. 240 mins)
To run the `pmemkv` experiments for all the variants:
```
$ cd SPP/benchmarks/pmemkv
$ NUMA_NODE=0 BENCHMARK_PM_PATH=/mnt/pmem0 ./run-variants.sh
```
For more information about `pmemkv`, please see [here](../benchmarks/pmemkv/README.md).

#### Phoenix (approx. 90 mins)
To run the `phoenix` experiments for all the variants:
```
$ cd SPP/benchmarks/phoenix
$ NUMA_NODE=0 BENCHMARK_PM_PATH=/mnt/pmem0 ./run_variants.sh
```
For more information about `phoenix`, please see [here](../benchmarks/phoenix/README.md).

#### Recovery time (approx. 10 mins)
To run the `recovery time` experiments for all the variants:
```
$ cd SPP/benchmarks/recovery_time
$ BENCHMARK_PM_PATH=/mnt/pmem0 ./run-variants.sh
```
For more information about the `recovery time` microbenchmark, please see [here](../benchmarks/recovery_time/README.md).

#### RIPE (approx. 30-40 mins)
To run the `ripe` experiments for all the variants:
```
$ cd SPP/benchmarks/ripe
$ BENCHMARK_PM_PATH=/mnt/pmem0 ./run-variants.sh
```
For more information about `ripe`, please see [here](../benchmarks/ripe/README.md).

#### Bug reproduction (approx. 1-2 mins)
To reproduce the `btree bug` of `PMDK`:
```
$ cd SPP/benchmarks/bugs
$ BENCHMARK_PM_PATH=/mnt/pmem0 ./spp_bugs.sh
```
The caught PM buffer overflow bugs will be presented in the `stdout` as faults.
For more information about the `bug reproduction`, please see [here](../benchmarks/bugs/README.md).

#### Space overhead (approx. 10 mins)
To run the `space overhead` experiments:
```
$ cd SPP/benchmarks/space_overhead
$ BENCHMARK_PM_PATH=/mnt/pmem0 ./run-variants.sh
```
For more information about the `space overhead` microbenchmark, please see [here](../benchmarks/space_overhead/README.md).

#### Crash consistency (approx. 20-30 mins)
To run the `crash consistency` validation experiments:
```
$ cd SPP/benchmarks/crash_consistency
$ BENCHMARK_PM_PATH=/mnt/pmem0 ./run-variants.sh
```
For more information about the `crash consistency` benchmark, please see [here](../benchmarks/crash_consistency/README.md).

### Results visualization
After the execution of the above benchmarks, and assuming that the results are placed in their default locations,
you can use the following commands to reproduce the figures and the tables of the SPP paper:

#### Figures 4 & 7
To produce the figures 4 and 7 of the paper:
```
$ cd SPP/plot_utils
$ ./import_pmembench.sh SPP/benchmarks/pmembench/results
$ python3 generate_plots.py plot_config_pmembench_tx.yml
$ python3 generate_plots.py plot_config_maps.yml
```

To copy the figures 4 and 7 in the `artifact_evaluation` directory:
```
$ cp SPP/plot_utils/plots/pmembench_map/pmembench_map_plot.png SPP/artifact_evaluation/figure_4-pmembench_indices.png
$ cp SPP/plot_utils/plots/pmembench_map/pmembench_map_plot.pdf SPP/artifact_evaluation/figure_4-pmembench_indices.pdf
$ cp SPP/plot_utils/plots/pmembench_tx/pmembench_tx_plot_single_vertical.png SPP/artifact_evaluation/figure_7-pmembench_PM_ops.png
$ cp SPP/plot_utils/plots/pmembench_tx/pmembench_tx_plot_single_vertical.pdf SPP/artifact_evaluation/figure_7-pmembench_PM_ops.pdf
```

#### Figure 5
To produce the figure 5 of the paper:
```
$ cd SPP/plot_utils
$ ./import_pmemkv.sh SPP/benchmarks/pmemkv/results
$ python3 generate_plots.py plot_config_pmemkv.yml
```

To copy the figure 5 in the `artifact_evaluation` directory:
```
$ cp SPP/plot_utils/plots/pmemkv/pmemkv_plot_overhead.png SPP/artifact_evaluation/figure_5-pmemkv.png
$ cp SPP/plot_utils/plots/pmemkv/pmemkv_plot_overhead.pdf SPP/artifact_evaluation/figure_5-pmemkv.pdf
```

#### Figure 6
To produce the figure 6 of the paper:
```
$ cd SPP/benchmarks/phoenix
$ python3 plot.py SPP/benchmarks/phoenix/results/phoenix_8_threads
```

To copy the figure 6 in the `artifact_evaluation` directory:
```
$ cp SPP/benchmarks/phoenix/results/phoenix_8_threads/phoenix_bar_plot_overhead.png SPP/artifact_evaluation/figure_6-phoenix.png
$ cp SPP/benchmarks/phoenix/results/phoenix_8_threads/phoenix_bar_plot_overhead.pdf SPP/artifact_evaluation/figure_6-phoenix.pdf
```

#### Table 2
To produce the table 2 of the paper in the `stdout` and copy it in the `artifact_evaluation` directory:
```
$ cd SPP/benchmarks/recovery_time
$ ./recovery_table.sh | tee SPP/artifact_evaluation/table_2-recovery_time.txt
```

#### Table 3
To produce the table 3 of the paper in the `stdout` and copy it in the `artifact_evaluation` directory:
```
$ cd SPP/benchmarks/space_overhead
$ ./space_overhead_results.sh | tee SPP/artifact_evaluation/table_3-space_overhead.txt
```

#### Table 4
To produce the table 4 of the paper in the `stdout` and copy it in the `artifact_evaluation` directory:
```
$ cd SPP/benchmarks/ripe
$ ./ripe_table.sh | tee SPP/artifact_evaluation/table_4-ripe.txt
```

#### Crash consistency
To output the results of the crash consistency checking tools in the `stdout` and copy it in the `artifact_evaluation` directory:
```
$ cd SPP/benchmarks/crash_consistency
$ ./crash-consistency-res.sh | tee SPP/artifact_evaluation/crash_consistency_results.txt
```

## Hardware configuration
**TODO**

## Software dependencies
We require the following software configuration to reproduce our experimental results:
1. Linux (tested in Ubuntu 20.04.3 LTS with kernel version 5.4.0)
2. Docker (tested with Docker version 20.10.7): Each experiment comes with its pre-configured Dockerfile. We provide scripts that automatically build the images containing the required software dependencies.
3. gcc and cmake (tested with gcc 9.3.0 and cmake 3.16.3)
4. Python 3.7 or newer (tested with Python 3.8.10). The packages `matplotlib` and `pyyml` are required.
5. The [`bc`](https://www.gnu.org/software/bc/) package for our analysis scripts.

## SPP code structure
For more information about the source code structure please see [here](https://github.com/dimstav23/SPP/tree/spp_lto-pm_ptr_bit#code-structure).