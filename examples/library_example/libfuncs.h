#include <libpmemobj.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

struct test_struct {
    uint64_t x[2];
};

int test_func(int a);
void test_func_ptr(uint64_t *ptr, uint64_t set, uint64_t *ptr_2);
void test_memcpy(PMEMobjpool* pop, struct test_struct* ptr1, struct test_struct* ptr2);