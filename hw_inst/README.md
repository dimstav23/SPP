# Compilation instructions

How to compile if you want to enable the HW instructions:
```
$ clang -mbmi2 -mbmi test_hw_instr.c -o test_hw_instr
```

Then simply run:
```
$ ./test_hw_instr
```

This program will print you stats for the several bit manipulation commands.
