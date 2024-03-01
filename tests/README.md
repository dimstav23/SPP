# Description of the SPP functionality tests & helper files

- [`common.h`](./common.h): Implements the `print_pass_flag` and `print_fail_flag` helper functions to determine the correct execution of the tests.
- [`free_tx_abort.cpp`](./free_tx_abort.cpp): **should NOT crash** | Accesses a PM object in-bounds after an aborted attempt to free it within a transaction which is a valid operation.
- [`overflow_ntx.cpp`](./overflow_ntx.cpp): **should crash** | PM object overflow access. Invalid operation -- therefore it should crash.
- [`overflow_persistence_helper.cpp`](./overflow_persistence_helper.cpp): Helper function that has 2 stages. In stage 1 it creates the pool and the root object. In stage 2 it attempts to perform an overflown access to the object. The stage is given as a cmdline argument. We have this test to show the ability of SPP to detect overflows even after program restarts. To see how to invoke this test, see [here](./overflow_persistence.sh).
- [`overflow_persistence.sh`](./overflow_persistence.sh): **should crash** | Script to run the [`overflow_persistence_helper.cpp`](./overflow_persistence_helper.cpp). It triggers an overflow in the Stage 2 of the run -- therefore it should crash.
- [`overflow.cpp`](./overflow.cpp): **should crash** | PM object overflow access. Invalid operation -- therefore it should crash.
- [`root_overflow.cpp`](./root_overflow.cpp): **should crash** | Root object overflow access. Invalid operation -- therefore it should crash.
- [`root_underflow.cpp`](./root_underflow.cpp): **should NOT crash** | Root object underflow. SPP does not detect underflows -- therefore, it's normal that it does not crash.
- [`tx_add_overflow.cpp`](./tx_add_overflow.cpp): **should crash** | PM buffer overflow on the snapshotting process of an object. It should crash.
- [`type_overflow.cpp`](./type_overflow.cpp): **should crash** | Contains a `memcpy` that overflows the buffer based on the type. Invalid operation -- therefore it should crash.
- [`zalloc.cpp`](./zalloc.cpp): **should NOT crash** | Contains valid PM object allocations and frees.
