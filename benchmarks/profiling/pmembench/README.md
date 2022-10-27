## Porfiling data for the pmembench benchmark

To run the benchmark, use `BENCHMARK_PM_PATH=<path to PM> ./run_variants.sh`. 
This will run the vanilla `PMDK` abd `SPP` variants of the pmembench benchmarks in the already built containers.
The results will be stored in the results folder of the current folder.

**Note**: If the machine you are using has multiple *NUMA nodes*, it is mandatory that you specify the node that the mounted PM is attached.
This can be done via the `NUMA_NODE` environment variable.
e.g. `NUMA_NODE=<node_id> BENCHMARK_PM_PATH=<path to PM> ./run_variants.sh`.

For more information about the images and containers, see [here](/utils/docker/README.md).