# Space overhead (~10mins)

## Workflow description

We build a new image (~5.5 GB), tagged as `spp-space_overhead`, using the [`Dockerfile`](./Dockerfile) which is based on the [SPP image](../../utils/docker/packaged_environments/). We patch the PMDK to be compiled with SPP.
Then, in a newly-built container, we run our microbenchmark. We execute the `insert` and `get` workloads for the PM indices (`ctree`, `rtree`, `rbtree` and `hashmap_tx`) using `pmembench`. Its configuration is defined in [`inner_run.sh`](./inner_run.sh).

We use the `pmempool` tool to measure the final size of the pool after the execution of each workload.

To run the microbenchmark, simply execute:
```
$ cd PROJECT_ROOT/benchmarks/space_overhead
$ BENCHMARK_PM_PATH=/path/to/pm ./run-variants.sh
```

The results of the execution are placed in the `results` directory of the current folder (created, if not existing).

To observe the results summary (SPP space overhead) in the `stdout` if you do not want to browse the files manually, run:
```
$ cd PROJECT_ROOT/benchmarks/space_overhead
$ ./space_overhead_results.sh
```

---

## File description

- [`Dockerfile`](./Dockerfile): Dockerfile that is used to patch the `PMDK` and compile it with SPP.
- [`inner_run.sh`](./inner_run.sh): Script to run inside the container and executes the benchmarks.
- [`pmdk_spp.patch`](./pmdk_spp.patch): PMDK patch to use the SPP passes.
- [`run-variants.sh`](./run-variants.sh): Creates the `spp-space_overhead` image and triggers the benchmark run inside a newly-built container.
- [`space_overhead_results.sh`](./space_overhead_results.sh): Script that analyses the results and prints the space overhead to `stdout` in a nice table.
