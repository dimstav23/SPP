/*
Original author: Kartal Kaan Bozdoğan (mstniy)
Source repository: https://github.com/mstniy/safepm
*/

#include <libpmemobj.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include "common.h"

POBJ_LAYOUT_BEGIN(spmo_test);
POBJ_LAYOUT_ROOT(spmo_test, struct root);
POBJ_LAYOUT_TOID(spmo_test, struct dummy);
POBJ_LAYOUT_END(spmo_test);

struct root {
	TOID(struct dummy) obj;
};

struct dummy {
	uint64_t x;
};

int main()
{
	unlink("spmo_test.pool");
	PMEMobjpool* pool = pmemobj_create("spmo_test.pool", "spmo_test", 32*1024*1024, 0660);
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

	TX_BEGIN(pool) {
		pmemobj_tx_free(proot->obj.oid);
		print_pass_flag();
		pmemobj_tx_free(proot->obj.oid); // This line should crash. Note that because libpmemobj's transactional free is delayed until commit, upstream doesn't notice this fault.
	} TX_END

	print_fail_flag();

	pmemobj_close(pool);
	return 0;
}
