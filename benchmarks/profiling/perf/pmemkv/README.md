## The pmemkv benchmark

Modified from [Robert Jandow's repo](https://github.com/RobertJndw/PMDK-performance-evaluation/tree/main/pmemkv-bench)

To run the benchmark, use `BENCHMARK_PM_PATH=<path to PM> ./run-variants.sh`. 
This will run the vanilla `PMDK` and `SPP` variants of the pmemkv benchmarks in the already built containers.
The results will be stored in the results folder of the current folder.
Note that the presenting results are the accummulated statistics for all the threads.
They do not reflect the actual execution time but the CPU time consumed in all the cores that were allocated.
The same applies to the HW instructions.

**Note**: If the machine you are using has multiple *NUMA nodes*, it is mandatory that you specify the node that the mounted PM is attached.
This can be done via the `NUMA_NODE` environment variable.
e.g. `NUMA_NODE=<node_id> BENCHMARK_PM_PATH=<path to PM> ./run-variants.sh`.

For more information about the images and containers, see [here](/utils/docker/README.md).
