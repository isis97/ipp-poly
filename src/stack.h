/** @file
*  Stack implementation (C99 standard)
*  Usage:
*  @code
*     #include <stack.h>
*      ...
*     Stack l = StackNew();
*  @endcode
*
*
*  @author Piotr Styczyński <piotrsty1@gmail.com>
*  @copyright MIT
*  @date 2017-05-13
*/
#include "utils.h"
#include <stdio.h>
#include "memalloc.h"
#include "array_lists.h"
#include "dynamic_lists.h"

#ifndef __STY_COMMON_STACK_H__
#define __STY_COMMON_STACK_H__

/** Use default stack array lists implementation **/
#ifndef STACK_IMPL
  /**
   * @def STACK_IMPL
   * Define default stack implementation
   */
  #define STACK_IMPL ARRAY_LISTS
#endif

/** Support for dynamic_lists and array_lists based stacks **/
#if STACK_IMPL == ARRAY_LISTS
  /** Use the prefixed method/type invocation for stack abstract impl **/
  #define STACK_IMPL_ARRAY(CALL) ArrayList##CALL
#elif STACK_IMPL == DYNAMIC_LISTS
  /** Use the prefixed method/type invocation for stack abstract impl **/
  #define STACK_IMPL_ARRAY(CALL) List##CALL
#else
  /**
   * @def STACK_IMPL_ARRAY
   *
   * Macro to be used to refer to implementation details of stack.
   * It depends on selected STACK_IMPL.
   * This macro redirects to implementation methods/variables/types.
   *
   * @param[in] CALL : Name of the entry you refer to
   */
  #define STACK_IMPL_ARRAY(CALL) ArrayList##CALL
#endif




/** Data type held by stack */
typedef struct Stack Stack;

/**
* Data holder for stack
* @return StackData extracted from the datatype of implementation
*/
typedef STACK_IMPL_ARRAY(Data) StackData;

/**
* Function allocating/deallocating space for stack elements
*
* If the function is called as deallocator then it should
* free given @p data.
* Then should return NULL.
*
* If as allocator then it should allocate new StackData.
* Assign a value of @p data to it.
* And return it.
*
* @param[in] data : the data prototype
* @return Allocated stack data
*/
typedef StackData (*StackAllocator)(StackData data);

/**
* Root element of the Stack containing pointers
* to the two ends of a Stack
*/
struct Stack {
  STACK_IMPL_ARRAY() data; ///< List providing the stack-like functionality
};

/**
* Create new Stack
* All Stacks must be then freed with Stacks.free(Stack).
*
* @return Stack
**/
static inline Stack StackNew() {
  return (Stack) {
    .data = STACK_IMPL_ARRAY(New)()
  };
}

/**
* Destroy given Stack freeing up memory.
*
* @param[in] s : Stack*
*/
static inline void StackDestroy(Stack* s) {
  if(s == NULL) return;
  STACK_IMPL_ARRAY(Destroy)(&(s->data));
}

/**
* Destroy given Stack freeing up memory.
*
*
* @param[in] s           : Stack*
* @param[in] deallocator : StackAllocator
*/
static inline void StackDestroyDeep(Stack* s, StackAllocator deallocator) {
  if(s == NULL) return;
  STACK_IMPL_ARRAY(DestroyDeep)(&(s->data), deallocator);
}


/**
* Push @p element to the end of a given Stack.
* Method returns pointer to the newly created Stack node.
*
* @param[in] s       : Stack*
* @param[in] element : StackData
* @return StackIterator
*/
static inline void StackPush( Stack* s, StackData element ) {
  if(s == NULL) return;
  STACK_IMPL_ARRAY(PushBack)(&(s->data), element);
}

/**
* Removes last element of the given Stack or does nothing if it's empty.
* Returns data pointer held by the removed element.
* If no element was removed (Stack is empty) NULL is returned.
*
* @param[in] s : Stack*
* @return StackData (popped element)
*/
static inline StackData StackPop( Stack* s ) {
  if(s == NULL) return NULL;
  return (StackData) STACK_IMPL_ARRAY(PopBack)(&(s->data));
}

/**
* Clears the entire Stack.
*
*
* @param[in] s : Stack*
*/
static inline void StackClear( Stack* s ) {
  if(s == NULL) return;
  STACK_IMPL_ARRAY(Clear)(&(s->data));
}

/**
* Obtain last element of the Stack.
* Function return StackData pointer to the data under first element.
*
* If the Stack is empty NULL is returned.
*
* @param[in] s : const Stack*
* @return last element if exists
*/
static inline StackData StackFirst( const Stack* s ) {
  if(s == NULL) return NULL;
  return (StackData) STACK_IMPL_ARRAY(Last)(&(s->data));
}

/**
* Obtain the size of a Stack.
*
*
* @param[in] s : const Stack*
* @return size of the Stack
*/
static inline int StackSize( const Stack* s ) {
  if(s == NULL) return 0;
  return STACK_IMPL_ARRAY(Size)(&(s->data));
}

/**
* Check's if Stack is empty
*
* @param[in] s : const Stack*
* @return If Stack is empty?
*/
static inline int StackEmpty( const Stack* s ) {
  if(s == NULL) return 1;
  return STACK_IMPL_ARRAY(Empty)(&(s->data));
}

/**
* Copy the Stack into a new one.
*
* WARN: Each element will be a new one, but the data
*       pointers will be still pointing to the same
*       memory locations (there're still the same
*       objects under StackData pointers)
*
* @param[in] s : const Stack*
* @return shallow copy of a given Stack
*/
static inline Stack StackCopy( const Stack* s ) {
  if(s == NULL) return StackNew();
  return (Stack){
    .data = STACK_IMPL_ARRAY(Copy)(&(s->data))
  };
}

/**
* Performs deep copy of the Stack returning new one.
* The given (StackData)->(StackData) function is used as assigner.
* The function should create new element value, copy the value of
* the given one and return pointer to this element.
*
* @param[in] s                : const Stack*
* @param[in] elementAllocator : StackModifierFn
* @return deep copy of a given Stack
*/
static inline Stack StackDeepCopy( const Stack* s, StackAllocator elementAllocator ) {
  if(s == NULL) return StackNew();
  return (Stack) {
    .data = STACK_IMPL_ARRAY(DeepCopy)(&(s->data), elementAllocator)
  };
}

/**
* Copy the Stack @p source into other Stack -  @p target
*
* WARN: Each element will be a new one, but the data
*       pointers will be still pointing to the same
*       memory locations (there're still the same
*       objects under StackData pointers)
*
* @param[in] source : const Stack*
* @param[in] target : Stack*
*/
static inline void StackCopyInto( const Stack* source, Stack* target ) {
  if(source == NULL || target == NULL) return;
  STACK_IMPL_ARRAY(CopyInto)(&(source->data), &(target->data));
}


/**
* Print given ArrayList to stdout.
* Prints only adresses of values not exact values.
*
* @param[in] s       : const ArrayList*
* @param[in] printer : ArrayListModifierFn
*/
static inline void StackPrint( const Stack* s, GenericsPrinter printer ) {
  if(s == NULL) {
    printf("NULL");
    return;
  }
  STACK_IMPL_ARRAY(Print)(&(s->data), printer);
}

/**
* Print given ArrayList to stdout.
* Prints only adresses of values not exact values.
* Variant displaying new line at the end of stringified ArrayList.
*
* @param[in] s       : const ArrayList*
* @param[in] printer : ArrayListModifierFn
*/
static inline void StackPrintln( const Stack* s, GenericsPrinter printer ) {
  if(s == NULL) {
    printf("NULL\n");
    return;
  }
  STACK_IMPL_ARRAY(Println)(&(s->data), printer);
}

/**
* Print given ArrayList to stdout.
* Prints only adresses of values not exact values.
*
* @param[in] s : const ArrayList*
*/
static inline void StackPrintData( const Stack* s ) {
  if(s == NULL) {
    printf("NULL");
    return;
  }
  STACK_IMPL_ARRAY(PrintData)(&(s->data));
}

/**
* Print given ArrayList to stdout.
* Prints only adresses of values not exact values.
* Variant displaying new line at the end of stringified ArrayList.
*
* @param[in] s : const ArrayList*
*/
static inline void StackPrintlnData( const Stack* s ) {
  if(s == NULL) {
    printf("NULL");
    return;
  }
  STACK_IMPL_ARRAY(PrintData)(&(s->data));
  printf("\n");
}

#endif /* __STY_COMMON_STACK_H__ */
