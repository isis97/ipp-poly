/** @file
*  Bidirectional list implementation (C99 standard)
*  Usage:
*  @code
*     #include <list.h>
*      ...
*     list l = Lists.new();
*  @endcode
*
*  All interface sould be accessed through Lists constant.
*
*  @author Piotr Styczyński <piotrsty1@gmail.com>
*  @copyright MIT
*  @date 2017-05-13
*/
#include "utils.h"

#ifndef __STY_COMMON_LIST_H__
#define __STY_COMMON_LIST_H__

/**
* @def loop_list(LIST, VAR_NAME)
* Macro for interating through list
*
* Usage:
*
* @code
*   loop_list(list_object, i) {
*         void* element = Lists.value(i);
*         printf("void_ptr = %p\n", i);
*    }
* @endcode
*
* @param[in] LIST : list name to be iterated
* @param[in] VAR_NAME : name of iterator variable
*/
#define loop_list(LIST, VAR_NAME) \
  for(listIterator VAR_NAME = Lists.begin(LIST); VAR_NAME != NULL; VAR_NAME=Lists.next(VAR_NAME))

/*
* Declare data types needed for lists implementation
*/

/** Lists interface */
typedef struct lists lists;

/** Type of lists nodes (elements) */
typedef struct listNode listNode;

/** Type of list root element */
typedef struct listRoot listRoot;

/**
* Actual type of list - syntax sugar for writing pointers everywhere
*/
typedef listRoot list;

/**
* Root element of the list containing pointers
* to the two ends of a list
*/
struct listRoot {
  listNode* begin; ///< pointer to the begining of the list
  listNode* end; ///< pointer to the end of the list
};

/**
* List iterator
*/
typedef listNode* listIterator;

/**
* Function of type
* void* -> void*
* Used as list data manipulators/iterators
*/
typedef void* (*listModifierFn)(void*);

/**
* Interface for lists
*/
struct lists {

  /**
  * Create new list
  * All lists must be then freed with Lists.free(list).
  *
  * @return list
  */
  list (*new)( );

  /**
  * Destroy given list freeing up memory.
  *
  * WARN: Invalidates listIterators
  *
  * @param[in] l : list*
  */
  void (*free)( list* );

  /**
  * Push @p element to the front of a given list.
  * Method returns pointer to the newly created list node.
  *
  * NOTICE: All listIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param[in] l : list*
  * @param[in] element : void*
  * @return listIterator
  */
  listIterator (*pushFront)( list* l, void* element );

  /**
  * Push @p element to the end of a given list.
  * Method returns pointer to the newly created list node.
  *
  * NOTICE: All listIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param[in] l : list*
  * @param[in] element : void*
  * @return listIterator
  */
  listIterator (*pushBack)( list* l, void* element );

  /**
  * Removes first element of the given list or does nothing if it's empty.
  * Returns data pointer held by the removed element.
  * If no element was removed (list is empty) NULL is returned.
  *
  * WARN: Invalidates listIterators when elment under pointers
  *       will be popped.
  *
  * @param[in] l : list*
  * @return void* (popped element)
  */
  void* (*popFront)( list* l );

  /**
  * Removes last element of the given list or does nothing if it's empty.
  * Returns data pointer held by the removed element.
  * If no element was removed (list is empty) NULL is returned.
  *
  * WARN: Invalidates listIterators when elment under pointers
  *       will be popped.
  *
  * @param[in] l : list*
  * @return void* (popped element)
  */
  void* (*popBack)( list* l );

  /**
  * Clears the entire list.
  *
  * WARN: Invalidates listIterators for all elements of list
  *
  * @param[in] l : list*
  */
  void (*clear)( list* l );

  /**
  * Obtain first element of the list.
  * Function return void* pointer to the data under first element.
  *
  * If the list is empty NULL is returned.
  *
  * @param[in] l : const list*
  * @return first element if exists
  */
  void* (*first)( const list* l );

  /**
  * Obtain last element of the list.
  * Function return void* pointer to the data under first element.
  *
  * If the list is empty NULL is returned.
  *
  * @param[in] l : const list*
  * @return last element if exists
  */
  void* (*last)( const list* l );

  /**
  * Obtain the size of a list.
  *
  * WARN: Works in O(n) time where n is the length of the list
  *
  * @param[in] l : const list*
  * @return size of the list
  */
  int (*size)( const list* l );

  /**
  * Check's if list is empty
  *
  * @param[in] l : const list*
  * @return If list is empty?
  */
  int (*empty)( const list* l );

  /**
  * Copy the list into a new one.
  *
  * WARN: Each element will be a new one, but the data
  *       pointers will be still pointing to the same
  *       memory locations (there're still the same
  *       objects under void* pointers)
  *
  * @param[in] l : const list*
  * @return shallow copy of a given list
  */
  list (*copy)( const list* l );

  /**
  * Performs deep copy of the list returning new one.
  * The given (void*)->(void*) function is used as assigner.
  * The function should create new element value, copy the value of
  * the given one and return pointer to this element.
  *
  * @param[in] l : const list*
  * @param[in] elementAllocator : listModifierFn
  * @return deep copy of a given list
  */
  list (*deepCopy)( const list* l, listModifierFn assigner );

  /**
  * Copy the list @p source into other list -  @p target
  *
  * WARN: Each element will be a new one, but the data
  *       pointers will be still pointing to the same
  *       memory locations (there're still the same
  *       objects under void* pointers)
  *
  * @param[in] source : const list*
  * @param[in] target : list*
  */
  void (*copyInto)( const list* source, list* target );

  /**
  * Iterate through list using given
  * (void*)->(void*) function.
  * Function is executed for every list element value
  * The return value is ignored.
  *
  * @param[in] l : const list*
  * @param[in] mappingFunction : listModifierFn
  */
  void (*iterate)( const list* l, listModifierFn iterator );

  /**
  * Map list values using given
  * (void*)->void function
  * Function is executed for every list element value
  * Then the result of function is assigned to the
  * element's data pointer.
  *
  * NOTICE: Mapping is made in place.
  *
  * @param[in] l : const list*
  * @param[in] mappingFunction : listModifierFn
  */
  void (*map)( list* l, listModifierFn mapping );

  /**
  * Print given list to stdout.
  * Prints only adresses of values not exact values.
  *
  * @param[in] l : const list*
  */
  void (*print)( const list* );

  /**
  * Print given list to stdout.
  * Prints only adresses of values not exact values.
  * Variant displaying new line at the end of stringified list.
  *
  * @param[in] l : const list*
  */
  void (*println)( const list* l );

  /**
  * Get the first element container pointer.
  * If the list is empty then NULL is returned.
  *
  * NOTICE: All listIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param[in] l : const list*
  * @return listIterator pointing to the list begining
  */
  listIterator (*begin)( const list* l );

  /**
  * Get the last element container pointer.
  * If the list is empty then NULL is returned.
  *
  * NOTICE: All listIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param[in] l : const list*
  * @return listIterator pointing to the list end
  */
  listIterator (*end)( const list* l );

  /**
  * Removes element from the list using given container pointer.
  * The list parameter MUST BE NON NULL for nodes that are first or last
  * (isSideElement return true)
  * For all other situations it may be NULL
  *
  * WARN: Invalidates pointers to the removed elements.
  *
  * @param[in] l : list*
  * @param[in] node : listIterator
  */
  void (*detachElement)( list* l, listIterator node );

  /**
  * Create node that is not attached to anything.
  * This functionality may be used in situations when you need
  * list nodes outside actual list.
  *
  * NOTICE: All listIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @return listIterator to that not-attached node
  */
  listIterator (*newDetachedElement)( );

  /**
  * Checks if given node is the last or first element.
  *
  * NOTICE: For detached nodes this function returns true
  *         Formally detached nodes are simultaniously
  *         on the left and right side of the list as they have no
  *         neightbours on any side.
  *
  * @param[in] node : listIterator
  * @return If the node is on the left/side of the list
  */
  int (*isSideElement)( listIterator node );

  /**
  * Inserts list @p source to the left side of @p node of list @p target
  * leaving @p source empty.
  * Note that @p target MUST BE NON-NULL only when the @p node is first/last
  * element of the list (isSideElement return true).
  * For all other situations it may be NULL
  *
  * @param[in] target : list*
  * @param[in] node : listIterator
  * @param[in] source : list*
  */
  void (*insertListAt)( list* target, listIterator node, list* source );

  /**
  * Inserts value  to the left side of @p node of list @p target
  *
  * Note that @p target MUST BE NON-NULL only when the @p node is first/last
  * element of the list (isSideElement return true).
  * For all other situations it may be NULL
  *
  * @param[in] target : list*
  * @param[in] node : listIterator
  * @param[in] value : void*
  */
  void (*insertElementAt)( list* target, listIterator node, void* value );


  /**
  * Checks if given node is the last element
  *
  * NOTICE: For detached nodes this function returns true
  *         Formally detached nodes are simultaniously
  *         on the left and right side of the list as they have no
  *         neightbours on any side.
  *
  * @param[in] node : listIterator
  * @return If the node is on the list's end?
  */
  int (*isListEnd)(listIterator);

  /**
  * Checks if given node is the first element.
  *
  * NOTICE: For detached nodes this function returns true
  *         Formally detached nodes are simultaniously
  *         on the left and right side of the list as they have no
  *         neightbours on any side.
  *
  * @param[in] node : listIterator
  * @return If the node is on the list's begining?
  */
  int (*isListBegin)(listIterator);

  /**
  * All elements on the right side of @p node are transferred to the new list
  * that is returned.
  *
  * @param l : list*
  * @param node : listIterator
  * @return list
  */
  list (*splitList)( list* l, listIterator node );

  /**
  * Get next element on the list.
  * Returns NULL if node is the last element.
  *
  * NOTICE: All listIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param node : listIterator
  * @return next node (the right neighbour of the current node)
  */
  listIterator (*next)( listIterator node );

  /**
  * Get prevous element on the list.
  * Returns NULL if node is the last element.
  *
  * NOTICE: All listIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param node : listIterator
  * @return previous node (the left neighbour of the current node)
  */
  listIterator (*previous)( listIterator node );

  /**
  * Get value of the list element. Returns void pointer to underlying data.
  * Returns NULL if element is NULL.
  *
  * NOTICE: All listIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param node : listIterator
  * @return value under the given node
  */
  void* (*getValue)( listIterator node );

  /**
  * Sets value of the list element.
  * Does nothing if element is NULL.
  *
  * NOTICE: All listIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param node : listIterator
  * @param value : void*
  */
  void (*setValue)( listIterator node, void* value );
};


/**
* Lists interface object
* All functions should be accessed using Lists object
*/
extern const struct lists Lists;


#endif /* __STY_COMMON_LIST_H__ */
