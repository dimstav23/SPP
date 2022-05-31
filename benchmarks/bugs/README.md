## Bugs found by SPP
To reproduce the bugs found with SPP ([[1]](https://github.com/pmem/pmdk/issues/5333) run:
``` 
./spp_bugs.sh <path-to-pm>
```
`path-to-pm` is the directory where benchmarks will create the persistent memory pools.

This execution will show the bug in the `standard output` with the appropriate overflow error message.