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
	uint32_t i;
};

int main()
{
	unlink("/dev/shm/spp_test.pool");
	PMEMobjpool* pool = pmemobj_create("/dev/shm/spp_test.pool", "spp_test", 32*1024*1024, 0660);
	assert(pool != NULL);

	PMEMoid proot_ = pmemobj_root(pool, sizeof(struct root));
	assert(OID_IS_NULL(proot_) == false);
	struct root* proot = (struct root*)pmemobj_direct(proot_);
	
	proot->i = 1;
	print_pass_flag();
	*(uint64_t*)(&proot->i) = 2; // This line should crash
	print_fail_flag();

	pmemobj_close(pool);
	return 0;
}
