///////////////////////////////////////////////
//////          spp.c           ///////////////
///////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>
#include <execinfo.h>
#include <signal.h>
#include <string.h>
#include "./spp.h" ///

#if defined(__x86_64__) && defined(__BMI2__) && !defined(HW_OFF) 
#include "x86intrin.h"
#endif

///////////////////////////////
///      Debug Macro        ///
///////////////////////////////

// #define SPP_PRINT_DEBUG
#ifdef SPP_PRINT_DEBUG
#  define dbg(x) x
#else
#  define dbg(x)
#endif

// #define SPP_DEBUG
#ifdef SPP_DEBUG
#  define __SPP_INLINE __attribute__((__optnone__))
#else
#  define __SPP_INLINE __attribute__((always_inline)) 
#endif

#define __SPP_USED __attribute__((__used__))

#define __SPP_ATTR __SPP_INLINE __SPP_USED

///////////////////////////////
///  End of Debug Macro     ///
///////////////////////////////

///////////////////////////////
///      Stats Macro        ///
///////////////////////////////

// #define SPP_STATS
#ifdef SPP_STATS
    #define stats(x) x
    long int __spp_extract_tagval_cnt = 0;
    long int __spp_is_pm_ptr_cnt = 0;
    long int __spp_vol_ptr_check_cnt = 0;
    long int __spp_cleantag_cnt = 0;
    long int __spp_cleantag_cnt_direct = 0;
    long int useless__spp_cleantag_cnt = 0;
    long int __spp_cleantag_external_cnt = 0;
    long int __spp_cleantag_external_cnt_direct = 0;
    long int useless__spp_cleantag_external_cnt = 0;
    long int __spp_checkbound_cnt = 0;
    long int __spp_checkbound_cnt_direct = 0;
    long int useless__spp_checkbound_cnt = 0;
    long int __spp_updatetag_cnt = 0;
    long int __spp_updatetag_cnt_direct = 0;
    long int useless__spp_updatetag_cnt = 0;
    long int __spp_memintr_check_and_clean_cnt = 0;
    long int __spp_memintr_check_and_clean_cnt_direct = 0;
    
    struct func_cnt {
        char* fname;
        int cnt;
        struct func_cnt* next;
    };
    struct func_cnt* head = NULL;

    void __spp_runtime_stats() {
        printf("***** SPP runtime statistics *****\n");
        printf("__spp_extract_tagval_cnt\t\t: %ld \n", __spp_extract_tagval_cnt);
        printf("__spp_is_pm_ptr_cnt\t\t\t: %ld \n", __spp_is_pm_ptr_cnt);
        printf("__spp_vol_ptr_check_cnt\t\t\t: %ld \n", __spp_vol_ptr_check_cnt);
        printf("__spp_cleantag_cnt\t\t\t: %ld \n", __spp_cleantag_cnt);
        printf("USELESS__spp_cleantag_cnt\t\t: %ld \n", useless__spp_cleantag_cnt);
        printf("__spp_cleantag_external_cnt\t\t: %ld \n", __spp_cleantag_external_cnt);
        printf("USELESS__spp_cleantag_external_cnt\t: %ld \n", useless__spp_cleantag_external_cnt);
        printf("__spp_checkbound_cnt\t\t\t: %ld \n", __spp_checkbound_cnt);
        printf("USELESS__spp_checkbound_cnt\t\t: %ld \n", useless__spp_checkbound_cnt);
        printf("__spp_updatetag_cnt\t\t\t: %ld \n", __spp_updatetag_cnt);
        printf("USELESS__spp_updatetag_cnt\t\t: %ld \n", useless__spp_updatetag_cnt);
        printf("__spp_memintr_check_and_clean_cnt\t: %ld \n", __spp_memintr_check_and_clean_cnt);
        printf("\n\n");
        printf("***** SPP direct runtime statistics *****\n");
        printf("__spp_cleantag_cnt_direct\t\t: %ld \n", __spp_cleantag_cnt_direct);
        printf("__spp_cleantag_external_cnt_direct\t: %ld \n", __spp_cleantag_external_cnt_direct);
        printf("__spp_checkbound_cnt_direct\t\t: %ld \n", __spp_checkbound_cnt_direct);
        printf("__spp_updatetag_cnt_direct\t\t: %ld \n", __spp_updatetag_cnt_direct);
        printf("__spp_memintr_check_and_clean_cnt_direct: %ld \n", __spp_memintr_check_and_clean_cnt_direct);
        printf("\n\n");
        struct func_cnt* curr = head;
        struct func_cnt* temp;
        while (curr) {
            temp = curr;
            printf("%d in check %s\n", curr->cnt, curr->fname);
            curr = curr->next;
            free(temp->fname);
            free(temp);
        }
        return;
    }
    int declared = 0;

    __SPP_ATTR
    void 
    __spp_update_checkcnt(char *fname) {
        if (head == NULL) 
        {
            head = (struct func_cnt*)malloc(sizeof(struct func_cnt));
            head->fname = (char*) malloc(strlen(fname)+1);
            strcpy(head->fname, fname);
            head->cnt = 1;
            head->next = NULL;
        }
        else
        {
            struct func_cnt* curr = head;
            while(strcmp(curr->fname, fname) != 0)
            {
                if (curr->next != NULL)
                {
                    curr = curr->next;
                }
                else 
                {
                    curr->next = (struct func_cnt*)malloc(sizeof(struct func_cnt));
                    curr->next->fname = (char*)malloc(strlen(fname)+1);
                    strcpy(curr->next->fname, fname);
                    curr->next->cnt = 1;
                    curr->next->next = NULL;
                }
            }
            curr->cnt++;
        }
        return;
    }

#else
    #define stats(x) 
#endif

///////////////////////////////
///  End of Stats Macro     ///
///////////////////////////////

/* 
 * https://stackoverflow.com/questions/3899870/print-call-stack-in-c-or-c
 */
static void 
error_report(const char* func, void* ptr) 
{
    printf("\033[0;31m");
    printf("!!!!> OVERFLOW detected at %s for %p\n", __func__, ptr);
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
    fflush(stdout);
    raise(SIGINT);
    exit(1);
}

__SPP_ATTR
uintptr_t
__spp_extract_tagval(void *ptr)
{
    stats(__spp_extract_tagval_cnt++;)
#if defined(__x86_64__) && defined(__BMI2__) && !defined(HW_OFF)
    return _pext_u64((uintptr_t)ptr, TAG_MASK);
#else
    return ((uintptr_t)ptr & TAG_MASK) >> NUM_PTR_BITS;
#endif
}

__SPP_ATTR
uintptr_t
__spp_is_pm_ptr(void *ptr)
{
    stats(__spp_is_pm_ptr_cnt++;)
#if defined(__x86_64__) && defined(__BMI2__) && !defined(HW_OFF)
    return _pext_u64((uintptr_t)ptr, PM_PTR_MASK);
#else 
    return ((uintptr_t)ptr) & PM_PTR_MASK;
#endif
}

__SPP_ATTR
void
__spp_manual_checkbound(void *ptr)
{
    printf("%s %p\n", __func__, ptr);
#if defined(__x86_64__) && defined(__BMI2__) && !defined(HW_OFF)
    if (_pext_u64((uintptr_t)ptr, OVERFLOW_MASK))
#else
    if ((uintptr_t)ptr & OVERFLOW_MASK)
#endif
    {
        //simply print it in red
        dbg(printf("\033[0;31m");)
        dbg(printf("!!!!> OVERFLOW detected at %s for %p\n", __func__, ptr);)
        error_report(__func__, ptr);    
    }
}

__SPP_ATTR
void*
__spp_cleantag(void *ptr)
{
    stats(__spp_cleantag_cnt++;)
    dbg(printf(">>%s with %p\n", __func__, ptr);)

    if (!__spp_is_pm_ptr(ptr)) 
    {
        stats(useless__spp_cleantag_cnt++;)
        stats(__spp_vol_ptr_check_cnt++;)
        //if ptr is not tagged, return!
        return ptr;  
    }

    uintptr_t ptrval = (uintptr_t)ptr & PTR_MASK;

    dbg(printf(">>%s new_ptr: %p\n", __func__, ptrval);)
    return (void*)ptrval;
}

__SPP_ATTR
void*
__spp_cleantag_direct(void *ptr)
{
    stats(__spp_cleantag_cnt++;)
    stats(__spp_cleantag_cnt_direct++;)
    dbg(printf(">>%s with %p\n", __func__, ptr);)
    
    uintptr_t ptrval = (uintptr_t)ptr & PTR_MASK;

    dbg(printf(">>%s new_ptr: %p\n", __func__, ptrval);)
    return (void*)ptrval;
}

__SPP_ATTR
void*
__spp_cleantag_external(void *ptr)
{
    stats(
        if (!declared) 
        {
            atexit(__spp_runtime_stats);
            declared=1;
        }
    )
    stats(__spp_cleantag_external_cnt++;)
    dbg(printf(">>%s with %p\n", __func__, ptr);)
    
    if (!__spp_is_pm_ptr(ptr)) 
    {
        stats(useless__spp_cleantag_external_cnt++;)
        stats(__spp_vol_ptr_check_cnt++;)
        //if ptr is not tagged, return!
        return ptr;  
    }
    
    uintptr_t ptrval = (uintptr_t)ptr & PTR_MASK;
    
    dbg(printf(">>%s old_ptr: %p new_ptr: %p\n", __func__, ptr, ptrval);)
    return (void*)ptrval;
}

__SPP_ATTR
void*
__spp_cleantag_external_direct(void *ptr)
{
    stats(
        if (!declared) 
        {
            atexit(__spp_runtime_stats);
            declared=1;
        }
    )
    stats(__spp_cleantag_external_cnt++;)
    stats(__spp_cleantag_external_cnt_direct++;)
    dbg(printf(">>%s with %p\n", __func__, ptr);)
    
    uintptr_t ptrval = (uintptr_t)ptr & PTR_MASK;
    
    dbg(printf(">>%s old_ptr: %p new_ptr: %p\n", __func__, ptr, ptrval);)
    return (void*)ptrval;
}

__SPP_ATTR
void*
__spp_checkbound(void *ptr)
{
    stats(__spp_checkbound_cnt++;)
    dbg(printf(">>%s with %p\n", __func__, ptr);)
    
    // NOTE: BE CAREFUL with signed/unsigned,
    // when performing bit operation.
    // Especially, shift opreations.
    
    if (!__spp_is_pm_ptr(ptr)) 
    {
        stats(__spp_vol_ptr_check_cnt++;)
        //if ptr is not tagged, return!
        stats(useless__spp_checkbound_cnt++;)
        return ptr;
    }    

#ifdef FAIL_OBL_COMP
    __spp_manual_checkbound(ptr);
#endif

    return __spp_cleantag_direct(ptr);
}

__SPP_ATTR
void*
__spp_checkbound_direct(void *ptr)
{
    stats(__spp_checkbound_cnt++;)
    stats(__spp_checkbound_cnt_direct++;)
    dbg(printf(">>%s with %p\n", __func__, ptr);)
    
    // NOTE: BE CAREFUL with signed/unsigned,
    // when performing bit operation.
    // Especially, shift opreations.  

#ifdef FAIL_OBL_COMP
    __spp_manual_checkbound(ptr);
#endif

    return __spp_cleantag_direct(ptr);
}

__SPP_ATTR
void* 
__spp_updatetag(void *ptr, int64_t off) {
    stats(__spp_updatetag_cnt++;)
    // ptr:  after pointer arithmetic  (i.e. GEP itself)
   
    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    
    if (!__spp_is_pm_ptr(ptr))
    {
        stats(__spp_vol_ptr_check_cnt++;)
        //if ptr is not tagged, return!
        stats(useless__spp_updatetag_cnt++;)
        return ptr; 
    }
    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    int64_t tag = (int64_t)__spp_extract_tagval(ptr);     
    tag = tag + off;  
    
    uintptr_t tempval = ((uintptr_t)tag) << NUM_PTR_BITS; // | PM_PTR_SET;
    uintptr_t untagged = (uintptr_t)__spp_cleantag_direct(ptr);
    untagged = untagged & ~OVERFLOW_MASK; // remove previous overflow bit
    uintptr_t ptrval = untagged | tempval; 
    
    dbg(printf(">>%s new_ptr: %p\n", __func__, ptrval);)
	
    return (void*)ptrval;
}

__SPP_ATTR
void* 
__spp_updatetag_direct(void *ptr, int64_t off) {
    stats(__spp_updatetag_cnt++;)
    stats(__spp_updatetag_cnt_direct++;)
    // ptr:  after pointer arithmetic  (i.e. GEP itself)
   
    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    
    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    int64_t tag = (int64_t)__spp_extract_tagval(ptr);     
    tag = tag + off;  
    
    uintptr_t tempval = ((uintptr_t)tag) << NUM_PTR_BITS; // | PM_PTR_SET;
    uintptr_t untagged = (uintptr_t)__spp_cleantag_direct(ptr);
    untagged = untagged & ~OVERFLOW_MASK; // remove previous overflow bit
    uintptr_t ptrval = untagged | tempval; 
    
    dbg(printf(">>%s new_ptr: %p\n", __func__, ptrval);)
	
    return (void*)ptrval;
}

__SPP_ATTR
void* 
__spp_update_check_clean(void *ptr, int64_t off) {
    stats(__spp_updatetag_cnt++;)
    stats(__spp_checkbound_cnt++;)
    
    if (!__spp_is_pm_ptr(ptr))
    {
        stats(__spp_vol_ptr_check_cnt++;)
        //if ptr is not tagged, return!
        stats(useless__spp_updatetag_cnt++;)
        stats(useless__spp_checkbound_cnt++;)
        return ptr; 
    }

    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    int64_t tag = (int64_t)__spp_extract_tagval(ptr);     
    tag = tag + off;  
    
    uintptr_t tempval = ((uintptr_t)tag) << NUM_PTR_BITS; // | PM_PTR_SET;
    tempval = tempval & OVERFLOW_MASK; // keep only the tag overflow bit
    uintptr_t untagged = (uintptr_t)__spp_cleantag_direct(ptr);	
    untagged = untagged & ~OVERFLOW_MASK; // remove previous overflow bit
    

#ifdef FAIL_OBL_COMP
    void* updatedPtr = (void*)(untagged | tempval);
    __spp_manual_checkbound(updatedPtr);
#endif

    return (void*)(untagged | tempval);
}

__SPP_ATTR
void* 
__spp_update_check_clean_direct(void *ptr, int64_t off) {
    stats(__spp_updatetag_cnt++;)
    stats(__spp_updatetag_cnt_direct++;)
    stats(__spp_checkbound_cnt++;)
    stats(__spp_checkbound_cnt_direct++;)

    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    int64_t tag = (int64_t)__spp_extract_tagval(ptr);   
    tag = tag + off;

    uintptr_t tempval = ((uintptr_t)tag) << NUM_PTR_BITS; // | PM_PTR_SET;
    tempval = tempval & OVERFLOW_MASK; // keep only the tag overflow bit
    uintptr_t untagged = (uintptr_t)__spp_cleantag_direct(ptr);	
    untagged = untagged & ~OVERFLOW_MASK; // remove previous overflow bit

#ifdef FAIL_OBL_COMP
    void* updatedPtr = (void*)(untagged | tempval);
    __spp_manual_checkbound(updatedPtr);
#endif

    return (void*)(untagged | tempval);
}


__SPP_ATTR
void* 
__spp_memintr_check_and_clean(void *ptr, int64_t off) {
    stats(__spp_memintr_check_and_clean_cnt++;)
    // ptr: pointer pass to LLVM memory intrinsic
    // off: offset to be added and checked
   
    // calculates the final address of the ptr according to the offset
    // performs bounds check
    // returns the clean ptr for the memory intrinsic function

    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    
    if (!__spp_is_pm_ptr(ptr))
    {
        stats(__spp_vol_ptr_check_cnt++;)
        //if ptr is not tagged, return!
        return ptr; 
    }
    
    int64_t tag = (int64_t)__spp_extract_tagval(ptr);   
    // -1 is applied due to addressing starting from 0   
    dbg(printf(">>%s with %p and offset %ld and tag %lx\n", __func__, ptr, off, tag);)
    tag = tag + (off - 1); 
    
    uintptr_t tempval = ((uintptr_t)tag) << NUM_PTR_BITS; // | PM_PTR_SET;
    tempval = tempval & OVERFLOW_MASK; // keep only the tag overflow bit
    uintptr_t untagged = (uintptr_t)__spp_cleantag_direct(ptr);
    untagged = untagged & ~OVERFLOW_MASK; // remove previous overflow bit
    
    dbg(printf(">>%s checked ptr: tag %p addr %p\n", __func__, tempval, untagged);)
    
#ifdef FAIL_OBL_COMP
    void* updatedPtr = (void*)(untagged | tempval);
    __spp_manual_checkbound(updatedPtr);
#endif
	
    return (void*)(untagged | tempval);
}

__SPP_ATTR
void* 
__spp_memintr_check_and_clean_direct(void *ptr, int64_t off) {
    stats(__spp_memintr_check_and_clean_cnt++;)
    stats(__spp_memintr_check_and_clean_cnt_direct++;)
    // ptr: pointer pass to LLVM memory intrinsic
    // off: offset to be added and checked
   
    // calculates the final address of the ptr according to the offset
    // performs bounds check
    // returns the clean ptr for the memory intrinsic function

    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
       
    int64_t tag = (int64_t)__spp_extract_tagval(ptr);   
    // -1 is applied due to addressing starting from 0   
    dbg(printf(">>%s with %p and offset %ld and tag %lx\n", __func__, ptr, off, tag);)
    tag = tag + (off - 1); 
    
    uintptr_t tempval = ((uintptr_t)tag) << NUM_PTR_BITS; // | PM_PTR_SET;
    tempval = tempval & OVERFLOW_MASK; // keep only the tag overflow bit
    uintptr_t untagged = (uintptr_t)__spp_cleantag_direct(ptr);
    untagged = untagged & ~OVERFLOW_MASK; // remove previous overflow bit
    
    dbg(printf(">>%s checked ptr: tag %p addr %p\n", __func__, tempval, untagged);)
    
#ifdef FAIL_OBL_COMP
    void* updatedPtr = (void*)(untagged | tempval);
    __spp_manual_checkbound(updatedPtr);
#endif
	
    return (void*)(untagged | tempval);
}

// it is not desirable that this func is called
// normally, it SHOULDN'T be called
__SPP_ATTR
void* 
__spp_update_pointer(void *ptr, int64_t off) {
    dbg(printf(">>%s with %p and offset %ld \n", __func__, ptr, off);)
    void *newptr = (void*)((int64_t)ptr + off);
    return __spp_updatetag(newptr, off);
}

#ifdef __cplusplus
}
#endif

