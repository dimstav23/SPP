/*
Original author: Kartal Kaan Bozdoğan (mstniy)
Source repository: https://github.com/mstniy/safepm
*/

#include <libpmemobj.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>

POBJ_LAYOUT_BEGIN(spp_test);
POBJ_LAYOUT_TOID(spp_test, struct dummy);
POBJ_LAYOUT_END(spp_test);

struct dummy {
	uint64_t x[2];
};

int main()
{
	unlink("/mnt/pmem0/dimitrios/spp_test.pool");
	PMEMobjpool* pool = pmemobj_create("/mnt/pmem0/dimitrios/spp_test.pool", "spp_test", 32*1024*1024, 0660);
	assert(pool != NULL);

	TOID(struct dummy) oid;

	TX_BEGIN(pool) {
		oid = TX_NEW(struct dummy);
	} TX_END
	
	TX_BEGIN(pool) {
		TX_FREE(oid);
		pmemobj_tx_abort(0);
	} TX_END
	
	D_RW(oid)->x[0] = 1; // This line should not crash, as the free didn't actually happen
	
	pmemobj_close(pool);
	return 0;
}
