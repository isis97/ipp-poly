/*
*  Bidirectional pList implementation (C99 standard)
*  Usage:
*     #include <pList.h>
*      ...
*     pList l = pLists.new();
*
*  All interface sould be accessed through pLists constant.
*
*  MIT LICENSE
*  @Piotr Styczyński 2017
*/
#include "utils.h"

#ifndef STY_COMMON_LIST_H__
#define STY_COMMON_LIST_H__

/*
* Macro for interating through pList
* Usage:
*
*   loop_pList(pList_object, i) {
*         void* element = pLists.value(i);
*         printf("void_ptr = %p\n", i);
*    }
*
*/
#define loop_pList(LIST, VAR_NAME) \
  for(pListIterator VAR_NAME = pLists.begin(LIST); VAR_NAME != NULL; VAR_NAME=pLists.next(VAR_NAME))

/*
* Declare data types needed for pLists implementation
*/
typedef struct pListsInterface pListsInterface;
typedef struct pListNode pListNode;
typedef struct pListRoot pListRoot;

/*
* Actual pList - syntax sugar for writing pointers everywhere
*/
typedef pListRoot* pList;

/*
* pList iterator
*/
typedef pListNode* pListIterator;

/*
* Function of type
* void* -> void*
* Used as pList data manipulators/iterators
*/
typedef void* (*pListModifierFn)(void*);

/*
* Interface for pLists
*/
struct pListsInterface {

  /*
  * Create new pList
  * All pLists must be then freed with pLists.free(pList).
  *
  */
  pList (*new)( );

  /*
  * Destroy given pList freeing up memory.
  *
  * WARN: Invalidates pListIterators
  */
  void (*free)( pList );

  /*
  * Push element to the front of a given pList.
  * Method returns pointer to the newly created pList node.
  *
  * NOTICE: All pListIterators are valid until used operation does not
  *         keep pointers validity.
  */
  pListIterator (*pushFront)( pList l, void* element );

  /*
  * Push element to the end of a given pList.
  * Method returns pointer to the newly created pList node.
  *
  * NOTICE: All pListIterators are valid until used operation does not
  *         keep pointers validity.
  */
  pListIterator (*pushBack)( pList l, void* element );

  /*
  * Removes first element of the given pList or does nothing if it's empty.
  * Returns data pointer held by the removed element.
  * If no element was removed (pList is empty) NULL is returned.
  *
  * WARN: Invalidates pListIterators when elment under pointers
  *       will be popped.
  */
  void* (*popFront)( pList l );

  /*
  * Removes last element of the given pList or does nothing if it's empty.
  * Returns data pointer held by the removed element.
  * If no element was removed (pList is empty) NULL is returned.
  *
  * WARN: Invalidates pListIterators when elment under pointers
  *       will be popped.
  */
  void* (*popBack)( pList l );

  /*
  * Clears the entire pList.
  *
  * WARN: Invalidates pListIterators for all elements of pList
  */
  void (*clear)( pList l );

  /*
  * Obtain first element of the pList.
  * Function return void* pointer to the data under first element.
  *
  * If the pList is empty NULL is returned.
  */
  void* (*first)( pList l );

  /*
  * Obtain last element of the pList.
  * Function return void* pointer to the data under first element.
  *
  * If the pList is empty NULL is returned.
  */
  void* (*last)( pList l );

  /*
  * Obtain the size of a pList.
  *
  * WARN: Works in O(n) time where n is the length of the pList
  */
  int (*size)( pList l );

  /*
  * Check's if pList is empty
  */
  int (*empty)( pList l );

  /*
  * Copy the pList into a new one.
  *
  * WARN: Each element will be a new one, but the data
  *       pointers will be still pointing to the same
  *       memory locations (there're still the same
  *       objects under void* pointers)
  */
  pList (*copy)( pList l );

  /*
  * Performs deep copy of the pList returning new one.
  * The given (void*)->(void*) function is used as assigner.
  * The function should create new element value, copy the value of
  * the given one and return pointer to this element.
  *
  */
  pList (*deepCopy)( pList l, pListModifierFn assigner );

  /*
  * Copy the pList <source> into other pList - <target>
  *
  * WARN: Each element will be a new one, but the data
  *       pointers will be still pointing to the same
  *       memory locations (there're still the same
  *       objects under void* pointers)
  */
  void (*copyInto)( pList source, pList target );

  /*
  * Iterate through pList using given
  * (void*)->(void*) function.
  * Function is executed for every pList element value
  * The return value is ignored.
  *
  */
  void (*iterate)( pList l, pListModifierFn iterator );

  /*
  * Map pList values using given
  * (void*)->void function
  * Function is executed for every pList element value
  * Then the result of function is assigned to the
  * element's data pointer.
  *
  * NOTE: Mapping is made in place.
  */
  void (*map)( pList l, pListModifierFn mapping );

  /*
  * Print given pList to stdout.
  * Prints only adresses of values not exact values.
  */
  void (*print)( pList );

  /*
  * Print given pList to stdout.
  * Prints only adresses of values not exact values.
  * Variant displaying new line at the end of stringified pList.
  */
  void (*println)( pList l );

  /*
  * Get the first element container pointer.
  * If the pList is empty then NULL is returned.
  *
  * NOTICE: All pListIterators are valid until used operation does not
  *         keep pointers validity.
  */
  pListIterator (*begin)( pList l );

  /*
  * Get the last element container pointer.
  * If the pList is empty then NULL is returned.
  *
  * NOTICE: All pListIterators are valid until used operation does not
  *         keep pointers validity.
  */
  pListIterator (*end)( pList l );

  /*
  * Removes element from the pList using given container pointer.
  * The pList parameter MUST BE NON NULL for nodes that are first or last
  * (isSideElement return true)
  * For all other situations it may be NULL
  *
  * WARN: Invalidates pointers to the removed elements.
  */
  void (*detachElement)( pList l, pListIterator node );

  /*
  * Create node that is not attached to anything.
  * This functionality may be used in situations when you need
  * lsit nodes outside actual pList.
  *
  * NOTICE: All pListIterators are valid until used operation does not
  *         keep pointers validity.
  */
  pListIterator (*newDetachedElement)( );

  /*
  * Checks if given node is the last or first element.
  */
  int (*isSideElement)( pListIterator node );

  /*
  * Inserts pList <source> to the left side of <node> of pList <target>
  * leaving <source> empty.
  * Note that <target> MUST BE NON-NULL only when the <node> is first/last
  * element of the pList (isSideElement return true).
  * For all other situations it may be NULL
  *
  */
  void (*insertpListAt)( pList target, pListIterator node, pList source );

  /*
  * Inserts value  to the left side of <node> of pList <target>
  *
  * Note that <target> MUST BE NON-NULL only when the <node> is first/last
  * element of the pList (isSideElement return true).
  * For all other situations it may be NULL
  *
  */
  void (*insertElementAt)( pList target, pListIterator node, void* value );


  /*
  * Checks if given node is the last element
  */
  int (*ispListEnd)(pListIterator);

  /*
  * Checks if given node is the first element.
  */
  int (*ispListBegin)(pListIterator);

  /*
  * All elements on the right side of <node> are transferred to the new pList
  * that is returned.
  */
  pList (*splitpList)( pList l, pListIterator node );

  /*
  * Get next element on the pList.
  * Returns NULL if node is the last element.
  *
  * NOTICE: All pListIterators are valid until used operation does not
  *         keep pointers validity.
  */
  pListIterator (*next)( pListIterator node );

  /*
  * Get prevous element on the pList.
  * Returns NULL if node is the last element.
  *
  * NOTICE: All pListIterators are valid until used operation does not
  *         keep pointers validity.
  */
  pListIterator (*previous)( pListIterator node );

  /*
  * Get value of the pList element. Returns void pointer to underlying data.
  * Returns NULL if element is NULL.
  *
  * NOTICE: All pListIterators are valid until used operation does not
  *         keep pointers validity.
  */
  void* (*getValue)( pListIterator node );

  /*
  * Sets value of the pList element.
  * Does nothing if element is NULL.
  *
  * NOTICE: All pListIterators are valid until used operation does not
  *         keep pointers validity.
  */
  void (*setValue)( pListIterator node, void* value );
};


/*
* pLists interface object
* All functions should be accessed using pLists object
*/
extern const struct pListsInterface pLists;


#endif
