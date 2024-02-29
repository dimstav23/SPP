#!/bin/bash
set -e

# Copy out the IRs of the maps
echo "1. PM INDICES IR generation"
cd /SPP/pmdk/src/examples/libpmemobj
find ./ -type f | grep -i .ll$ | xargs -i cp {} /results/pm_indices/

# Copy out the pre-optimized and pre-codegen IRs of pmembench
echo "2. PMEMBENCH IR generation"
cd /SPP/pmdk/src/benchmarks
rm pmembench
LIBS=-Wl,-plugin-opt=save-temps make &>/dev/null
llvm-dis pmembench.0.0.preopt.bc
llvm-dis pmembench.0.5.precodegen.bc
mv pmembench.0.0.preopt.ll /results/pmembench/
mv pmembench.0.5.precodegen.ll /results/pmembench/

# Copy out the pre-optimized and pre-codegen IRs of pmemkv_bench
echo "3. PMEMKV_BENCH IR generation"
cd /pmemkv-bench
rm pmemkv_bench
CXXFLAGS=-Wl,-plugin-opt=save-temps make &>/dev/null
llvm-dis pmemkv_bench.0.0.preopt.bc
llvm-dis pmemkv_bench.0.5.precodegen.bc
mv pmemkv_bench.0.0.preopt.ll /results/pmemkv_bench/
mv pmemkv_bench.0.5.precodegen.ll /results/pmemkv_bench/

# Copy out pmemkv transformed IRs
echo "4. PMEMKV IR generation"
cd /pmemkv/build
sed -i 's/_CMAP/& -S -emit-llvm/' CMakeFiles/pmemkv.dir/flags.make
make clean && make &>/dev/null
mv CMakeFiles/pmemkv.dir/src/libpmemkv.cc.o /results/pmemkv/libpmemkv.cc.trasnformed.ll
mv CMakeFiles/pmemkv.dir/src/engine.cc.o /results/pmemkv/engine.cc.trasnformed.ll
mv CMakeFiles/pmemkv.dir/src/engines/blackhole.cc.o /results/pmemkv/blackhole.cc.trasnformed.ll
mv CMakeFiles/pmemkv.dir/src/out.cc.o /results/pmemkv/out.cc.trasnformed.ll
mv CMakeFiles/pmemkv.dir/src/iterator.cc.o /results/pmemkv/iterator.cc.trasnformed.ll
mv CMakeFiles/pmemkv.dir/src/engines/cmap.cc.o /results/pmemkv/cmap.cc.trasnformed.ll
cmake .. &>/dev/null
make clean && make &>/dev/null