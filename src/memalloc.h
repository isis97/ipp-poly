/** @file
*  Error-detecing memory allocators.
*
*  @author Piotr Styczyński <piotrsty1@gmail.com>
*  @copyright MIT
*  @date 2017-05-13
*/

#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#ifndef __STY_COMMON_MEMALLOC_H__
#define __STY_COMMON_MEMALLOC_H__

/**
* @def MALLOCATE(STRUCT)
*
* Macro giving value of pointer to the allocated structure @p STRUCT
*
* NOTICE: Assertion checking is done for allocation errors.
*/
#define MALLOCATE(STRUCT) \
( (STRUCT*) AllocateMemoryBlock(sizeof(STRUCT)) )

/**
* @def MALLOCATE_ARRAY(STRUCT, LEN)
*
* Macro giving value of pointer to the allocated array of size @p LEN
* of structures @p STRUCT
*
* NOTICE: Assertion checking is done for allocation errors.
*/
#define MALLOCATE_ARRAY(STRUCT, LEN) \
( (STRUCT*) AllocateMemoryBlockArray((LEN), sizeof(STRUCT)) )

/**
* @def MFREE(POINTER)
*
* Macro freeing data pointed by @p POINTER
*
* NOTICE: Assertion checking is done for deallocation errors.
*/
#define MFREE(POINTER) \
FreeMemoryBlock((POINTER));


/**
* Function allocating @p size bytes.
*
* NOTICE: Assertion checking is done for allocation errors.
*
* @param[in] size : int
* @return void* to allocated memory block
*/
static inline void* AllocateMemoryBlock(int size) {
  assert(size > 0);

  int old_errno = errno;
  errno = 0;
  void* data = malloc(size);
  assert(errno == 0);
  assert(data != NULL);
  errno = old_errno;

  return data;
}

/**
* Function allocating @p count block each of size @p size bytes.
*
* NOTICE: Assertion checking is done for allocation errors.
*
* @param[in] count : int
* @param[in] size : int
* @return void* to allocated memory array
*/
static inline void* AllocateMemoryBlockArray(int count, int size) {
  assert(count > 0);
  assert(size > 0);

  int old_errno = errno;
  errno = 0;
  void* data = calloc(count, size);
  assert(errno == 0);
  assert(data != NULL);
  errno = old_errno;

  return data;
}

/**
* Function deallocating data pointed by @p p
*
* NOTICE: Assertion checking is done for deallocation errors.
*
* @param[in] p : void*
*/
static inline void FreeMemoryBlock(void* p) {
  assert(p != NULL);
  int old_errno = errno;
  errno = 0;
  free(p);
  assert(errno == 0);
  errno = old_errno;
}

#endif /* __STY_COMMON_MEMALLOC_H__ */
