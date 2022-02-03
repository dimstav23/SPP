make build/clang_attack_gen_pmemobj_heap
cd build && mv clang_attack_gen_pmemobj_heap clang_attack_gen && cd ../
python ripe_tester.py both 1 clang