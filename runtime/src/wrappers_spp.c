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

#define true  1
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

void __real_free(void *ptr);

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

extern void* __spp_cleantag_external(void *p);  
extern void* __spp_memintr_check_and_clean(void *ptr, int64_t off);

///////////////////////////////////////
///                                 /// 
///         Free definition         ///
///                                 /// 
///////////////////////////////////////

void __wrap_free (void *base)  
{
  dbg(printf("SPP: real free is interposed\n");)
  __real_free(__spp_cleantag_external(base)); 
}

/////////////////////////////////
///                           /// 
///      String functions     ///
///                           /// 
/////////////////////////////////

char* __real_strcpy(char *dest, char *src);
char* __real_strcat(char *dest, const char *src);
int __real_strcmp(char *str1, char *str2);
int __real_strncmp(char *str1, char *str2, size_t n);
char* __real_strncpy(char *dest,char *src,size_t n);
int __real_memcmp(void *str1, void *str2, size_t n);
void* __real_memchr(void *str, int c, size_t n);
char* __real_strchr(char *str, int c);
char* __real_strncat(char *dest, char *src, size_t n); 
long int __real_strtol(const char *str, char **endptr, int base); 
size_t __real_strlen(const char *str); 
void* __real_memcpy(void *dest, const void *src, size_t n);
void* __real_memset(void *str, int c, size_t n); 
void* __real_memmove(void *str1, const void *str2, size_t n);

int __real_snprintf(char *str, size_t size, const char *format, ...);

////////////////////////
///                  /// 
///      strlen      ///
///                  /// 
////////////////////////

size_t 
__wrap_strlen(const char *str)
{
  dbg(printf(">>%s at %p\n", __func__, str);)
  return __real_strlen((const char*)__spp_cleantag_external((void*)str));
}

////////////////////////
///                  /// 
///      strcpy      ///
///                  /// 
////////////////////////

char*
__wrap_strcpy(char *dest, char *src)
{
  printf(">>%s\n",__func__);
  dbg(printf(">>%s dest:%p src:%p\n", __func__, dest, src);)
     
  // 1. get objsize (dest) strlen(src) 
  // 2. check dest + strlen(str) (+1?) < dest_end
  // then return untagged_dest. 
  // (untagging src is handled in untag hook)
   
  __real_strcpy((char*)__spp_memintr_check_and_clean(dest, strlen(src)), 
                  (char*)__spp_memintr_check_and_clean(src, strlen(src)));

  assert_dbg(assert((uintptr_t)temp==(uintptr_t)dest);)

  return dest;
}

////////////////////////
///                  /// 
///      strcmp      ///
///                  /// 
////////////////////////

int 
__wrap_strcmp(char *str1, char *str2)
{
  printf(">>%s\n",__func__);
  dbg(printf(">>%s str1:%p str2:%p\n", __func__, str1, str1);)
   
  return __real_strcmp((char*)__spp_memintr_check_and_clean(str1, strlen(str1) + 1), 
                       (char*)__spp_memintr_check_and_clean(str2, strlen(str2) + 1)); 
}

////////////////////////
///                  /// 
///     strncmp      ///
///                  /// 
////////////////////////

int 
__wrap_strncmp(char *str1, char *str2, size_t n)
{
  printf(">>%s\n",__func__);
  dbg(printf(">>%s str1:%p str2:%p size:%ld\n", __func__, str1, str1, n);)
    
  return __real_strncmp((char*)__spp_memintr_check_and_clean((void*)str1, n), 
                       (char*)__spp_memintr_check_and_clean((void*)str2, n), n); 
}

////////////////////////
///                  /// 
///     strncpy      ///
///                  /// 
////////////////////////


char*
__wrap_strncpy(char *dest, char *src, size_t n)
{
  printf(">>%s\n",__func__);
  dbg(printf(">>%s dest:%p src:%p size:%ld\n", __func__, dest, src, n);)

  __real_strncpy((char*)__spp_memintr_check_and_clean((void*)dest, n), 
                  (char*)__spp_memintr_check_and_clean((void*)src, n), n);
  
  return dest;
}

////////////////////////
///                  /// 
///      memcmp      ///
///                  /// 
////////////////////////

int 
__wrap_memcmp(void *str1, void *str2, size_t n)
{
  printf(">>%s\n",__func__);
  dbg(printf(">>%s str1:%p str2:%p size:%ld\n", __func__, str1, str1, n);)
  return __real_memcmp((void*)__spp_memintr_check_and_clean(str1, n), 
                        (void*)__spp_memintr_check_and_clean(str2, n), n);
}

////////////////////////
///                  /// 
///      memchr      ///
///                  /// 
////////////////////////

void*
__wrap_memchr(void *str, int c, size_t n) 
{
  printf(">>%s\n",__func__);
  dbg(printf(">>%s str:%p c:%d size:%ld\n", __func__, str, c, n);)

  void *result = __real_memchr(
                (void*)__spp_memintr_check_and_clean((void*)str , n),
                c, n); 
    
  if (result != NULL)
  {
    uintptr_t tag = (uintptr_t)str & (((uintptr_t)(~0))<<NUM_USED_BITS); 
    result= (char*)(tag | (uintptr_t)result); 
  }

  return result; 
}

////////////////////////
///                  /// 
///      strchr      ///
///                  /// 
////////////////////////

char* 
__wrap_strchr(char *str, int c)
{
  printf(">>%s\n",__func__);
  dbg(printf(">>%s str:%p c:%d\n", __func__, str, c);)
  
  char *result = __real_strchr((char*)__spp_cleantag_external(str),c);

  if (result!=NULL)
  {
    uintptr_t tag = (uintptr_t)str & (((uintptr_t)(~0))<<NUM_USED_BITS); 
    result = (char*)(tag | (uintptr_t)result); 
  }
  
  return result; 
}

////////////////////////
///                  /// 
///      strcat      ///
///                  /// 
////////////////////////

char* 
__wrap_strcat(char *dest, const char *src)
{
  printf(">>%s\n",__func__);
  dbg(printf(">>%s dest:%p %ld src:%p %ld\n", __func__, dest, strlen(dest), src, strlen(src));)
  
  __real_strcat((char*)__spp_memintr_check_and_clean((void*)dest, strlen(dest) + strlen(src) + 1),
                  (char*)__spp_memintr_check_and_clean((void*)src, strlen(src) + 1));
    
  return dest; 
}

////////////////////////
///                  /// 
///     strncat      ///
///                  /// 
////////////////////////

char* 
__wrap_strncat(char *dest, char *src, size_t n)
{
  printf(">>%s\n",__func__);
  dbg(printf(">>%s dest:%p src:%p size:%ld\n", __func__, dest, src, n);)
  
  __real_strncat((char*)__spp_memintr_check_and_clean((void*)dest, strlen(dest) + n + 1),
                  (char*)__spp_memintr_check_and_clean((void*)src, n), n);
    
  return dest; 
}

////////////////////////
///                  /// 
///      strtol      ///
///                  /// 
////////////////////////

//Dimitris: unsafe for now
long int 
__wrap_strtol (char *str, char **endptr, int base)
{
    printf(">>%s\n",__func__);
    dbg(printf(">>%s str:%p endptr:%p base:%d\n", __func__, str, endptr, base);)

    return __real_strtol((char*)__spp_cleantag_external(str), 
                      (char**)__spp_cleantag_external(endptr), base);
}

////////////////////////
///                  /// 
///      memcpy      ///
///                  /// 
////////////////////////

void* 
__wrap_memcpy (void *dest, const void *src, size_t n)
{
  printf(">>%s\n",__func__);
  dbg(printf(">>%s dest:%p src:%p size:%ld\n", __func__, dest, src, n);)

  return __real_memcpy((void*)__spp_memintr_check_and_clean(dest, n), 
                    (const void*)__spp_memintr_check_and_clean((void*)src, n), n);
}

////////////////////////
///                  /// 
///      memset      ///
///                  /// 
////////////////////////

void* 
__wrap_memset(void *str, int c, size_t n)
{
  printf(">>%s\n",__func__);
  dbg(printf(">>%s str:%p c:%d size:%ld\n", __func__, str, c, n);)

  return __real_memset((void*)__spp_memintr_check_and_clean(str, n), c, n);
} 

////////////////////////
///                  /// 
///      memmove     ///
///                  /// 
////////////////////////

void* 
__wrap_memmove(void *str1, const void *str2, size_t n)
{
  printf(">>%s\n",__func__);
  dbg(printf(">>%s str1:%p str2:%p size:%ld\n", __func__, str1, str2, n);)

  return __real_memmove((void*)__spp_memintr_check_and_clean(str1, n), 
                  (void*)__spp_memintr_check_and_clean((void*)str2, n), n);
} 

#ifdef __cplusplus
} // extern "C"
#endif

////////////////////////
///                  /// 
///     snprintf     ///
///                  /// 
////////////////////////
int __wrap_snprintf(char *str, size_t size, const char *format, ...)
{
  printf(">>%s\n",__func__);
  dbg(printf(">>%s str:%p size:%ld\n", __func__, str, size);)

  return __real_snprintf((char*)__spp_memintr_check_and_clean(str, size), 
                          size, format);
}