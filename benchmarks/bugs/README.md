# Bugs found by SPP (5 min)

## Workflow description

To reproduce the pmembench bugs found with SPP, run:
``` 
$ cd PROJECT_ROOT/benchmarks/bugs
$ BENCHMARK_PM_PATH=/path/to/pm ./spp_bugs.sh
```
where `/path/to/pm` is the directory where benchmarks will create the persistent memory pools.

During the execution, you can view in the `stdout` the outcome of the bugs reproduction. Every script should end with a segmentation fault (expected).

Precisely:
- The `btree` workload will run once in release mode and once in debug mode:
  - In the `release` mode, a segmentation fault will be triggered when the overflow occurs.
  - In the `debug` mode, the manual bound checks are enabled and the appropriate stack trace is also returned.
- The `phoenix` bug occurs in the `string_match` application of the phoenix benchmark suite. It happens because the application performs an off-by-one buffer overflow on the buffer allocated for the input file. This causes the [`bug2_phoenix.sh`](./bug2_phoenix.sh) script to end with a segmentation fault.
- The `array` PMDK example initially allocates an array and then requests a reallocation that results in a PM buffer overflow and causes the [`bug3_pmdk_array.sh`](./bug3_pmdk_array.sh) script to end with a segmentation fault.

---

## File description
- [`bug1_btree_dbg.sh`](./bug1_btree_dbg.sh): Compiles the runtime and PMDK again inside the SPP container in debug mode, adjusts the PM pool path with the help of [`change_pmembench_bugs_file_path.sh`](change_pmembench_bugs_file_path.sh) and runs the `btree` workload.
- [`bug1_btree_rel.sh`](./bug1_btree_rel.sh): Compiles the runtime and PMDK again inside the SPP container in release mode, adjusts the PM pool path with the help of [`change_pmembench_bugs_file_path.sh`](change_pmembench_bugs_file_path.sh) and runs the `btree` workload.
- [`bug2_phoenix.sh`](./bug2_phoenix.sh): Compiles the runtime and PMDK with tjeor appropriate parameters inside the SPP container. Then, it reverts the file allocation of the `string_match` phoenix application to its [original size](https://github.com/dimstav23/phoenix/blob/b8aa166e15220c9a31d43efd13067478c7114f34/phoenix-2.0/tests/string_match/string_match.c#L259). Finally, it runs the `string_match` phoenix application that triggers the buffer overflow.
Note that in our port of phoenix, we already fixed this bug [here](https://github.com/dimstav23/phoenix/blob/b8aa166e15220c9a31d43efd13067478c7114f34/PM_phoenix-2.0/tests/string_match/string_match.c#L276). Therefore, we have to modify our source code to reflect the intact state of the phoenix benchmark.
- [`bug3_pmdk_array.sh`](./bug3_pmdk_array.sh): Compiles the runtime and PMDK, along with its examples, inside the SPP container and runs the `array` PMDK exmample application (first alloc the array and then request a realloc) that triggers the buffer overflow.
- [`change_pmembench_bugs_file_path.sh`](change_pmembench_bugs_file_path.sh): Helper script to adjust the PM pool path for the `btree` bug.
- [`spp_bugs.sh`](./spp_bugs.sh): Uses the SPP image and runs the `btree` workload in debug and release mode, the `string_match` application from the phoenix benchmark and the `array` PMDK example application to showcase the PM buffer overflows.