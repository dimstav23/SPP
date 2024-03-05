# Bugs found by SPP (1 min)

## Workflow description

To reproduce the pmembench bug found with SPP ([[1]](https://github.com/pmem/pmdk/issues/5333)), run:
``` 
./spp_bugs.sh <path-to-pm>
```
where `path-to-pm` is the directory where benchmarks will create the persistent memory pools.

The `btree` workload will run once in release mode and once in debug mode:
- In the `release` mode, a seg fault will be triggered when the overflow occurs.
- In the `debug` mode, the manual bound checks are enabled and the appropriate stack trace is also returned.

---

## File description
- [`bug1_btree_dbg.sh`](./bug1_btree_dbg.sh): Compiles the runtime and PMDK again inside the SPP container in debug mode, adjusts the PM pool path with the help of [`change_pmembench_bugs_file_path.sh`](change_pmembench_bugs_file_path.sh) and runs the `btree` workload.
- [`bug1_btree_rel.sh`](./bug1_btree_rel.sh): Compiles the runtime and PMDK again inside the SPP container in release mode, adjusts the PM pool path with the help of [`change_pmembench_bugs_file_path.sh`](change_pmembench_bugs_file_path.sh) and runs the `btree` workload.
- [`change_pmembench_bugs_file_path.sh`](change_pmembench_bugs_file_path.sh): Helper script to adjust the PM pool path for the benchmark.
- [`spp_bugs.sh`](./spp_bugs.sh): Uses the SPP image and runs the `btree` workload in debug and release mode.