# LLVM IR Dumps (<1min)

Helper benchmark that dumps the modified LLVM IRs by SPP of `pmembench`, `PM indices` of PMDK, `pmemkv` and `pmemkv_bench`.

## Workflow description

We start a container based on the [SPP image](../../utils/docker/packaged_environments/) and [SPP image with O1 optimization level](../../utils/docker/packaged_environments/), and the produce the IRs of the aforementioned applications using the [`run_spp.sh`](./run_spp.sh) and [`run_spp_O1.sh`](./run_spp_O1.sh) scripts depending on the desired optimization level.

To produce the LLVM IRs, run:
```
$ cd PROJECT_ROOT/benchmarks/llvm_ir_dump
$ ./dump_all_IR.sh
```

The resulting IRs are placed in the `spp_IRs` and `spp_o1_IRs` directories of the current folder (created, if not existing).

---

## File description

- [`dump_all_IR.sh`](./dump_all_IR.sh): Creates 2 containers and executed the [`run_spp.sh`](./run_spp.sh) and [`run_spp_O1.sh`](./run_spp_O1.sh) scripts inside of them (one in each) to produce the desired IRs of `pmembench`, `PM indices` of PMDK, `pmemkv` and `pmemkv_bench` after the SPP transformation pass.
- [`run_spp_O1.sh`](./run_spp_O1.sh): Compiles the aforementioned projects and dumps its IR into files that are copied in the result directories using O1 optimization level.
- [`run_spp.sh`](./run_spp.sh): Compiles the aforementioned projects and dumps its IR into files that are copied in the result directories using their default optimization level (O2,O3). 