#include "libfuncs.h"

int test_func(int a) {
	return a+1;
}

void test_func_ptr(uint64_t *ptr, uint64_t set, uint64_t *ptr_2) {
	*ptr = set;
	*ptr_2 = set+1;
	return;
} 

void test_memcpy(PMEMobjpool* pop, struct test_struct* ptr1, struct test_struct* ptr2) {
	
	ptr1->x[0] = 4;
	ptr2->x[1] = 2;

	printf("%s args %p %p\n", __func__, ptr1, ptr2);
	memcpy(ptr2, ptr1, sizeof(struct test_struct) + 1);

	printf("%ld %ld\n", ptr2->x[0], ptr2->x[1]);

	return;
} 