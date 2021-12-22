#include <libpmemobj.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

extern void * mymalloc (int x);
extern void myfree (void * ptr);
extern void mystrcpy (char * x, char * y);

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
};

int main (int argc, char ** argv)
{
    char* str_src= "spp_test";
    char * str_dest= mymalloc(10); 

    mystrcpy (str_dest, str_src);
    
    printf("print_1 str_dest: %s\n", str_dest);
    
    char* temp= str_dest;
    
    temp++;
    printf("temp_tagged_1: 0x%" PRIx64 "\n", (uintptr_t)temp);
    
    temp++;
    printf("temp_tagged_1: 0x%" PRIx64 "\n", (uintptr_t)temp);
    
    myfree(str_dest);

    unlink("/dev/shm/spp_test.pool");

    PMEMobjpool* pool = pmemobj_create("/dev/shm/spp_test.pool", "spp_test", 32 * 1024 * 1024, 0666);
    assert(pool != NULL);

    PMEMoid proot_ = pmemobj_root(pool, sizeof(struct root_));
    assert(!OID_IS_NULL(proot_));

    struct root_* proot = (struct root_*)pmemobj_direct(proot_);
    printf("root oid %ld %ld ptr %p\n", proot_.pool_uuid_lo, proot_.off, proot);

    TX_BEGIN(pool) {
        PMEMoid oid = pmemobj_tx_alloc(sizeof(struct dummy), TOID_TYPE_NUM(struct dummy));
        pmemobj_tx_add_range_direct(&proot->obj, 16);
        //TOID_ASSIGN(proot->obj, oid);
    } TX_ONABORT{
        printf("Failed to allocate or snapshot a dummy object\n");
        abort();
    } TX_END

    return 0;
}

