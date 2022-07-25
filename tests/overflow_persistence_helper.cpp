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
POBJ_LAYOUT_END(spp_test);

struct root {
	uint64_t arr[2];
};

void createPool() {
	unlink("/mnt/pmem0/dimitrios/spp_test.pool");
	PMEMobjpool* pool = pmemobj_create("/mnt/pmem0/dimitrios/spp_test.pool", "spp_test", 32*1024*1024, 0660);
	assert(pool != NULL);
	
	PMEMoid proot_ = pmemobj_root(pool, sizeof(struct root));
	assert(OID_IS_NULL(proot_) == false);

	pmemobj_close(pool);
}

void attemptOverflow() {
	PMEMobjpool* pool = pmemobj_open("/mnt/pmem0/dimitrios/spp_test.pool", "spp_test");
	assert(pool != NULL);
	
	PMEMoid proot_ = pmemobj_root(pool, sizeof(struct root));
	assert(OID_IS_NULL(proot_) == false);
	struct root* proot = (struct root*)pmemobj_direct(proot_);

	print_pass_flag(); // print moved here as the gep preemption detects the overflow earlier
	proot->arr[0] = 1;
	proot->arr[1] = 1;
	// print_pass_flag();
	proot->arr[2] = 1; // This line should crash
	print_fail_flag();
	
	pmemobj_close(pool);
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		std::cerr << "Usage: overflow_persistence_helper <stage: 1 or 2>" << std::endl;
		return 1;
	}
	if (strcmp(argv[1], "1") == 0) {
		createPool();
	}
	else {
		attemptOverflow();
	}

	return 0;
}
