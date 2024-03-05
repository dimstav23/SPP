# Recovery time (~10mins)

## Workflow description

We build a new image (~5.5 GB), tagged as `spp-recovery`, using the [`Dockerfile`](./Dockerfile) which is based on the [SPP image](../../utils/docker/packaged_environments/). We patch the PMDK to be compiled with SPP.
Then, in a newly-built container, we run our microbenchmark ([`source/recovery_run.c`](./source/recovery_run.c)) which performs the following:
1. Creates a PM pool and allocates a set of objects which contain 1 buffer and 1 PMEMoid each.
2. Start a PMDK transaction and snapshots the PMEMoid fields of the object (worst case scenario for SPP as it increases the log size for our variant).
3. Closes the pool before the TX end (to trigger a recovery in the next run)
4. Re-opens the PM pool and measures the time it takes for its initilization (recovery time from the interrupted transaction).

We run this benchmark for vanilla PMDK and SPP 100 times for different amount of objects (100, 1000, 10000, 100000, 1000000) as specified in [`inner_run.sh`](./inner_run.sh).

To run the benchmark, simply execute:
```
$ cd PROJECT_ROOT/benchmarks/recovery_time
$ BENCHMARK_PM_PATH=/path/to/pm ./run-variants.sh
```

The results of the execution are placed in the `results` directory of the current folder (created, if not existing).

To observe the results summary in the stdout if you do not want to browse the files manually, run:
```
$ cd PROJECT_ROOT/benchmarks/recovery_time
$ ./recovery_table.sh
```

---

## File description

- [`source/recovery_run.c`](./source/recovery_run.c): This is the source of our microbenchmark. It performs the following:
  1. Creates a PM pool and allocates a set of objects which contain 1 buffer and 1 PMEMoid each.
  2. Start a PMDK transaction and snapshots the PMEMoid fields of the object (worst case scenario for SPP as it increases the log size for our variant).
  3. Closes the pool before the TX end (to trigger a recovery in the next run)
  4. Re-opens the PM pool and measures the time it takes for its initilization (recovery time from the interrupted transaction).
- [`Dockerfile`](./Dockerfile): Dockerfile that is used to patch the `PMDK` and compile it with SPP.
- [`inner_run.sh`](./inner_run.sh): Script to run inside the container and execute the benchmarks.
- [`Makefile`](./Makefile): Makefile to build the [`source/recovery_run.c`](./source/recovery_run.c) inside the container.
- [`pmdk_spp.patch`](./pmdk_spp.patch): PMDK patch to use the SPP passes.
- [`recovery_table.sh`](./recovery_table.sh): Script that analyses the results, takes the average of the execute repeats and prints it to `stdout` in a nice table.
- [`run-variants.sh`](./run-variants.sh): Creates the `spp-recovery` image and triggers the benchmark run inside a newly-built container.
