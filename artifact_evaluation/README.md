# Artifact evaluation

## Set-up
To reproduce the results of SPP, a machine equipped with a physical persistent memory module is required. SPP requires a DAX-enabled file system, such as EXT4-DAX. To format and mount the drive (assuming the device name is `/dev/pmem0`), follow the instructions below:
```
$ sudo mkdir /mnt/pmem0
$ sudo mkfs.ext4 /dev/pmem0
$ sudo mount -t ext4 -o dax /dev/pmem0 /mnt/pmem0
$ sudo chmod -R 777 /mnt/pmem0
 ```
In case you do not have a machine with persistent memory, you can execute the experiments by [emulating](https://pmem.io/blog/2016/02/how-to-emulate-persistent-memory/) it with DRAM, e.g., with the `/dev/shm/` directory. Note that, this results in considerable differences in the final performance measurements.

Further, you need to get the source code of SPP. It can be found by simply cloning the current repository:
```
$ git clone git@github.com:dimstav23/spp-pass.git
$ git checkout spp_lto-pm_ptr_bit
$ git submodule --update init
```

## Image creation
```
$ cd spp-pass/utils/docker/compiler_images
$ GIT_TOKEN=<your_git_token> ./create_clang_12_image.sh
$ GIT_TOKEN=<your_git_token> ./create_spp_image.sh

$ cd spp-pass/utils/docker/packaged_environments
$ ./create_env_pmdk.sh
$ ./create_env_safepm.sh
$ ./create_env_spp.sh
```

## Experiments execution
### pmembench
To run the `pmembench` experiments for all the variants:
```
$ cd spp-pass/benchmarks/pmembench
$ NUMA_NODE=<numa_node_id> BENCHMARK_PM_PATH=<path_to_pm> ./run_variants.sh
```
### pmemkv
To run the `pmemkv` experiments for all the variants:
```
$ cd spp-pass/benchmarks/pmemkv
$ NUMA_NODE=<numa_node_id> BENCHMARK_PM_PATH=<path_to_pm> ./run-variants.sh
```
### phoenix
To run the `phoenix` experiments for all the variants:
```
$ cd spp-pass/benchmarks/phoenix
$ NUMA_NODE=<numa_node_id> BENCHMARK_PM_PATH=<path_to_pm> ./run-variants.sh
```
### recovery time
To run the `recovery time` experiments for all the variants:
```
$ cd spp-pass/benchmarks/recovery_time
$ BENCHMARK_PM_PATH=<path_to_pm> ./run_variants.sh
```
### ripe
To run the `ripe` experiments for all the variants:
```
$ cd spp-pass/benchmarks/ripe
$ BENCHMARK_PM_PATH=<path_to_pm> ./run_variants.sh
```
**BUG**: memcheck requires sudo pwd for spp_user. Needs to be fixed for the automation.

### bug reproduction
To reproduce the `btree bug` of `PMDK`:
```
$ cd spp-pass/benchmarks/bugs
$ BENCHMARK_PM_PATH=<path_to_pm> ./spp_bugs.sh
```
### space overhead
To run the `space overhead` experiments:
```
$ cd spp-pass/benchmarks/space_overhead
$ BENCHMARK_PM_PATH=<path_to_pm> ./run_variants.sh
```
### crash_consistency
To run the `crash consistency` validation experiments:
```
$ cd spp-pass/benchmarks/crash_consistency
$ BENCHMARK_PM_PATH=<path_to_pm> ./run_variants.sh
```

## Results visualization
To produce the figures 4, 5 and 6 of the paper:
```
$ cd spp-pass/plot_utils
$ ./plot_all
```

```
$ cd spp-pass/benchmarks/recovery_time
$ ./recovery_table.sh
```

```
$ cd spp-pass/benchmarks/ripe
$ #TODO --- get from SafePM
```

```
$ cd spp-pass/benchmarks/space_overhead 
$ ./space_overhead_results.sh
```

```
$ cd spp-pass/benchmarks/crash_consistency 
$ #TODO --- get from SafePM
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
For more information about the source code structure please see [here](https://github.com/dimstav23/spp-pass/tree/spp_lto-pm_ptr_bit#code-structure).