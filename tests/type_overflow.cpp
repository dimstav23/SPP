/*
Original author: Kartal Kaan Bozdoğan (mstniy)
Source repository: https://github.com/mstniy/safepm
*/

#include <libpmemobj.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include "common.h"

POBJ_LAYOUT_BEGIN(spp_test);
POBJ_LAYOUT_ROOT(spp_test, struct root);
POBJ_LAYOUT_TOID(spp_test, struct dummy);
POBJ_LAYOUT_END(spp_test);

struct root {
	TOID(struct dummy) obj;
};

struct dummy {
	uint32_t x[2];
};

int main()
{
	unlink("/mnt/pmem0/dimitrios/spp_test.pool");
	PMEMobjpool* pool = pmemobj_create("/mnt/pmem0/dimitrios/spp_test.pool", "spp_test", 32*1024*1024, 0660);
	assert(pool != NULL);

	PMEMoid proot_ = pmemobj_root(pool, sizeof(struct root));
	assert(OID_IS_NULL(proot_) == false);
	struct root* proot = (struct root*)pmemobj_direct(proot_);

	TX_BEGIN(pool) {
		PMEMoid oid = pmemobj_tx_alloc(sizeof(struct dummy), TOID_TYPE_NUM(struct dummy));
		pmemobj_tx_add_range_direct(&proot->obj, 16);
		TOID_ASSIGN(proot->obj, oid);
	} TX_ONABORT {
		std::cerr << "Faild to allocate a dummy object" << std::endl;
		abort();
	}
	TX_END
	
	D_RW(proot->obj)->x[0] = (uint32_t) 1;
    print_pass_flag();
	uint64_t *exploit = (uint64_t*)calloc(48, sizeof(uint64_t));
	memcpy(&D_RW(proot->obj)->x[1], exploit, sizeof(*exploit)); // This line should crash
	print_fail_flag();
	
	pmemobj_close(pool);
	return 0;
}
