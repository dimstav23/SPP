///////////////////////////////////////////////
//////          spp.c           ///////////////
///////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

#include "libpmemobj.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <math.h>
#include <execinfo.h>
#include "./spp.h" ///

///////////////////////////////
///      Debug Macro        ///
///////////////////////////////

//#define SPP_PRINT_DEBUG
#ifdef SPP_PRINT_DEBUG
#  define dbg(x) x
#else
#  define dbg(x)
#endif

//#define SPP_DEBUG
#ifdef SPP_DEBUG
#  define __SPP_ATTR __optnone__ 
#else
#  define __SPP_ATTR always_inline 
#endif

//#define ASSERT_DEBUG
#ifdef ASSERT_DEBUG
#  define assert_dbg(x) x
#else
#  define assert_dbg(x) 
#endif

///////////////////////////////
///  End of Debug Macro     ///
///////////////////////////////

#define uthash_fatal(msg)
#define uthash_malloc(sz)    malloc(sz)
#define uthash_free(ptr,sz)  free(ptr)
#define uthash_strlen(s)     strlen(s)
#define uthash_memcmp(a,b,n) memcmp(a,b,n)
#define uthash_memset(s,c,n) memset(s,c,n)

#define CACHE_ADDR_ALIGN (1ULL << 10)                    // 1KB alignment of chunks
#define MAX_CACHE_SIZE ((1024*1024)/CACHE_ADDR_ALIGN)    // X entries to store 1MB of data
#define CACHE_OFFSET_MASK (CACHE_ADDR_ALIGN - 1ULL)
#define CACHE_ADDR_MASK   (~CACHE_OFFSET_MASK)

POBJ_LAYOUT_BEGIN(spp_cache);
POBJ_LAYOUT_TOID(spp_cache, struct CacheEntry);
POBJ_LAYOUT_END(spp_cache);

struct CacheEntry {
  void* key;    // out-of-bounds ptr value
  void* value;  // real allocated 64B chunk (ptr to which mem access is redirected)
  UT_hash_handle hh;
};

static struct CacheEntry *cache = NULL;

static void* highest_bound = 0;

static pthread_mutex_t cache_lock = PTHREAD_MUTEX_INITIALIZER;

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void* __spp_find_in_cache(void *ptrval, size_t size) {
  void* key  = (void*) ((uintptr_t)ptrval & CACHE_ADDR_MASK);
  uintptr_t offset = (uintptr_t)ptrval & CACHE_OFFSET_MASK;

  void* keyend = (void*) ((uintptr_t)(ptrval + size) & CACHE_ADDR_MASK);
  if (key != keyend) {
    // mem access spans more than one CACHE_ADDR_ALIGN chunk, panic
    // TODO: support such mem accesses?
    return NULL;
  }

  struct CacheEntry *entry;
  
  void* ret = NULL;

  pthread_mutex_lock(&cache_lock);
  HASH_FIND_PTR(cache, &key, entry);
  if (entry) {
    // remove it (so the subsequent add will throw it on the front of the list)
    HASH_DEL(cache, entry);
    HASH_ADD_PTR(cache, key, entry);
    ret = entry->value + offset;
  }
  pthread_mutex_unlock(&cache_lock);
  return ret;
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void* __spp_add_to_cache(void *ptrval, size_t size) {
  const void* key  = (const void*) ((uintptr_t)ptrval & CACHE_ADDR_MASK);
  uintptr_t offset = (uintptr_t)ptrval & CACHE_OFFSET_MASK;

  const void* keyend = (const void*) ((uintptr_t)(ptrval + size) & CACHE_ADDR_MASK);
  if (key != keyend) {
    // mem access spans more than one CACHE_ADDR_ALIGN chunk, panic
    // TODO: support such mem accesses?
    return NULL;
  }

  struct CacheEntry *entry, *tmp_entry;
  entry = uthash_malloc(sizeof(struct CacheEntry));
  entry->key   = key;
  entry->value = uthash_malloc(CACHE_ADDR_ALIGN);
  // will return ptr into newly allocated 64B chunk
  const void* ret = entry->value + offset;

  pthread_mutex_lock(&cache_lock);
  HASH_ADD_PTR(cache, key, entry);
  // prune the cache to MAX_CACHE_SIZE
  if (HASH_COUNT(cache) > MAX_CACHE_SIZE) {
    HASH_ITER(hh, cache, entry, tmp_entry) {
      // prune the first entry (loop is based on insertion order so this
      // deletes the oldest item)
      HASH_DEL(cache, entry);
      free((void*)entry->value);
      free(entry);
      break;
    }
  }
  pthread_mutex_unlock(&cache_lock);
  return ret;
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void* __spp_outofbounds(void* ptrval, size_t size) {
  ptrval = __spp_cleantag(ptrval);
  printf("[spp-info] TOLERATING OUT-OF-BOUNDS ACCESS AT %p\n", ptrval);

  void* realptr = __spp_find_in_cache(ptrval, size);
  if (realptr)  return realptr;
  // not found in LRU cache, try to allocate more memory for a write
  void* newptr = __spp_add_to_cache(ptrval, size);
  if (newptr)   return newptr;
  // cannot even add to cache, resort to failure-oblivious (dummy write)
  return (void*)(uintptr_t)(highest_bound + 2048);

  return ptrval;
}

////////////////////////////////
/* 
 * https://stackoverflow.com/questions/3899870/print-call-stack-in-c-or-c
 */
void 
print_trace(void) 
{
    printf("\033[0;34m");
    char **strings;
    size_t i, size;
    enum Constexpr { MAX_SIZE = 1024 };
    void *array[MAX_SIZE];
    size = backtrace(array, MAX_SIZE);
    strings = backtrace_symbols(array, size);
    for (i = 0; i < size; i++)
        printf("%s\n", strings[i]);
    puts("");
    free(strings);
    printf("\033[0m");
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
uintptr_t
__spp_extract_tagval (void *ptr)
{
    return ((uintptr_t)ptr)>>NUM_USED_BITS;
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
uintptr_t
__spp_extract_x86_tagval (void *ptr)
{
    assert_dbg(assert(NUM_X86_USED_BITS==48);)
    return ((uintptr_t)ptr)>>NUM_X86_USED_BITS;
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void*
__spp_cleantag (void *ptr)
{
    dbg(printf(">>%s with %p\n", __func__, ptr);)

    if (!__spp_extract_x86_tagval(ptr)) 
    {
        //if ptr is not tagged, return!
        return ptr;  
    }
    uintptr_t ptrval = ((((uintptr_t)ptr)<<NUM_SPARE_BITS)>>NUM_SPARE_BITS);
    dbg(printf(">>%s new_ptr: %p\n", __func__, ptrval);)
    return (void*)ptrval;
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void*
__spp_cleantag_external(void *ptr)
{
    dbg(printf(">>%s with %p\n", __func__, ptr);)
    
    if (!__spp_extract_x86_tagval(ptr)) 
    {
        //if ptr is not tagged, return!
        return ptr;  
    }
    
    uintptr_t ptrval = ((((uintptr_t)ptr)<<NUM_SPARE_BITS)>>NUM_SPARE_BITS);
    dbg(printf(">>%s new_ptr: %p\n", __func__, ptrval);)
    return (void*)ptrval;
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void*
__spp_checkbound(void *ptr)
{
    dbg(printf(">>%s with %p\n", __func__, ptr);)
    
    // NOTE: BE CAREFUL with signed/unsigned,
    // when performing bit operation.
    // Especially, shift opreations.
    
    if (!__spp_extract_x86_tagval(ptr)) 
    {
        //if ptr is not tagged, return!
        return ptr;
    }    
    
    uintptr_t tag = __spp_extract_tagval(ptr);
    
    // careful with function body optimised away at Opt level 2
    if (tag & 0x1000000) 
    {
        //simply print it in red
        printf("\033[0;31m");
        printf("!!!!> OVERFLOW detected at %s for %p\n", __func__, ptr);
        print_trace();
        printf("\033[0m");
        fflush(stdout);
        return __spp_outofbounds(ptr, 64);
        _exit(1);      
    }

    return __spp_cleantag(ptr);
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void* 
__spp_updatetag(void *ptr, int64_t off) {
    // ptr:  after pointer arithmetic  (i.e. GEP itself)
   
    ///////////////////////////////////////////////
    //  NOTE: BE CAREFUL with signed/unsigned,   //
    //  when performing bit operation.           //
    //  Especially, shift opreations.            //
    ///////////////////////////////////////////////
    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    
    if (!__spp_extract_x86_tagval(ptr))
    {
        //if ptr is not tagged, return!
        return ptr; 
    }
    
    int64_t tag = (int64_t)__spp_extract_tagval(ptr);     
    tag = tag + off;  
    
    uintptr_t tempval = ((uintptr_t)tag)<<NUM_USED_BITS;
    uintptr_t untagged = (uintptr_t)__spp_cleantag(ptr);
    uintptr_t ptrval = untagged | tempval; 
    
    dbg(printf(">>%s new_ptr: %p\n", __func__, ptrval);)
	
    return (void*)ptrval;
}

__attribute__((__used__))
__attribute__((__SPP_ATTR))
void* 
__spp_memintr_check_and_clean(void *ptr, int64_t off) {
    // ptr: pointer pass to LLVM memory intrinsic
    // off: offset to be added and checked
   
    // calculates the final address of the ptr according to the offset
    // performs bounds check
    // returns the clean ptr for the memory intrinsic function

    ///////////////////////////////////////////////
    //  NOTE: BE CAREFUL with signed/unsigned,   //
    //  when performing bit operation.           //
    //  Especially, shift opreations.            //
    ///////////////////////////////////////////////
    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    
    if (!__spp_extract_x86_tagval(ptr))
    {
        //if ptr is not tagged, return!
        return ptr; 
    }
    
    int64_t tag = (int64_t)__spp_extract_tagval(ptr);   
    // -1 is applied due to addressing starting from 0   
    dbg(printf(">>%s with %p and offset %ld and tag %lx\n", __func__, ptr, off, tag);)
    tag = tag + (off - 1); 
    
    dbg(printf(">>%s checked tag: %p\n", __func__, tag);)
    
    return (uintptr_t)__spp_cleantag(ptr);
}

// it is not desirable that this func is called
// normally, it SHOULDN'T be called
__attribute__((__used__))
__attribute__((__SPP_ATTR))
void* 
__spp_update_pointer(void *ptr, int64_t off) {
    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    assert_dbg(assert((uintptr_t)ptr==(int64_t)ptr);)
    void *newptr = (void*)((int64_t)ptr + off);
    return __spp_updatetag(newptr, off);
}

#ifdef __cplusplus
}
#endif

