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
POBJ_LAYOUT_TOID(spmo_test, struct dummy);
POBJ_LAYOUT_END(spmo_test);

struct dummy {
	uint64_t x[2];
};

int main()
{
	unlink("spmo_test.pool");
	PMEMobjpool* pool = pmemobj_create("spmo_test.pool", "spmo_test", 32*1024*1024, 0660);
	assert(pool != NULL);

	TOID(struct dummy) oid;

	TX_BEGIN(pool) {
		oid = TX_NEW(struct dummy);
		pmemobj_tx_abort(0);
	} TX_END
	
	print_pass_flag();
	D_RW(oid)->x[0] = 1; // This line should crash, as the allocation didn't actually happen
	print_fail_flag();
	
	pmemobj_close(pool);
	return 0;
}
