# PMDK fork - Crash Consistency checks
**Crash consistency** is an important property to ensure both for PM applications and for PMDK itself.
Here we list a set of tools we used to ensure the correctness and crash consistency of our modified PMDK fork.

## Tools
We considered two state-of-the-art debugging tools based on [**Valgrind**](https://valgrind.org/):
- `pmemcheck`
- `PMDebugger`

### pmemcheck ([src](https://github.com/pmem/valgrind), [docs](https://www.intel.com/content/www/us/en/developer/articles/technical/discover-persistent-memory-programming-errors-with-pmemcheck.html))

Initially you have to fetch the repository:
```
$ git clone git@github.com:pmem/valgrind.git
```
All packages necessary to build the modified version of Valgrind that includes **pmemcheck** are the same as for the original version.

Once the build system is setup, Valgrind is built using these command at the top level of the fetched repository:
```
$ ./autogen.sh
$ ./configure [--prefix=/where/to/install]
$ make
```
To build tests:
```
$ make check
```
To install Valgrind run (possibly as root if destination permissions require that):
```
$ make install
```
For information on how to run the new tool refer to the appropriate part of the documentation or type:
```
$ valgrind --tool=pmemcheck --help
```
A practical example on how to run pmemcheck on the PMDK benchmark suite (**pmembench**) is:
```
$ cd pmdk_top_level/src/benchmarks
$ LD_LIBRARY_PATH=/path/to/pmdk/libs/ valgrind --tool=pmemcheck ./pmembench pmembench_config_file.cfg
```
### PMDebugger ([src](https://github.com/PASAUCMerced/PMDebugger), [paper](http://pasalabs.org/papers/2021/asplos21_pmdebugger_full_paper.pdf))
**PMDebugger** is based on **Valgrind** and is able to detect more bugs in PM-enabled applications compared to **pmemcheck**.
It is included in our project as a submodule. 
Initially you have to fetch the submodule:
```
$ git submodule update --init
```
Then compile and install the **PMDebugger** (see [here](https://github.com/PASAUCMerced/PMDebugger) for further instructions and customization):
```
$ cd valgrind-pmdebugger
$ ./autogen.sh
$ ./configure
$ make
$ make check
$ sudo make install 
```
After the successful installation you can run ```$ valgrind --tool=pmdebugger -h``` to get more information about the capabilities of PMDebugger.

A practical example on how to run **PMDebugger** on the PMDK benchmark suite (**pmembench**) is:
```
$ cd pmdk_top_level/src/benchmarks
$ LD_LIBRARY_PATH=/path/to/pmdk/libs/ valgrind --tool=pmdebugger --tree-reorganization=yes --order-guarantee=yes --redundant-logging=yes ./pmembench pmembench_config_file.cfg
```