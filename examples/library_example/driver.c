#include "libfuncs.h"
#include "driver.h"

int main()
{
	int k = 5;
	k = test_func(k);
	printf("%d\n",k);
	
	uint64_t *array, *array_2;
	array = (uint64_t*)malloc(ARRAY_SIZE * sizeof(uint64_t));
	array_2 = (uint64_t*)malloc(ARRAY_SIZE * sizeof(uint64_t));
	for (uint64_t i=0; i<ARRAY_SIZE; i++) {
		test_func_ptr(array+i, i, array_2+i);
	}
	free(array);
	free(array_2);

	char *str = (char*)calloc(sizeof(uint64_t), STR_SIZE);
	char *str_2 = (char*)malloc(STR_SIZE * sizeof(uint64_t));
	memcpy(str_2, str, STR_SIZE*sizeof(uint64_t)+1); //overflow by 1
	strcpy(str_2, str);
	free(str);
	free(str_2);

	unlink("/dev/shm/spp_test.pool_lib");

	PMEMobjpool* pool = pmemobj_create("/dev/shm/spp_test.pool_lib", "spp_test", 32*1024*1024, 0666);
	assert(pool != NULL);

	PMEMoid proot_ = pmemobj_root(pool, sizeof(struct root_));
	assert(!OID_IS_NULL(proot_));

	struct root_* proot = (struct root_*)pmemobj_direct(proot_);
	printf("root oid 0x%lx 0x%lx ptr %p\n", proot_.pool_uuid_lo, proot_.off, proot);

	TX_BEGIN(pool) {
		PMEMoid oid = pmemobj_tx_alloc(sizeof(struct dummy), TOID_TYPE_NUM(struct dummy));
		pmemobj_tx_add_range_direct(&proot->obj, 16);
		TOID_ASSIGN(proot->obj, oid);
	} TX_ONABORT {
		printf("Failed to allocate or snapshot a dummy object\n");
        abort();
	} TX_END

	printf("pool %p\n", pool);
	printf("ptrs %p %p\n", &D_RW(proot->obj)->x[0], &D_RW(proot->obj)->x[1]);

	D_RW(proot->obj)->x[0] = 1;
	D_RW(proot->obj)->x[1] = 2;
	//D_RW(proot->obj)->x[2] = 3; // This line should crash
	
	PMEMoid oid1,oid2;
	pmemobj_alloc(pool, &oid1, sizeof(struct test_struct), 0, NULL, NULL);
	pmemobj_alloc(pool, &oid2, sizeof(struct test_struct), 0, NULL, NULL);

	struct test_struct* ptr1 = pmemobj_direct(oid1);
	struct test_struct* ptr2 = pmemobj_direct(oid2);

	test_memcpy(pool, ptr1, ptr2); // This line should crash for the static library case

	pmemobj_free(&oid1);
	pmemobj_free(&oid2);
	
	pmemobj_close(pool);
	
	return 0;
}
