# Crash Consistency (~20-30mins)

**Crash consistency** is an important property to ensure both for PM applications and for PMDK itself.
Here we list a set of tools we used to ensure the correctness and crash consistency of our modified PMDK fork.

## Workflow description

We use `pmemcheck `([src](https://github.com/pmem/valgrind), [docs](https://www.intel.com/content/www/us/en/developer/articles/technical/discover-persistent-memory-programming-errors-with-pmemcheck.html)), a state-of-the-art debugging tool based on [**Valgrind**](https://valgrind.org/).

We build a new image (~6 GB), tagged as `spp-crash_consistency`, using the [`Dockerfile`](./Dockerfile) which is based on the [SPP image](../../utils/docker/packaged_environments/). We patch the PMDK to be compiled with SPP and also install `Valgrind`.
Then, we run the persistent indices benchmark of `pmembench` and its transactional PM updates benchmark using `memcheck` and `pmemcheck`.

To run the benchmark, simply execute:
```
$ cd PROJECT_ROOT/benchmarks/crash_consistency
$ BENCHMARK_PM_PATH=/path/to/pm ./run-variants.sh
```

The results of the execution are placed in the `results` directory of the current folder (created, if not existing).

To observe the results summary in the stdout if you do not want to browse the files manually, run:
```
$ cd PROJECT_ROOT/benchmarks/crash_consistency
$ ./crash-consistency-res.sh
```

---

## File description

- [`change_pmembench_file_path`](./change_pmembench_file_path.sh): Helper script to adjust the PM pool path for the benchmark.
- [`Dockerfile`](./Dockerfile): Dockerfile that is used to patch the `PMDK` and fetch & install `Valgrind`.
- [`inner_run.sh`](./inner_run.sh): Script to run inside the container and execute the benchmarks.
- [`pmdk_spp.patch`](./pmdk_spp.patch): PMDK patch to use the SPP passes.
- [`run-variants.sh`](./run-variants.sh): Creates the `spp-crash_consistency` image and triggers the benchmark run inside a newly-built container.

---

## Manual execution (as an example of the flow)

Initially, fetch the `Valgrind` repository:
```
$ git clone git@github.com:pmem/valgrind.git
```
All packages necessary to build the modified version of Valgrind that includes **pmemcheck** are the same as for the original version.

Once the build system is setup, Valgrind can be built using these commands at the top level of the fetched repository:
```
$ ./autogen.sh
$ ./configure [--prefix=/where/to/install]
$ make
```
To build and run tests:
```
$ make check
```
To install Valgrind run (possibly as root if destination permissions require that):
```
$ make install
```
For information on how to run the new tool refer to the appropriate part of the documentation or type:
```
$ valgrind --tool=pmemcheck --help
```
A practical example on how to run pmemcheck on the PMDK benchmark suite (**pmembench**) is:
```
$ cd pmdk_top_level/src/benchmarks
$ LD_LIBRARY_PATH=/path/to/pmdk/libs/ valgrind --tool=pmemcheck ./pmembench pmembench_config_file.cfg
```