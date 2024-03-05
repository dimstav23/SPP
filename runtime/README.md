# SPP runtime library

## Files description

- [`create_spplib.sh`](./create_spplib.sh): Script that builds the object files constituting the SPP runtime library. It places them in the `obj/` directory (created if it's not existing).
- [`Makefile`](./Makefile): Makefile to compile the SPP runtime library.
- [`src/spp.c`](./src/spp.c): Source code of the SPP runtime functions that are used for the manipulation of the tagged pointer, its cleaning and the bounds checks (if they are activated through the `FAIL_OBL_COMP` flag).
- [`src/wrappers_spp.c`](./src/wrappers_spp.c): The memory intrinsic functions wrappers of SPP. Contains both the normal volatile memory variants as well as the PM memory intrinsic functions.
