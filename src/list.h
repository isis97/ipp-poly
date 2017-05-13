/** @file
*  Bidirectional List implementation (C99 standard)
*  Usage:
*  @code
*     #include <list.h>
*      ...
*     List l = LISTS.new();
*  @endcode
*
*  All interface sould be accessed through ::LISTS constant.
*
*  @author Piotr Styczyński <piotrsty1@gmail.com>
*  @copyright MIT
*  @date 2017-05-13
*/
#include "utils.h"
#include "memalloc.h"

#ifndef __STY_COMMON_LIST_H__
#define __STY_COMMON_LIST_H__

/**
* @def LOOP_LIST(LIST, VAR_NAME)
* Macro for interating through List
*
* Usage:
*
* @code
*   LOOP_LIST(List_object, i) {
*         ListData element = Lists.value(i);
*         printf("void_ptr = %p\n", i);
*    }
* @endcode
*
* @param[in] LIST : List name to be iterated
* @param[in] VAR_NAME : name of iterator variable
*/
#define LOOP_LIST(LIST, VAR_NAME) \
  for(ListIterator VAR_NAME = LISTS.begin(LIST); \
  VAR_NAME != NULL; VAR_NAME=LISTS.next(VAR_NAME))

/*
* Declare data types needed for Lists implementation
*/

/** Data type held by list */
typedef void* ListData;

/** Lists interface */
typedef struct ListsInterface ListsInterface;

/** Type of Lists nodes (elements) */
typedef struct ListNode ListNode;

/** Type of List root element */
typedef struct ListRoot ListRoot;

/**
* Lists interface object
* All functions should be accessed using Lists object
*/
extern const ListsInterface LISTS;

/**
* Actual type of List - syntax sugar for writing pointers everywhere
*/
typedef ListRoot List;

/**
* Root element of the List containing pointers
* to the two ends of a List
*/
struct ListRoot {
  ListNode* begin; ///< pointer to the begining of the List
  ListNode* end; ///< pointer to the end of the List
};

/**
* List iterator
*/
typedef ListNode* ListIterator;

/**
* Function of type
* ListData -> ListData
* Used as List data manipulators/iterators
*/
typedef ListData (*ListModifierFn)(ListData);

/**
* Interface for Lists accessible through ::LISTS
*
* All operations involving lists should be done indirectly
* by this interface.
* Representant of this interface is object ::LISTS
*
* All function pointers can be called by using
* `LISTS.function( args )`
*
*/
struct ListsInterface {

  /**
  * Create new List
  * All Lists must be then freed with Lists.free(List).
  *
  * @return List
  */
  List (*new)( );

  /**
  * Destroy given List freeing up memory.
  *
  * WARN: Invalidates ListIterators
  *
  * @param[in] l : List*
  */
  void (*free)( List* );

  /**
  * Push @p element to the front of a given List.
  * Method returns pointer to the newly created List node.
  *
  * NOTICE: All ListIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param[in] l : List*
  * @param[in] element : ListData
  * @return ListIterator
  */
  ListIterator (*pushFront)( List* l, ListData element );

  /**
  * Push @p element to the end of a given List.
  * Method returns pointer to the newly created List node.
  *
  * NOTICE: All ListIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param[in] l : List*
  * @param[in] element : ListData
  * @return ListIterator
  */
  ListIterator (*pushBack)( List* l, ListData element );

  /**
  * Removes first element of the given List or does nothing if it's empty.
  * Returns data pointer held by the removed element.
  * If no element was removed (List is empty) NULL is returned.
  *
  * WARN: Invalidates ListIterators when elment under pointers
  *       will be popped.
  *
  * @param[in] l : List*
  * @return ListData (popped element)
  */
  ListData (*popFront)( List* l );

  /**
  * Removes last element of the given List or does nothing if it's empty.
  * Returns data pointer held by the removed element.
  * If no element was removed (List is empty) NULL is returned.
  *
  * WARN: Invalidates ListIterators when elment under pointers
  *       will be popped.
  *
  * @param[in] l : List*
  * @return ListData (popped element)
  */
  ListData (*popBack)( List* l );

  /**
  * Clears the entire List.
  *
  * WARN: Invalidates ListIterators for all elements of List
  *
  * @param[in] l : List*
  */
  void (*clear)( List* l );

  /**
  * Obtain first element of the List.
  * Function return ListData pointer to the data under first element.
  *
  * If the List is empty NULL is returned.
  *
  * @param[in] l : const List*
  * @return first element if exists
  */
  ListData (*first)( const List* l );

  /**
  * Obtain last element of the List.
  * Function return ListData pointer to the data under first element.
  *
  * If the List is empty NULL is returned.
  *
  * @param[in] l : const List*
  * @return last element if exists
  */
  ListData (*last)( const List* l );

  /**
  * Obtain the size of a List.
  *
  * WARN: Works in O(n) time where n is the length of the List
  *
  * @param[in] l : const List*
  * @return size of the List
  */
  int (*size)( const List* l );

  /**
  * Check's if List is empty
  *
  * @param[in] l : const List*
  * @return If List is empty?
  */
  int (*empty)( const List* l );

  /**
  * Copy the List into a new one.
  *
  * WARN: Each element will be a new one, but the data
  *       pointers will be still pointing to the same
  *       memory locations (there're still the same
  *       objects under ListData pointers)
  *
  * @param[in] l : const List*
  * @return shallow copy of a given List
  */
  List (*copy)( const List* l );

  /**
  * Performs deep copy of the List returning new one.
  * The given (ListData)->(ListData) function is used as assigner.
  * The function should create new element value, copy the value of
  * the given one and return pointer to this element.
  *
  * @param[in] l : const List*
  * @param[in] elementAllocator : ListModifierFn
  * @return deep copy of a given List
  */
  List (*deepCopy)( const List* l, ListModifierFn assigner );

  /**
  * Copy the List @p source into other List -  @p target
  *
  * WARN: Each element will be a new one, but the data
  *       pointers will be still pointing to the same
  *       memory locations (there're still the same
  *       objects under ListData pointers)
  *
  * @param[in] source : const List*
  * @param[in] target : List*
  */
  void (*copyInto)( const List* source, List* target );

  /**
  * Iterate through List using given
  * (ListData)->(ListData) function.
  * Function is executed for every List element value
  * The return value is ignored.
  *
  * @param[in] l : const List*
  * @param[in] mappingFunction : ListModifierFn
  */
  void (*iterate)( const List* l, ListModifierFn iterator );

  /**
  * Map List values using given
  * (ListData)->void function
  * Function is executed for every List element value
  * Then the result of function is assigned to the
  * element's data pointer.
  *
  * NOTICE: Mapping is made in place.
  *
  * @param[in] l : const List*
  * @param[in] mappingFunction : ListModifierFn
  */
  void (*map)( List* l, ListModifierFn mapping );

  /**
  * Print given List to stdout.
  * Prints only adresses of values not exact values.
  *
  * @param[in] l : const List*
  */
  void (*print)( const List* );

  /**
  * Print given List to stdout.
  * Prints only adresses of values not exact values.
  * Variant displaying new line at the end of stringified List.
  *
  * @param[in] l : const List*
  */
  void (*println)( const List* l );

  /**
  * Get the first element container pointer.
  * If the List is empty then NULL is returned.
  *
  * NOTICE: All ListIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param[in] l : const List*
  * @return ListIterator pointing to the List begining
  */
  ListIterator (*begin)( const List* l );

  /**
  * Get the last element container pointer.
  * If the List is empty then NULL is returned.
  *
  * NOTICE: All ListIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param[in] l : const List*
  * @return ListIterator pointing to the List end
  */
  ListIterator (*end)( const List* l );

  /**
  * Removes element from the List using given container pointer.
  * The List parameter MUST BE NON NULL for nodes that are first or last
  * (isSideElement return true)
  * For all other situations it may be NULL
  *
  * WARN: Invalidates pointers to the removed elements.
  *
  * @param[in] l : List*
  * @param[in] node : ListIterator
  */
  void (*detachElement)( List* l, ListIterator node );

  /**
  * Create node that is not attached to anything.
  * This functionality may be used in situations when you need
  * List nodes outside actual List.
  *
  * NOTICE: All ListIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @return ListIterator to that not-attached node
  */
  ListIterator (*newDetachedElement)( );

  /**
  * Checks if given node is the last or first element.
  *
  * NOTICE: For detached nodes this function returns true
  *         Formally detached nodes are simultaniously
  *         on the left and right side of the List as they have no
  *         neightbours on any side.
  *
  * @param[in] node : ListIterator
  * @return If the node is on the left/side of the List
  */
  int (*isSideElement)( ListIterator node );

  /**
  * Inserts List @p source to the left side of @p node of List @p target
  * leaving @p source empty.
  * Note that @p target MUST BE NON-NULL only when the @p node is first/last
  * element of the List (isSideElement return true).
  * For all other situations it may be NULL
  *
  * @param[in] target : List*
  * @param[in] node : ListIterator
  * @param[in] source : List*
  */
  void (*insertListAt)( List* target, ListIterator node, List* source );

  /**
  * Inserts value  to the left side of @p node of List @p target
  *
  * Note that @p target MUST BE NON-NULL only when the @p node is first/last
  * element of the List (isSideElement return true).
  * For all other situations it may be NULL
  *
  * @param[in] target : List*
  * @param[in] node : ListIterator
  * @param[in] value : ListData
  */
  void (*insertElementAt)( List* target, ListIterator node, ListData value );


  /**
  * Checks if given node is the last element
  *
  * NOTICE: For detached nodes this function returns true
  *         Formally detached nodes are simultaniously
  *         on the left and right side of the List as they have no
  *         neightbours on any side.
  *
  * @param[in] node : ListIterator
  * @return If the node is on the List's end?
  */
  int (*isListEnd)(ListIterator);

  /**
  * Checks if given node is the first element.
  *
  * NOTICE: For detached nodes this function returns true
  *         Formally detached nodes are simultaniously
  *         on the left and right side of the List as they have no
  *         neightbours on any side.
  *
  * @param[in] node : ListIterator
  * @return If the node is on the List's begining?
  */
  int (*isListBegin)(ListIterator);

  /**
  * All elements on the right side of @p node are transferred to the new List
  * that is returned.
  *
  * @param l : List*
  * @param node : ListIterator
  * @return List
  */
  List (*splitList)( List* l, ListIterator node );

  /**
  * Get next element on the List.
  * Returns NULL if node is the last element.
  *
  * NOTICE: All ListIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param node : ListIterator
  * @return next node (the right neighbour of the current node)
  */
  ListIterator (*next)( ListIterator node );

  /**
  * Get prevous element on the List.
  * Returns NULL if node is the last element.
  *
  * NOTICE: All ListIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param node : ListIterator
  * @return previous node (the left neighbour of the current node)
  */
  ListIterator (*previous)( ListIterator node );

  /**
  * Get value of the List element. Returns void pointer to underlying data.
  * Returns NULL if element is NULL.
  *
  * NOTICE: All ListIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param node : ListIterator
  * @return value under the given node
  */
  ListData (*getValue)( ListIterator node );

  /**
  * Sets value of the List element.
  * Does nothing if element is NULL.
  *
  * NOTICE: All ListIterators are valid until used operation does not
  *         keep pointers validity.
  *
  * @param node : ListIterator
  * @param value : ListData
  */
  void (*setValue)( ListIterator node, ListData value );
};



#endif /* __STY_COMMON_LIST_H__ */
