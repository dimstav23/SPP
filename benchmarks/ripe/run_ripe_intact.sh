mkdir -p build
make clean
make build/clang_attack_gen_system_heap
cd build && mv clang_attack_gen_system_heap clang_attack_gen && cd ../
python ripe_tester.py both 1 clang
