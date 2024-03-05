# SPP Benchmarks

This directory contains the automation for the benchmark execution of the SPP paper.

- [`bugs`](./bugs/): Scripts to reproduce the pmembench bug found with SPP ([[1]](https://github.com/pmem/pmdk/issues/5333)). For more information, see [here](./bugs/README.md). (Duration ~1min)
- [`crash_consistency`](./crash_consistency/): Scripts to run `memcheck` and `pmemcheck` (`Valgrind`-based) tools for crash consistency checks. (Duration ~20-30mins)
- [`llvm_ir_dump`](./llvm_ir_dump/): Helper benchmark that dumps the modified LLVM IRs by SPP of `pmembench`, `PM indices` of PMDK, `pmemkv` and `pmemkv_bench`. For more information, see [here](./llvm_ir_dump/README.md). (Duration <1min)
- [`phoenix`](./phoenix/): Scripts to run the `phoenix` benchmark suite and plot its results. For more information, see [here](./phoenix/README.md). (Duration ~90mins)
- [`pmembench`](./pmembench/): Scripts to run the `pmembench` benchmarks and plot their results. For more information, see [here](./pmembench/README.md). (Duration ~45-60mins)
- [`pmemkv`](./pmemkv/): Scripts to run the `pmemkv` benchmarks and plot their results. For more information, see [here](./pmemkv/README.md). (Duration ~240mins)
- [`recovery_time`](./recovery_time/): Scripts to run the `recovery_time` benchmarks. For more information, see [here](./recovery_time/README.md). (Duration ~10mins)
- [`ripe`](./ripe/): Scripts to run the `ripe` benchmark framework. For more information, see [here](./ripe/README.md). (Duration ~30-40mins)
- [`space_overhead`](./space_overhead/): Scripts to run the `sapce_overhead` benchmarks. For more information, see [here](./space_overhead/README.md). (Duration ~10mins)
