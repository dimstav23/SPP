//#define CPP

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "./spp.h"

#define PERFORM_ONLY_TAG_CLEANING_AT_MEM_ACCESS

// if defined above, performing only tag cleaning.

/////////////////////////////////
///                           /// 
///     Debugging setting     ///
///                           /// 
/////////////////////////////////

//#define DEBUG
#ifdef DEBUG
#  define dbg(x) x
#else
#  define dbg(x) 
#endif

//#define ASSERT_DEBUG
#ifdef ASSERT_DEBUG
#  define assert_dbg(x) x
#else
#  define assert_dbg(x) 
#endif


#define MIU_Bool_ unsigned char 
#define true   1
#define false 0


//////////////////////////////////////
///                                /// 
///      Interposing setting       ///
///                                /// 
//////////////////////////////////////

//////////////////////////////
///                        /// 
///      free setting      ///
///                        /// 
//////////////////////////////

void 
__real_free (void *ptr);

//////////////////////////////
///                        /// 
///     Alloc setting      ///
///                        /// 
//////////////////////////////

//void * __real_malloc(size_t size);
//void * __real_realloc(void *ptr, size_t size);
//void * __real_calloc(size_t elems, size_t elemsize);

///////////////////////////////////////
///                                 /// 
///     SPP func declarations       ///
///                                 /// 
///////////////////////////////////////

extern void* __spp_cleantag_external (void * p);  

///////////////////////////////////////
///                                 /// 
///         Free definition         ///
///                                 /// 
///////////////////////////////////////

/*
void* 
__wrap_malloc (size_t sz)  
{
    void * ptr= __real_malloc (sz);
     
    uintptr_t tag= ((uintptr_t)sz)<<NUM_USED_BITS;
    //dbg(printf("malloc_tag: 0x%.16" PRIXPTR ", size: %lu\n", tag, sz);) 
     
    uintptr_t tagged= (((uintptr_t)ptr) | tag);
    dbg(printf("spp_malloc. ptr: 0x%.16" PRIXPTR ", tagged: 0x%.16" PRIXPTR ", sz: %lu\n", (uintptr_t)ptr, tagged, sz);) 
    
    return (void*)tagged;
}
*/

void 
__wrap_free (void* base)  
{
    dbg(printf("SPP: real free is interposed\n");)
    __real_free(__spp_cleantag_external(base)); 
}

/////////////////////////////////
///                           /// 
///      String function      ///
///                           /// 
/////////////////////////////////

char * __real_strcpy (char * dest, char *src);
int __real_strcmp   (char * str1, char * str2);
int __real_strncmp  (char *str1, char *str2, size_t n);
char *__real_strncpy    (char * dest,char * src,size_t n);
int __real_memcmp       (void *str1, void *str2, size_t n);
void * __real_memchr    (void *str, int c, size_t n);
char * __real_strchr    (char *str, int c);
char * __real_strncat   (char *dest, char *src, size_t n); 
long int __real_strtol  (const char *str, char **endptr, int base); 
size_t __real_strlen  (const char *str); 
void * __real_memcpy    (void *dest, const void *src, size_t n);
char * __real_memset   (void *str, int c, size_t n); 


////////////////////////
///                  /// 
///      strlen      ///
///                  /// 
////////////////////////

size_t 
__wrap_strlen(const char * str)
{
  dbg(printf("__wrap_strlen at %p\n", str);)
  
  return __real_strlen((const char*)__spp_cleantag_external((void*)str));
}

////////////////////////
///                  /// 
///      strcpy      ///
///                  /// 
////////////////////////

char *
__wrap_strcpy(char * dest, char * src)
{
    char * temp= dest; 
    
    // 1. get objsize (dest) strlen(src) 
    // 2. check dest + strlen(str) (+1?) < dest_end
    // then return untagged_dest. 
    // (untagging src is handled in untag hook)

    dbg(printf("__wrap_strcpy at d: %p, s: %p\n", dest, src);)
    dbg(printf("\tsrc: %s\n", src);)

    // Preventive check for avoiding buffer overflows
    void* src_tagged = __spp_updatetag((void*)src, strlen(src));
    void* dest_tagged = __spp_updatetag((void*)dest, strlen(src));
    __spp_checkbound(src_tagged);
    __spp_checkbound(dest_tagged);

    // strlen is hooked at link time at the moment, 
    // so an arg of this strlen is already tag-free. 
  
  #if defined(ENABLE_SPACEMIU) || defined(PERFORM_ONLY_TAG_CLEANING_AT_MEM_ACCESS)
    
    __real_strcpy ((char*)__spp_cleantag_external(dest), 
                   (char*)__spp_cleantag_external(src));
  
  #else  
    
    __real_strcpy((char*)MIU_check_mem_access(dest,strlen(src)), 
                  (char*)__spp_cleantag_external(src));
    
  
  #endif  // ENABLE_SPACEMIU, PERFORM_ONLY_TAG_CLEANING_AT_MEM_ACCESS
 
  assert((uintptr_t)temp==(uintptr_t)dest);

  return dest;

}

////////////////////////
///                  /// 
///      strcmp      ///
///                  /// 
////////////////////////


int 
__wrap_strcmp(char * str1, char * str2)
{
    char * untagstr1= (char*)__spp_cleantag_external(str1); 
    char * untagstr2= (char*)__spp_cleantag_external(str2); 
    
    return __real_strcmp ((char*)__spp_cleantag_external(str1), 
                         (char*)__spp_cleantag_external(str2)); 
}


////////////////////////
///                  /// 
///     strncmp      ///
///                  /// 
////////////////////////

int 
__wrap_strncmp(char *str1, char *str2, size_t n)
{

  #if defined(ENABLE_SPACEMIU) || defined(PERFORM_ONLY_TAG_CLEANING_AT_MEM_ACCESS)
    
    char * untagstr1= (char*)__spp_cleantag_external(str1);
    char * untagstr2= (char*)__spp_cleantag_external(str2);

  #else
     
    char * untagstr1= (char*)MIU_check_mem_access(str1, n);
    char * untagstr2= (char*)MIU_check_mem_access(str2, n);

  #endif   

    return __real_strncmp(untagstr1, untagstr2, n);
}

////////////////////////
///                  /// 
///     strncpy      ///
///                  /// 
////////////////////////


char *
__wrap_strncpy(char * dest, char * src,size_t n)
{

  char * temp= dest;

  #if defined(ENABLE_SPACEMIU) || defined(PERFORM_ONLY_TAG_CLEANING_AT_MEM_ACCESS)
    
   __real_strncpy ((char*)__spp_cleantag_external((void*)dest),
                    (char*)__spp_cleantag_external((void*)src),
                     n); 

  #else

   __real_strncpy((char*)MIU_check_mem_access((void*)dest,n),
                   (char*)MIU_check_mem_access((void*)src,n), 
                    n); 
  
  #endif
  
  return temp; 
    
}

////////////////////////
///                  /// 
///      memcmp      ///
///                  /// 
////////////////////////

int 
__wrap_memcmp(void *str1, void *str2, size_t n)
{
    dbg(printf("__wrap_memcmp/ p1: 0x%.16" PRIXPTR ", p2: 0x%.16" PRIXPTR ", sz: %lu\n", (uintptr_t)str1, (uintptr_t)str2, n);)
    return __real_memcmp ((void*)__spp_cleantag_external(str1), 
                          (void*)__spp_cleantag_external(str2),n);
}

////////////////////////
///                  /// 
///      memchr      ///
///                  /// 
////////////////////////

void *
__wrap_memchr(void *str, int c, size_t n) 
{
  
  #if defined(ENABLE_SPACEMIU) || defined(PERFORM_ONLY_TAG_CLEANING_AT_MEM_ACCESS)
    
    void * result= __real_memchr((void*)__spp_cleantag_external(str),c,n); 
  
  #else 
    
    void * result= __real_memchr(MIU_check_mem_access(str,n),c,n); 
  
  #endif
  
    if (result != NULL){
        uintptr_t tag= (uintptr_t)str & (((uintptr_t)(~0))<<NUM_USED_BITS); 
        result= (char*)(tag|(uintptr_t)result); 
    }
    dbg(printf("__wrap_memchr.\nstr: 0x%.16" PRIXPTR "\n", (uintptr_t)str);)
    dbg(printf("\tresult: 0x%.16" PRIXPTR "\n", (uintptr_t)result);)
    return result; 
}

////////////////////////
///                  /// 
///      strchr      ///
///                  /// 
////////////////////////

char * 
__wrap_strchr (char *str, int c)
{
    char* result= __real_strchr((char*)__spp_cleantag_external(str),c);

    if (result!=NULL){
        uintptr_t tag= (uintptr_t)str & (((uintptr_t)(~0))<<NUM_USED_BITS); 
        result= (char*)(tag|(uintptr_t)result); 
    }
    dbg(printf("__wrap_strchr.\nstr: 0x%.16" PRIXPTR "\n", (uintptr_t)str);)
    dbg(printf("\tresult: 0x%.16" PRIXPTR "\n", (uintptr_t)result);)
  
  return result; 

}

////////////////////////
///                  /// 
///     strncat      ///
///                  /// 
////////////////////////


char * 
__wrap_strncat (char *dest, char *src, size_t n)
{

  char * temp= dest;

  #if defined(ENABLE_SPACEMIU) || defined(PERFORM_ONLY_TAG_CLEANING_AT_MEM_ACCESS)
    
    __real_strncat((char*)__spp_cleantag_external(dest),
                    (char*)__spp_cleantag_external(src), n);
  
  #else 

    size_t srclen= strlen(src);  
    size_t destlen= strlen(dest);  
    
    assert_dbg(assert((srclen<n)
                &&"MIU ERROR at strncat: strlen <= n\n");) 
    assert(srclen<n);
    
    if (srclen>=n){
      
      #ifndef MEASURE_RUNTIME
        exit(0);
      #else
        asm("nop");
      #endif
    }
    __real_strncat ((char*)MIU_check_mem_access(dest,destlen+srclen),
                    (char*)__spp_cleantag_external(src), n);

  #endif // ENABLE_SPACEMIU || PErform only tagging
 
  assert((uintptr_t)dest == (uintptr_t)temp); 
  
  dbg(printf("__wrap_strncat.\ndest: 0x%.16" PRIXPTR "\n", (uintptr_t)dest);)
  dbg(printf("\tsrc: 0x%.16" PRIXPTR "\n", (uintptr_t)src);)
  
  return dest; 

}

////////////////////////
///                  /// 
///      strtol      ///
///                  /// 
////////////////////////


long int 
__wrap_strtol (char *str, char **endptr, int base)
{
    return __real_strtol((char*)__spp_cleantag_external(str), 
                      (char**)__spp_cleantag_external(endptr), 
                      base);
}


////////////////////////
///                  /// 
///      memcpy      ///
///                  /// 
////////////////////////


void * 
__wrap_memcpy (void *dest, const void *src, size_t n)
{
  
  dbg(printf("__wrap_memcpy.\nstr: %lx\n", (uintptr_t)dest);)
  dbg(printf("dest after cleaning: %lx\n", (uintptr_t)__spp_cleantag_external(dest));)

  return __real_memcpy((void*)__spp_cleantag_external(dest), 
                    (const void*)__spp_cleantag_external(src),
                    n);
}

////////////////////////
///                  /// 
///      memset      ///
///                  /// 
////////////////////////


char * 
__wrap_memset (void *str, int c, size_t n)
{
  
  dbg(printf("__wrap_memset.\nstr: %lx\n", (uintptr_t)str);)
  dbg(printf("str after cleaning: %lx\n", (uintptr_t)__spp_cleantag_external(str));)

  return __real_memset((void*)__spp_cleantag_external(str), 
                      c,
                      n);
} 

#ifdef __cplusplus
} // extern "C"
#endif
