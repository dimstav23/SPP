# Minimal examples for SPP

We provide 2 sets of minimal examples.
To run them, make sure that you have completed the steps in the [Installation instructions](../README.md#installation-instructions) and that you are in the appropriate ```nix-shell```.

## Single source file example 
In the [`simple_example`](./simple_example/) folder you can find a minimal example which can be built and run with the [`launch.sh`](./simple_example/launch.sh) script as follows:
```
$ cd PROJECT_ROOT/examples/simple_example
$ ./launch.sh
```
The example opens a persistent memory pool, allocates the root object and performs various accesses and copies. 
It uses the `/dev/shm` for the pool (with the ```PMEM_IS_PMEM_FORCE``` exported flag, it is handled as PM from the application).
We opted for that option so that this simple example can run in systems without PM.
It also includes some dummy volatile memory actions in the beginning for comparison purposes of the programming model.
When you run the example, it should result in a SEGMENTATION FAULT because of the injected overflow. 
The [`example.c`](./simple_example/example.c) file has some annotated lines that should crash.
Feel free to experiment with those. One of them contains a function that also triggers a PM buffer overflow in the `memcpy` function.
The [`launch.sh`](./simple_example/launch.sh) script also produces the original and transformed LLVM-IR for the simple [`example.c`](./simple_example/example.c) code.

---

## Library example
In the [`library_example`](./library_example/) folder you can find a minimal example which can be built and run with the [`launch.sh`](./library_example/launch.sh) script as follows:
```
$ cd PROJECT_ROOT/examples/library_example
$ ./launch.sh
```
This examples showcases how SPP treats libraries. Especially, it highlights also one limitation of SPP when an overflow occurs in a shared library and cannot be caught.
We provide the [`libfuncs.h`](./library_example/libfuncs.h) and [`libfuncs.c`](./library_example/libfuncs.c) source code that is built into both a shared (.so) and a static (.a) library.
This library contains code that performs memory access and memory copies.
We provide the [`driver.c`](./library_example/driver.c) code that performs similar steps with the `simple_example` above but instead of performing the memory ations or calling the functions in the single source, it uses the compiled libraries.
Here, we also use the `/dev/shm` for the pool (with the ```PMEM_IS_PMEM_FORCE``` exported flag, it is handled as PM from the application).
Similarly with the `simple_example`, we opted for that option so that this simple example can run in systems without PM.
When you run the example, it should result in a SEGMENTATION FAULT because of the injected overflow for the case of the `driver_static` produced executable that uses the static library. The `driver_shared` runs normally without an error as the overflow cannot be detected because the tagged pointer is cleared when passed to the external library (clear limitation -- specified also in the paper).
Note the **line 60**: The `driver.c` calls the `test_memcpy` function. In the case of the static library the pointer is not cleared and therefore, the overflow is detected, causing the SEGMENTATION FAULT.
The [`driver.c`](./library_example/driver.c) file has some annotated lines that should crash.
Feel free to experiment with those. 
The [`launch.sh`](./library_example/launch.sh) script also produces the original and transformed LLVM-IR for the [`driver.c`](./library_example/driver.c) code.

---

## Note for the `launch.sh` scripts
The `launch.sh` scripts perform the appropriate compilation steps for each example, recompile the `SPP runtime` library and then execute the examples.
They are fairly simple -- feel free to browse through them :)
