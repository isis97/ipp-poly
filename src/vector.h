/*
*  Bidirectional vector implementation (C99 standard)
*  Usage:
*     #include <vector.h>
*      ...
*     vector l = Vectors.new();
*
*  All interface sould be accessed through Vectors constant.
*
*  MIT LICENSE
*  @Piotr Styczyński 2017
*/
#include "utils.h"

#ifndef STY_COMMON_VECTOR_H__
#define STY_COMMON_VECTOR_H__

#define VECTOR_INITIAL_SIZE 3

/*
* Macro for interating through vector
* Usage:
*
*   loop_vector(vector_object, i) {
*         void* element = Vectors.value(i);
*         printf("void_ptr = %p\n", i);
*    }
*
*/
#define loop_vector(VECTOR, VAR_NAME) \
  for(vectorIterator VAR_NAME = Vectors.begin(VECTOR); VAR_NAME != NULL; VAR_NAME=Vectors.next(VAR_NAME))

/*
* Declare data types needed for vectors implementation
*/
typedef struct vectors vectors;
typedef struct vectorRoot vectorRoot;

/*
* Structure representing one element of vector
* It's got two neighbours (may be NULL)
* Element also contains void* pointer to the actual data.
*/
typedef void* vectorNode;

/*
* Actual vector - syntax sugar for writing pointers everywhere
*/
typedef vectorRoot vector;

/*
* Root element of the vector containing pointers
* to the two ends of a vector
*/
struct vectorRoot {
  vectorNode* data;
  int len;
  int allocLen;
};

/*
* Vector iterator
*/
typedef vectorNode* vectorIterator;

/*
* Function of type
* void* -> void*
* Used as vector data manipulators/iterators
*/
typedef void* (*vectorModifierFn)(void*);

/*
* Interface for vectors
*/
struct vectors {

  /*
  * Create new vector
  * All vectors must be then freed with Vectors.free(vector).
  *
  */
  vector (*new)( );

  /*
  * Destroy given vector freeing up memory.
  *
  * WARN: Invalidates vectorIterators
  */
  void (*free)( vector* );

  /*
  * Push element to the front of a given vector.
  * Method returns pointer to the newly created vector node.
  *
  * NOTICE: All vectorIterators are valid until used operation does not
  *         keep pointers validity.
  */
  vectorIterator (*pushFront)( vector* l, void* element );

  /*
  * Push element to the end of a given vector.
  * Method returns pointer to the newly created vector node.
  *
  * NOTICE: All vectorIterators are valid until used operation does not
  *         keep pointers validity.
  */
  vectorIterator (*pushBack)( vector* l, void* element );

  /*
  * Removes first element of the given vector or does nothing if it's empty.
  * Returns data pointer held by the removed element.
  * If no element was removed (vector is empty) NULL is returned.
  *
  * WARN: Invalidates vectorIterators when elment under pointers
  *       will be popped.
  */
  void* (*popFront)( vector* l );

  /*
  * Removes last element of the given vector or does nothing if it's empty.
  * Returns data pointer held by the removed element.
  * If no element was removed (vector is empty) NULL is returned.
  *
  * WARN: Invalidates vectorIterators when elment under pointers
  *       will be popped.
  */
  void* (*popBack)( vector* l );

  /*
  * Clears the entire vector.
  *
  * WARN: Invalidates vectorIterators for all elements of vector
  */
  void (*clear)( vector* l );

  /*
  * Obtain first element of the vector.
  * Function return void* pointer to the data under first element.
  *
  * If the vector is empty NULL is returned.
  */
  void* (*first)( vector* l );

  /*
  * Obtain last element of the vector.
  * Function return void* pointer to the data under first element.
  *
  * If the vector is empty NULL is returned.
  */
  void* (*last)( vector* l );

  /*
  * Obtain the size of a vector.
  *
  * WARN: Works in O(n) time where n is the length of the vector
  */
  int (*size)( vector* l );

  /*
  * Check's if vector is empty
  */
  int (*empty)( vector* l );

  /*
  * Copy the vector into a new one.
  *
  * WARN: Each element will be a new one, but the data
  *       pointers will be still pointing to the same
  *       memory locations (there're still the same
  *       objects under void* pointers)
  */
  vector (*copy)( vector* l );

  /*
  * Performs deep copy of the vector returning new one.
  * The given (void*)->(void*) function is used as assigner.
  * The function should create new element value, copy the value of
  * the given one and return pointer to this element.
  *
  */
  vector (*deepCopy)( vector* l, vectorModifierFn assigner );

  /*
  * Copy the vector <source> into other vector - <target>
  *
  * WARN: Each element will be a new one, but the data
  *       pointers will be still pointing to the same
  *       memory locations (there're still the same
  *       objects under void* pointers)
  */
  void (*copyInto)( vector* source, vector* target );

  /*
  * Iterate through vector using given
  * (void*)->(void*) function.
  * Function is executed for every vector element value
  * The return value is ignored.
  *
  */
  void (*iterate)( vector* l, vectorModifierFn iterator );

  /*
  * Map vector values using given
  * (void*)->void function
  * Function is executed for every vector element value
  * Then the result of function is assigned to the
  * element's data pointer.
  *
  * NOTE: Mapping is made in place.
  */
  void (*map)( vector* l, vectorModifierFn mapping );

  /*
  * Print given vector to stdout.
  * Prints only adresses of values not exact values.
  */
  void (*print)( vector* );

  /*
  * Print given vector to stdout.
  * Prints only adresses of values not exact values.
  * Variant displaying new line at the end of stringified vector.
  */
  void (*println)( vector* l );

  /*
  * Get the first element container pointer.
  * If the vector is empty then NULL is returned.
  *
  * NOTICE: All vectorIterators are valid until used operation does not
  *         keep pointers validity.
  */
  vectorIterator (*begin)( vector* l );

  /*
  * Get the last element container pointer.
  * If the vector is empty then NULL is returned.
  *
  * NOTICE: All vectorIterators are valid until used operation does not
  *         keep pointers validity.
  */
  vectorIterator (*end)( vector* l );

  /*
  * Removes element from the vector using given container pointer.
  * The vector parameter MUST BE NON NULL for nodes that are first or last
  * (isSideElement return true)
  * For all other situations it may be NULL
  *
  * WARN: Invalidates pointers to the removed elements.
  */
  void (*detachElement)( vector* l, vectorIterator node );

  /*
  * Create node that is not attached to anything.
  * This functionality may be used in situations when you need
  * lsit nodes outside actual vector.
  *
  * NOTICE: All vectorIterators are valid until used operation does not
  *         keep pointers validity.
  */
  vectorIterator (*newDetachedElement)( );

  /*
  * Checks if given node is the last or first element.
  */
  int (*isSideElement)( vectorIterator node );

  /*
  * Inserts vector <source> to the left side of <node> of vector <target>
  * leaving <source> empty.
  * Note that <target> MUST BE NON-NULL only when the <node> is first/last
  * element of the vector (isSideElement return true).
  * For all other situations it may be NULL
  *
  */
  void (*insertVectorAt)( vector* target, vectorIterator node, vector* source );

  /*
  * Inserts value  to the left side of <node> of vector <target>
  *
  * Note that <target> MUST BE NON-NULL only when the <node> is first/last
  * element of the vector (isSideElement return true).
  * For all other situations it may be NULL
  *
  */
  void (*insertElementAt)( vector* target, vectorIterator node, void* value );


  /*
  * Checks if given node is the last element
  */
  int (*isVectorEnd)(vectorIterator);

  /*
  * Checks if given node is the first element.
  */
  int (*isVectorBegin)(vectorIterator);

  /*
  * All elements on the right side of <node> are transferred to the new vector
  * that is returned.
  */
  vector (*splitVector)( vector* l, vectorIterator node );

  /*
  * Get next element on the vector.
  * Returns NULL if node is the last element.
  *
  * NOTICE: All vectorIterators are valid until used operation does not
  *         keep pointers validity.
  */
  vectorIterator (*next)( vectorIterator node );

  /*
  * Get prevous element on the vector.
  * Returns NULL if node is the last element.
  *
  * NOTICE: All vectorIterators are valid until used operation does not
  *         keep pointers validity.
  */
  vectorIterator (*previous)( vectorIterator node );

  /*
  * Get value of the vector element. Returns void pointer to underlying data.
  * Returns NULL if element is NULL.
  *
  * NOTICE: All vectorIterators are valid until used operation does not
  *         keep pointers validity.
  */
  void* (*getValue)( vectorIterator node );

  /*
  * Sets value of the vector element.
  * Does nothing if element is NULL.
  *
  * NOTICE: All vectorIterators are valid until used operation does not
  *         keep pointers validity.
  */
  void (*setValue)( vectorIterator node, void* value );
};


/*
* Vectors interface object
* All functions should be accessed using Vectors object
*/
extern const struct vectors Vectors;


#endif
