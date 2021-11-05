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
	uint64_t x;
};

int main()
{
	unlink("/dev/shm/spp_test.pool");
	PMEMobjpool* pool = pmemobj_create("/dev/shm/spp_test.pool", "spp_test", 32*1024*1024, 0660);
	if (pool == NULL)
		abort();

	PMEMoid proot_ = pmemobj_root(pool, sizeof(struct root));
	if (OID_IS_NULL(proot_))
		abort();
	struct root* proot = (struct root*)pmemobj_direct(proot_);
	if (POBJ_NEW(pool, &proot->obj, struct dummy, NULL, NULL))
		abort();
	TOID(struct dummy) toid = proot->obj;
	POBJ_FREE(&proot->obj);
	print_pass_flag();
	POBJ_FREE(&toid); // This line should crash. The first FREE call sets proot->obj to NULL, so we free the other copy here.
	print_fail_flag();

	pmemobj_close(pool);
	return 0;
}
