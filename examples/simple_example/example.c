/*
Original author: Kartal Kaan Bozdoğan (mstniy)
Source repository: https://github.com/mstniy/safepm
*/

#include <libpmemobj.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#define ARRAY_SIZE 100
#define STR_SIZE 200

POBJ_LAYOUT_BEGIN(spp_test);
POBJ_LAYOUT_ROOT(spp_test, struct root);
POBJ_LAYOUT_TOID(spp_test, struct dummy);
POBJ_LAYOUT_END(spp_test);

struct root_ {
	TOID(struct dummy) obj;
};

struct dummy {
	uint64_t x[2];
	PMEMoid next;
};


int test_func(int a) {
	return a+1;
}

void test_func_ptr(uint64_t *ptr, uint64_t set, uint64_t *ptr_2) {
	*ptr = set;
	*ptr_2 = set+1;
	return;
} 

void test_memcpy(PMEMobjpool* pop) {
	
	PMEMoid oid1,oid2;
	PMEMoid *oid_ptr1, *oid_ptr2;
	oid_ptr1 = (PMEMoid*)malloc(sizeof(PMEMoid));
	oid_ptr2 = (PMEMoid*)malloc(sizeof(PMEMoid));

	pmemobj_alloc(pop, oid_ptr1, sizeof(struct dummy), 0, NULL, NULL);
	struct dummy* ptr1 = pmemobj_direct(*oid_ptr1);

	pmemobj_alloc(pop, &ptr1->next, sizeof(struct dummy), 0, NULL, NULL);

	struct dummy* ptr2 = pmemobj_direct(ptr1->next);

	ptr1->x[0] = 4;
	ptr2->x[1] = 2;

	memcpy(ptr2, ptr1, sizeof(struct dummy) + 1);

	printf("%ld %ld\n", ptr2->x[0], ptr2->x[1]);

	pmemobj_free(&ptr1->next);
	pmemobj_free(oid_ptr1);

	free(oid_ptr1);
	free(oid_ptr2);
	return;
} 

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

	unlink("/dev/shm/spp_test.pool");

	PMEMobjpool* pool = pmemobj_create("/dev/shm/spp_test.pool", "spp_test", 32*1024*1024, 0666);
	assert(pool != NULL);

	PMEMoid proot_ = pmemobj_root(pool, sizeof(struct root_));
	assert(!OID_IS_NULL(proot_));

	struct root_* proot = (struct root_*)pmemobj_direct(proot_);
	printf("root oid %ld %ld ptr %p\n", proot_.pool_uuid_lo, proot_.off, proot);

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
	
	test_memcpy(pool);

	//D_RW(proot->obj)->x[2] = 3; // This line should crash
	
	pmemobj_close(pool);
	
	return 0;
}
