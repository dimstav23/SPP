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
	uint64_t x[1024];
};

int main()
{
	unlink("/dev/shm/spp_test.pool");
	PMEMobjpool* pool = pmemobj_create("/dev/shm/spp_test.pool", "spp_test", 10*1024*1024, 0660);
	assert(pool != NULL);

	TOID(struct dummy) ptr;
	
	for (int i=0; i<1300; i++) {

		TX_BEGIN(pool) {
			ptr = TX_ZNEW(struct dummy);
		} TX_ONABORT {
			std::cerr << "Failed to allocate a dummy object" << std::endl;
			abort();
		}
		TX_END
		
		TX_BEGIN(pool) {
			TX_FREE(ptr);
		} TX_ONABORT {
			std::cerr << "Failed to free the dummy object" << std::endl;
			abort();
		}
		TX_END
	
	}

	for (int i=0; i<1300; i++) {

		TX_BEGIN(pool) {
			ptr = pmemobj_tx_xalloc(sizeof(struct dummy), TOID_TYPE_NUM(struct dummy), POBJ_XALLOC_ZERO);
		} TX_ONABORT {
			std::cerr << "Failed to allocate a dummy object" << std::endl;
			abort();
		}
		TX_END
		
		TX_BEGIN(pool) {
			TX_FREE(ptr);
		} TX_ONABORT {
			std::cerr << "Failed to free the dummy object" << std::endl;
			abort();
		}
		TX_END
	
	}
	
	pmemobj_close(pool);
	return 0;
}
