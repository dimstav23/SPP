# The RIPE benchmark (~30-40mins)

The RIPE benchmark, originally developed by John Wilander and Nick Nikiforakis and
presented at the 2011 Annual Computer Security Applications Conference (ACSAC) in Orlando, Florida.

Ported to 64-bits by Hubert ROSIER.

Our current version is based on the [SafePM RIPE version](https://github.com/TUM-DSE/safepm/tree/master/benchmarks/ripe) that is patched to use the PMDK's persistent heap (instead of the volatile system heap).

For further details about RIPE, also see the original [Hubert ROSIER's repo](https://github.com/hrosier/ripe64).

## Workflow description

We build a set of new images, tagged as `ripe64-$variant` where `$variant` is one of the following:
1. intact (~5GB) : the image includes RIPE-64 that uses the volatile heap
2. pmem_heap (~5.3GB) : the image includes RIPE-64 that uses the PM heap
3. safepm (~8GB) : the image includes RIPE-64 that uses the PM heap and is compiled with SafePM
4. spp (~5.3GB) : the image includes RIPE-64 that uses the PM heap and is compiled with SPP
5. memcheck (~6.2GB) : the image includes RIPE-64 that uses the PM heap and can be run with the `memcheck` tool

Then, for each of the variants, starts a new container, builds the appropriate version of RIPE (volatile or PM port) and executes the [`ripe_tester.py`](./ripe_tester.py) script. 

Each RIPE overflow attack is executed 3 times by default, as specified in the [`ripe_tester.py`](./ripe_tester.py) inside the [`run-variants.sh`](./run-variants.sh) script.

To run the RIPE benchmark for all the variants, simply execute:
```
$ cd PROJECT_ROOT/benchmarks/ripe
$ BENCHMARK_PM_PATH=/path/to/pm ./run-variants.sh
```

The results of the execution are placed in the `results` directory of the current folder (created, if not existing).
The files inside this directory showcase the variant of the execution.

To observe the results summary in the `stdout` if you do not want to browse the files manually, run:
```
$ cd PROJECT_ROOT/benchmarks/ripe
$ ./ripe_table.sh
```

---

## File description
- [`source/`](./source/): Contains the source code of RIPE. It has both variants; [`attack_gen_system_heap.c`](./source/attack_gen_system_heap.c), that uses the volatile system heap, and the ported PM version, [`attack_gen_pmemobj_heap.c`](./source/attack_gen_pmemobj_heap.c), that uses the PM heap.
- [`Dockerfile_intact`](./Dockerfile_intact): Dockerfile that is used to build the `ripe-intact` image with the compiled RIPE benchmark using the default `clang-12` based on the [default clang image](../../utils/docker/compiler_images/).
- [`Dockerfile_memcheck`](./Dockerfile_memcheck): Dockerfile that is used to build the `ripe-memcheck` image with the compiled PM port of RIPE benchmark using the SPP `clang-12` based on the [SPP clang image](../../utils/docker/compiler_images/). It also installs the PMDK inside the image for the PM pool and object management of the RIPE port as well as `Valgrind` to obtain the `memcheck` tool. 
- [`Dockerfile_pmem_heap`](./Dockerfile_pmem_heap): Dockerfile that is used to build the `ripe-pmem_heap` image with the compiled PM port of RIPE benchmark using the SPP `clang-12` based on the [SPP clang image](../../utils/docker/compiler_images/). It also installs the PMDK inside the image for the PM pool and object management of the RIPE port.
- [`Dockerfile_safepm`](./Dockerfile_safepm): Dockerfile that is used to build the `ripe-safepm` image with the compiled PM port of RIPE benchmark using the default `clang-12` based on the [default clang image](../../utils/docker/compiler_images/). It also installs the SafePM PMDK inside the image for the PM pool and object management of the RIPE port. RIPE is compiled with SafePM **enabled**.
- [`Dockerfile_spp`](./Dockerfile_spp): Dockerfile that is used to build the `ripe-spp` image with the compiled PM port of RIPE benchmark using the SPP `clang-12` based on the [SPP clang image](../../utils/docker/compiler_images/). It also installs the SPP PMDK inside the image for the PM pool and object management of the RIPE port. RIPE is compiled with SPP **enabled**.
- [`Makefile`](./Makefile): Makefile to compile RIPE-64 in the native environment (outside the containers) with SPP **enabled**.
- [`Makefile_container`](./Makefile_container): Makefile to compile RIPE-64 inside the containers environment for all the variants depending on the provided `INTACT` and `ASAN` flags.
- [`memcheck.patch`](./memcheck.patch): Patch for [`ripe_tester.py`](./ripe_tester.py) for the `memcheck` variant.
- [`pmdk_spp.patch`](./pmdk_spp.patch): PMDK patch to use the SPP passes.
- [`ripe_tester.py`](./ripe_tester.py): Python script that instruments the execution of the RIPE attacks. The chosen attacks (buffer overflows) can be seen in the beginning of the script.
- [`run_ripe_intact.sh`](./run_ripe_intact.sh): Script that runs inside the container, builds the intact version (using the volatile heap) of RIPE and executes the [`ripe_tester.py`](./ripe_tester.py) script.
- [`run_ripe_spp.sh`](./run_ripe_spp.sh): Script that runs inside the container, builds the PM version (using the PM heap) of RIPE and executes the [`ripe_tester.py`](./ripe_tester.py) script.
- [`run-variants.sh`](./run-variants.sh): Creates the images for all the variants and triggers the benchmark run inside a newly-built containers.

**Note**: We further provide scripts to run `ripe` in additional environments:
- `*_O1.sh`: These scripts (with the suffix `_O1`) run `ripe` in the same environment but set the optimization level to `O1` using the respective `O1` images, produced by the scripts [here](../../utils/docker/packaged_environments/).
