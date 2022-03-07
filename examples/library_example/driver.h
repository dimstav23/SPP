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

struct dummy {
	uint64_t x[2];
};

struct root_ {
	TOID(struct dummy) obj;
};