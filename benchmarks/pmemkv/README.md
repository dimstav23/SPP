## The pmemkv benchmark

Modified from [Robert Jandow's repo](https://github.com/RobertJndw/PMDK-performance-evaluation/tree/main/pmemkv-bench)

To run the benchmark, use `BENCHMARK_PM_PATH=<path to PM> ./run-variants.sh`. 
This will run the vanilla `PMDK`, `SPP` and `SafePM` (to be added) variants of the pmemkv benchmarks in the already built containers.

For more information about the images and containers, see [here](/utils/docker/README.md).
