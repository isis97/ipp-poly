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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "pList.h"


/*
* Structure representing one element of pList
* It's got two neighbours (may be NULL)
* Element also contains void* pointer to the actual data.
*/
struct pListNode {
  pListNode* right;
  void* value;
  pListNode* left;
};

/*
* Root element of the pList containing pointers
* to the two ends of a pList
*/
struct pListRoot {
  pListNode* begin;
  pListNode* end;
};

/*
* Null/empty objects - used for memory allocation
*/
const pListNode nullPListNode = {
  .right = NULL,
  .left = NULL
};

const pListRoot nullPListRoot = {
  .begin = NULL,
  .end = NULL
};

/*
* Makes new pList from given values
*/
pListNode* makepListNode(pListNode* left, void* value, pListNode* right) {
  pListNode* l = malloc(sizeof(nullPListNode));
  l->right = right;
  l->left = left;
  l->value = value;
  return l;
}

/*
* Allocate new pList
*/
pList newpList() {
  pList l = malloc(sizeof(nullPListRoot));
  l->begin = NULL;
  l->end = NULL;
  return l;
}

/*
* Null/empty objects - used for memory allocation
*/
void freepListRecLeft(pListNode* l) {
  if(l == NULL) return;
  if(l->left != NULL) {
    freepListRecLeft(l->left);
  }
  free(l);
}

/*
* Push element to the front of the given pList
*/
pListNode* pushFrontpList(pList l, void* value) {
  if(l == NULL) {
    return NULL;
  } else {
    pListNode* new_node = makepListNode(NULL, value, l->begin);
    if(l->begin != NULL) {
      (l->begin)->left = new_node;
    }
    (l->begin) = new_node;
    if(l->end == NULL) {
      (l->end) = new_node;
    }
    return new_node;
  }
}

/*
* Push element to the end of the given pList
*/
pListNode* pushBackpList(pList l, void* value) {
  if(l == NULL) {
    return NULL;
  } else {
    pListNode* new_node = makepListNode(l->end, value, NULL);
    if(l->end != NULL) {
      (l->end)->right = new_node;
    }
    (l->end) = new_node;
    if(l->begin == NULL) {
      (l->begin) = new_node;
    }
    return new_node;
  }
}

/*
* Remove the first element of the given pList
* Return data pointer held by the removed element.
* If pList is empty return NULL.
*/
void* popFrontpList(pList l) {
  if(l == NULL) {
    return NULL;
  } else if (l->begin == NULL) {
    return NULL;
  } else {
    void *val = (l->begin)->value;
    pListNode* new_begin = (l->begin)->right;
    if(new_begin != NULL) {
      new_begin->left = NULL;
    } else {
      (l->end) = NULL;
    }
    free(l->begin);
    (l->begin) = new_begin;
    return val;
  }
}

/*
* Remove the last element of the given pList
* Return data pointer held by the removed element.
* If pList is empty return NULL.
*/
void* popBackpList(pList l) {
  if(l == NULL) {
    return NULL;
  } else if (l->end == NULL) {
    return NULL;
  } else {
    void* val = (l->end)->value;
    pListNode* new_end = (l->end)->left;
    if(new_end != NULL) {
      new_end->right = NULL;
    } else {
      (l->begin) = NULL;
    }
    free(l->end);
    (l->end) = new_end;
    return val;
  }
}

/*
* Remove all elements of the given pList
*/
void clearpList(pList l) {
  if(l == NULL) return;
  if(l->end != NULL) {
    freepListRecLeft(l->end);
  }
  (l->begin)=NULL;
  (l->end)=NULL;
}

/*
* Deallocate pList
*/
void freepList(pList l) {
  if(l == NULL) return;
  //clearpList(l);
  pListNode* it = l->begin;
  while(it != NULL) {
    pListNode* next = it->right;
    free(it);
    it = next;
  }

  free(l);
}

/*
* Get first element data pointer or NULL if the pList is empty
*/
void* getFirstpListElement(pList l) {
  if(l == NULL) {
    return NULL;
  }
  if(l->begin == NULL) {
    return NULL;
  }
  if(l->begin != NULL) return (l->begin)->value;
  return NULL;
}

/*
* Get lst element data pointer or NULL if the pList is empty
*/
void* getLastpListElement(pList l) {
  if(l == NULL) {
    return NULL;
  }
  if(l->end == NULL) {
    return NULL;
  }
  if(l->end != NULL) return (l->end)->value;
  return NULL;
}

/*
* Measure pList size in O(n) time
*/
int getpListSize(pList l) {
  if(l == NULL) return 0;
  int size = 0;
  loop_pList(l, it) {
    ++size;
  }
  return size;
}

/*
* Check if pList is empty
*/
int ispListEmpty( pList l ) {
  if(l == NULL) return 1;
  return (l->begin == NULL && l->end == NULL);
}

/*
* Add all elements of <src> to the <tgt>
*/
void copypListInto( pList src, pList tgt ) {
  if(src == NULL) return;
  loop_pList(src, it) {
    pushBackpList(tgt, it->value);
  }
}

/*
* Make a copy of a pList
*/
pList copypList( pList l ) {
  if(l == NULL) return NULL;
  pList ret = newpList();
  copypListInto(l, ret);
  return ret;
}


/*
* Maps (void*)->(void*) function on node <l> going to its right side
*/
void mappListNodes( pListNode* l, pListModifierFn mapping, int preserveValue ) {
  if(l==NULL) return;
  void* ret = mapping(l->value);
  if(!preserveValue) {
    l->value = ret;
  }
  mappListNodes(l->right, mapping, preserveValue);
}

/*
* Iterate (void*)->(void*) function through the given pList
*/
void iteratepList(pList l, pListModifierFn iterator) {
  if(l == NULL) return;
  if(l->begin == NULL) return;
  mappListNodes(l->begin, iterator, 1);
}

/*
* Maps (void*)->(void*) function on the given pList
*/
void mappList(pList l, pListModifierFn mapping) {
  if(l == NULL) return;
  if(l->begin == NULL) return;
  mappListNodes(l->begin, mapping, 0);
}

/*
* Make a deep copy of a pList
*/
pList deepCopypList( pList l, pListModifierFn assigner ) {
  if(l == NULL) return NULL;
  pList ret = copypList(l);
  mappList(l, assigner);
  return ret;
}

/*
* Print pList nodes starting at <l> and going right
*/
void printpListNodes(pListNode* l) {
  if(l==NULL) return;
  printf("%p; ", (void*)(l));
  printpListNodes(l->right);
}

/*
* Print pList nodes of a given pList
*/
void printPList(pList l) {
  printf("[ ");
  printpListNodes(l->begin);
  printf("] ");
}

/*
* Print pList nodes of a given pList with trailing newline
*/
void printlnPList(pList l) {
  printPList(l);
  printf("\n");
}

/*
* Obtain first pList element
*/
pListNode* getpListBegin( pList l ) {
  if(l == NULL) return NULL;
  return l->begin;
}

/*
* Obtain last pList element
*/
pListNode* getpListEnd( pList l ) {
  if(l == NULL) return NULL;
  return l->end;
}

/*
* Remove given element from the pList
*/
void detachpListElement( pList l, pListNode* node ) {
  if(node == NULL) return;

  pListNode* left_neighbour = node->left;
  pListNode* right_neighbour = node->right;

  if(left_neighbour != NULL) {
    left_neighbour->right = right_neighbour;
  } else {
    l->begin = right_neighbour;
  }
  if(right_neighbour != NULL) {
    right_neighbour->left = left_neighbour;
  } else {
    l->end = left_neighbour;
  }
  free(node);
}

/*
* Create new element container not attached to any pList.
*/
pListNode* newpListDetachedElement() {
  pListNode* ret = malloc(sizeof(nullPListNode));
  ret->left = NULL;
  ret->right = NULL;
  ret->value = NULL;
  return ret;
}

/*
* Check if is first/last element
*/
int ispListSideElement( pListNode* node ) {
  if(node == NULL) return 0;
  return ( (node->left == NULL) || (node->right == NULL) );
}

/*
* Check if is first element
*/
int ispListLeftSideEnd( pListNode* node ) {
  if(node == NULL) return 0;
  return (node->left == NULL);
}

/*
* Check if is last element
*/
int ispListRightSideEnd( pListNode* node ) {
  if(node == NULL) return 0;
  return (node->right == NULL);
}

/*
* Append pList <src> to the left of <node> in pList <tgt>
* leaving <src> empty.
* Tgt parameter MUST BE NON-NULL only if <node> is first/last element.
* Otherwise it may be NULL
*/
void insertpListAt( pList tgt, pListNode* node, pList src ) {
  if(node == NULL) return;
  if(src == NULL) return;
  if((src->begin == NULL) || (src->end == NULL)) {
    return;
  }
  if(node->left == NULL) {
    tgt->begin = src->begin;
    src->end->right = node;
    node->left = src->end;

    src->begin = NULL;
    src->end = NULL;
    return;
  } else if(node->right == NULL) {
    node->left->right = src->begin;
    src->begin->left = node->left;
    node->left = src->end;
    src->end->right = node;

    src->begin = NULL;
    src->end = NULL;
    return;
  } else {
    node->left->right = src->begin;
    src->begin->left = node->left;
    node->left = src->end;
    src->end->right = node;

    src->begin = NULL;
    src->end = NULL;
    return;
  }
}


/*
* Append a new value to the left of <node> in pList <tgt>
* Tgt parameter MUST BE NON-NULL only if <node> is first/last element.
* Otherwise it may be NULL
*/
void insertElementAt( pList tgt, pListNode* node, void* value ) {
  pList wrapper = newpList();
  pushBackpList(wrapper, value);
  insertpListAt(tgt, node, wrapper);
  freepList(wrapper);
}


/*
* Transfer all right-side neighbours of <splitter> to a new pList.
* Return that new pList.
*/
pList splitpList( pList src, pListNode* splitter ) {
  if(src == NULL) return NULL;
  pList ret = newpList();
  if(splitter == NULL) {
    return ret;
  }

  pListNode* realEnd = src->end;
  src->end = splitter;

  if(splitter->right != NULL) {
    ret->begin = splitter->right;
    ret->end = realEnd;
    splitter->right = NULL;
    ret->begin->left = NULL;
  }
  return ret;
}

/*
* Get next element on the pList.
* Returns NULL if node is the last element.
*/
pListNode* pListNext( pListNode* node ) {
  if(node==NULL) return NULL;
  return node->right;
}

/*
* Get previous element on the pList.
* Returns NULL if node is the last element.
*/
pListNode* pListPrevious( pListNode* node ) {
  if(node==NULL) return NULL;
  return node->left;
}

/*
* Get value of the pList element. Returns void pointer to underlying data.
* Returns NULL if node is NULL.
*/
void* pListGetValue( pListNode* node ) {
  if(node==NULL) return NULL;
  return node->value;
}

/*
* Sets value of the pList element.
* Does nothing if element is NULL.
*/
void pListSetValue( pListNode* node, void* value ) {
  if(node==NULL) return;
  node->value = value;
}

/*
* pLists interface
*/
const pListsInterface pLists = {
  .new = newpList,
  .free = freepList,
  .pushFront = pushFrontpList,
  .pushBack = pushBackpList,
  .popFront = popFrontpList,
  .popBack = popBackpList,
  .clear = clearpList,
  .first = getFirstpListElement,
  .last = getLastpListElement,
  .size = getpListSize,
  .empty = ispListEmpty,
  .copy = copypList,
  .copyInto = copypListInto,
  .deepCopy = deepCopypList,
  .iterate = iteratepList,
  .print = printPList,
  .println = printlnPList,
  .begin = getpListBegin,
  .end = getpListEnd,
  .detachElement = detachpListElement,
  .newDetachedElement = newpListDetachedElement,
  .isSideElement = ispListSideElement,
  .insertpListAt = insertpListAt,
  .insertElementAt = insertElementAt,
  .ispListEnd = ispListRightSideEnd,
  .ispListBegin = ispListLeftSideEnd,
  .splitpList = splitpList,
  .next = pListNext,
  .previous = pListPrevious,
  .getValue = pListGetValue,
  .setValue = pListSetValue
};
