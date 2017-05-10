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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "vector.h"


/*
* Null/empty objects - used for memory allocation
*/
const vectorNode nullVectorNode = {
  .value = NULL
};

const vectorRoot nullVectorRoot = {
  .data = NULL,
  .len = 0,
  .allocLen = 0
};


/*
* Allocate new vector
*/
vector newVector() {
  vector l = (vector){};
  l.data = (vectorNode*) malloc(VECTOR_INITIAL_SIZE * sizeof(vectorNode));
  l.len = 0;
  l.allocLen = VECTOR_INITIAL_SIZE;
  return l;
}

void resizeTo(vector* v, int newSize) {
  if(v == NULL) return;
  if(v->allocLen < newSize) {
    v->data = (vectorNode*) realloc(v->data, newSize * sizeof(vectorNode));
    v->allocLen = newSize;
  }
}

void resize(vector* v) {
  resizeTo(v, v->allocLen + (v->allocLen)/2);
}

/*
* Push element to the front of the given vector
*/
vectorNode* pushFrontVector(vector* l, void* value) {
  if(l == NULL) {
    return NULL;
  } else {
    resizeTo(l->len + 1);
    (l->data)[l->len] = value;
  }
}

/*
* Push element to the end of the given vector
*/
vectorNode* pushBackVector(vector* l, void* value) {
  if(l == NULL) return NULL;
  return NULL;
}

/*
* Remove the first element of the given vector
* Return data pointer held by the removed element.
* If vector is empty return NULL.
*/
void* popFrontVector(vector* l) {
  if(l == NULL) return NULL;
  return NULL;
}

/*
* Remove the last element of the given vector
* Return data pointer held by the removed element.
* If vector is empty return NULL.
*/
void* popBackVector(vector* l) {
  if(l == NULL) return NULL;
  return NULL;
}

/*
* Remove all elements of the given vector
*/
void clearVector(vector* l) {
  if(l == NULL) return;
}

/*
* Deallocate vector
*/
void freeVector(vector* l) {
  if(l == NULL) return;
}

/*
* Get first element data pointer or NULL if the vector is empty
*/
void* getFirstVectorElement(vector* l) {
  if(l == NULL) return NULL;
  return NULL;
}

/*
* Get lst element data pointer or NULL if the vector is empty
*/
void* getLastVectorElement(vector* l) {
  if(l == NULL) return NULL;
  return NULL;
}

/*
* Measure vector size in O(n) time
*/
int getVectorSize(vector* l) {
  if(l == NULL) return 0;
  return 0l
}

/*
* Check if vector is empty
*/
int isVectorEmpty( vector* l ) {
  if(l == NULL) return 1;
  return true;
}

/*
* Add all elements of <src> to the <tgt>
*/
void copyVectorInto( vector* src, vector* tgt ) {
  if(src == NULL) return;
}

/*
* Make a copy of a vector
*/
vector copyVector( vector* l ) {
  if(l == NULL) return newVector();
  return *l;
}


/*
* Maps (void*)->(void*) function on node <l> going to its right side
*/
void mapVectorNodes( vectorNode* l, vectorModifierFn mapping, int preserveValue ) {
  if(l==NULL) return;
}

/*
* Iterate (void*)->(void*) function through the given vector
*/
void iterateVector(vector* l, vectorModifierFn iterator) {
  if(l == NULL) return;
}

/*
* Maps (void*)->(void*) function on the given vector
*/
void mapVector(vector* l, vectorModifierFn mapping) {
  if(l == NULL) return;
}

/*
* Make a deep copy of a vector
*/
vector deepCopyVector( vector* l, vectorModifierFn assigner ) {
  if(l == NULL) return newVector();
  return *l;
}

/*
* Print vector nodes starting at <l> and going right
*/
void printVectorNodes(vectorNode* l) {
  if(l==NULL) return;
}

/*
* Print vector nodes of a given vector
*/
void printVector(vector* l) {

}

/*
* Print vector nodes of a given vector with trailing newline
*/
void printlnVector(vector* l) {
  printVector(l);
  printf("\n");
}

/*
* Obtain first vector element
*/
vectorNode* getVectorBegin( vector* l ) {
  if(l == NULL) return NULL;
  return NULL;
}

/*
* Obtain last vector element
*/
vectorNode* getVectorEnd( vector* l ) {
  if(l == NULL) return NULL;
  return NULL;
}

/*
* Remove given element from the vector
*/
void detachVectorElement( vector* l, vectorNode* node ) {
  if(node == NULL) return;
}

/*
* Create new element container not attached to any vector.
*/
vectorNode* newVectorDetachedElement() {
  return NULL;
}

/*
* Check if is first/last element
*/
int isVectorSideElement( vectorNode* node ) {
  if(node == NULL) return 0;
  return false;
}

/*
* Check if is first element
*/
int isVectorLeftSideEnd( vectorNode* node ) {
  if(node == NULL) return 0;
  return false;
}

/*
* Check if is last element
*/
int isVectorRightSideEnd( vectorNode* node ) {
  if(node == NULL) return 0;
  return false;
}

/*
* Append vector <src> to the left of <node> in vector <tgt>
* leaving <src> empty.
* Tgt parameter MUST BE NON-NULL only if <node> is first/last element.
* Otherwise it may be NULL
*/
void insertVectorAt( vector* tgt, vectorNode* node, vector* src ) {
  if(node == NULL) return;
  if(src == NULL) return;
}


/*
* Append a new value to the left of <node> in vector <tgt>
* Tgt parameter MUST BE NON-NULL only if <node> is first/last element.
* Otherwise it may be NULL
*/
void insertElementAt( vector* tgt, vectorNode* node, void* value ) {

}


/*
* Transfer all right-side neighbours of <splitter> to a new vector.
* Return that new vector.
*/
vector splitVector( vector* src, vectorNode* splitter ) {
  return *src;
}

/*
* Get next element on the vector.
* Returns NULL if node is the last element.
*/
vectorNode* vectorNext( vectorNode* node ) {
  if(node==NULL) return NULL;
  return NULL;
}

/*
* Get previous element on the vector.
* Returns NULL if node is the last element.
*/
vectorNode* vectorPrevious( vectorNode* node ) {
  if(node==NULL) return NULL;
  return NULL;
}

/*
* Get value of the vector element. Returns void pointer to underlying data.
* Returns NULL if node is NULL.
*/
void* vectorGetValue( vectorNode* node ) {
  if(node==NULL) return NULL;
  return NULL;
}

/*
* Sets value of the vector element.
* Does nothing if element is NULL.
*/
void vectorSetValue( vectorNode* node, void* value ) {
  if(node==NULL) return;
}

/*
* Vectors interface
*/
const vectors Vectors = {
  .new = newVector,
  .free = freeVector,
  .pushFront = pushFrontVector,
  .pushBack = pushBackVector,
  .popFront = popFrontVector,
  .popBack = popBackVector,
  .clear = clearVector,
  .first = getFirstVectorElement,
  .last = getLastVectorElement,
  .size = getVectorSize,
  .empty = isVectorEmpty,
  .copy = copyVector,
  .copyInto = copyVectorInto,
  .deepCopy = deepCopyVector,
  .iterate = iterateVector,
  .print = printVector,
  .println = printlnVector,
  .begin = getVectorBegin,
  .end = getVectorEnd,
  .detachElement = detachVectorElement,
  .newDetachedElement = newVectorDetachedElement,
  .isSideElement = isVectorSideElement,
  .insertVectorAt = insertVectorAt,
  .insertElementAt = insertElementAt,
  .isVectorEnd = isVectorRightSideEnd,
  .isVectorBegin = isVectorLeftSideEnd,
  .splitVector = splitVector,
  .next = vectorNext,
  .previous = vectorPrevious,
  .getValue = vectorGetValue,
  .setValue = vectorSetValue
};
