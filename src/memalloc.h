/** @file
*  Error-detecing memory allocators.
*
*  @author Piotr Styczyński <piotrsty1@gmail.com>
*  @copyright MIT
*  @date 2017-05-13
*/

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
extern void* AllocateMemoryBlock(int size);

/**
* Function allocating @p count block each of size @p size bytes.
*
* NOTICE: Assertion checking is done for allocation errors.
*
* @param[in] count : int
* @param[in] size : int
* @return void* to allocated memory array
*/
extern void* AllocateMemoryBlockArray(int count, int size);

/**
* Function deallocating data pointed by @p p
*
* NOTICE: Assertion checking is done for deallocation errors.
*
* @param[in] p : void*
*/
extern void FreeMemoryBlock(void* p);

#endif /* __STY_COMMON_MEMALLOC_H__ */
