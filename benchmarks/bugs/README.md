## Bugs found by SPP
To reproduce the bugs found with SPP ([[1]](https://github.com/pmem/pmdk/issues/5333) run:
``` 
./spp_bugs.sh <path-to-pm>
```
`path-to-pm` is the directory where benchmarks will create the persistent memory pools.

The btree workload will run once in release mode and one in debug mode.
In the `release` mode, a seg fault will be triggered when the overflow occurs.
In the `debug` mode, the manual bound checks are enabled and the appropriate stack trace is also returned.