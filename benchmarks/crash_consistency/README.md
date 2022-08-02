# PMDK fork - Crash Consistency checks
**Crash consistency** is an important property to ensure both for PM applications and for PMDK itself.
Here we list a set of tools we used to ensure the correctness and crash consistency of our modified PMDK fork.

## Tools
We considered a state-of-the-art debugging tool based on [**Valgrind**](https://valgrind.org/):
- `pmemcheck`

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

To run the SPP crash consistency benchmark end-to-end:
```
$ BENCHMARK_PM_PATH=/path/to/pm ./run-variants.sh
```