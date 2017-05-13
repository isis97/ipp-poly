/*
*  Bidirectional List implementation (C99 standard)
*  Usage:
*  @code
*     #include <list.h>
*      ...
*     List l = Lists.new();
*  @endcode
*
*  All interface sould be accessed through Lists constant.
*
*  @author Piotr Styczyński <piotrsty1@gmail.com>
*  @copyright MIT
*  @date 2017-05-13
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
#include "memalloc.h"
#include "list.h"


/**
* Structure representing one element of List
* It's got two neighbours (may be NULL)
* Element also contains ListData pointer to the actual data.
*/
struct ListNode {
  ListNode* right; ///< pointer to the right neighbour
  ListData value; ///< pointer to the data held in List element
  ListNode* left; ///< pointer to the right neighbour
};

/*
* Makes new List from given values
*/
ListNode* MakeListNode(ListNode* left, ListData value, ListNode* right) {
  ListNode* l = MALLOCATE(ListNode);
  l->right = right;
  l->left = left;
  l->value = value;
  return l;
}

/*
* Allocate new List
*/
List NewList() {
  List l = (List){};
  l.begin = NULL;
  l.end = NULL;
  return l;
}

/*
* Null/empty objects - used for memory allocation
*/
void FreeListRecLeft(ListNode* l) {
  if(l == NULL) return;
  if(l->left != NULL) {
    FreeListRecLeft(l->left);
  }
  DBG {printf("FREE Lists.MFREEListRecLeft %p\n", l);fflush(stdout);}
  MFREE(l);
}

/*
* Push element to the front of the given List
*/
ListNode* PushFrontList(List* l, ListData value) {
  if(l == NULL) {
    return NULL;
  } else {
    ListNode* new_node = MakeListNode(NULL, value, l->begin);
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
* Push element to the end of the given List
*/
ListNode* PushBackList(List* l, ListData value) {
  if(l == NULL) {
    return NULL;
  } else {
    ListNode* new_node = MakeListNode(l->end, value, NULL);
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
* Remove the first element of the given List
* Return data pointer held by the removed element.
* If List is empty return NULL.
*/
ListData PopFrontList(List* l) {
  if(l == NULL) {
    return NULL;
  } else if (l->begin == NULL) {
    return NULL;
  } else {
    void *val = (l->begin)->value;
    ListNode* new_begin = (l->begin)->right;
    if(new_begin != NULL) {
      new_begin->left = NULL;
    } else {
      (l->end) = NULL;
    }
    DBG {printf("FREE Lists.popFrontList %p\n", l->begin);fflush(stdout);}
    MFREE(l->begin);
    (l->begin) = new_begin;
    return val;
  }
}

/*
* Remove the last element of the given List
* Return data pointer held by the removed element.
* If List is empty return NULL.
*/
ListData PopBackList(List* l) {
  if(l == NULL) {
    return NULL;
  } else if (l->end == NULL) {
    return NULL;
  } else {
    ListData val = (l->end)->value;
    ListNode* new_end = (l->end)->left;
    if(new_end != NULL) {
      new_end->right = NULL;
    } else {
      (l->begin) = NULL;
    }
    MFREE(l->end);
    (l->end) = new_end;
    return val;
  }
}

/*
* Remove all elements of the given List
*/
void ClearList(List* l) {
  if(l == NULL) return;
  if(l->end != NULL) {
    FreeListRecLeft(l->end);
  }
  (l->begin)=NULL;
  (l->end)=NULL;
}

/*
* Deallocate List
*/
void FreeList(List* l) {
  if(l == NULL) return;
  //clearList(l);
  ListNode* it = l->begin;
  while(it != NULL) {
    ListNode* next = it->right;
    DBG {printf("FREE Lists.MFREEList %p\n", it);fflush(stdout);}
    MFREE(it);
    it = next;
  }
  l->begin = NULL;
  l->end = NULL;
  //MFREE(l);
}

/*
* Get first element data pointer or NULL if the List is empty
*/
ListData GetFirstListElement(const List* l) {
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
* Get lst element data pointer or NULL if the List is empty
*/
ListData GetLastListElement(const List* l) {
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
* Measure List size in O(n) time
*/
int GetListSize(const List* l) {
  if(l == NULL) return 0;
  int size = 0;
  LOOP_LIST(l, it) {
    ++size;
  }
  return size;
}

/*
* Check if List is empty
*/
int IsListEmpty( const List* l ) {
  if(l == NULL) return 1;
  return (l->begin == NULL && l->end == NULL);
}

/*
* Add all elements of <src> to the <tgt>
*/
void CopyListInto( const List* src, List* tgt ) {
  if(src == NULL) return;
  if(tgt == NULL) return;

  LOOP_LIST(src, it) {
    PushBackList(tgt, it->value);
  }
  return;


  /*if(tgt->end != NULL) {
    printf("END IS NONULL\n"); fflush(stdout);
    tgt->end->right = &arr[0];
  } else {
    printf("> END IS NULL\n"); fflush(stdout);
  }
  if(tgt->begin == NULL) {
    printf("> BEG IS NULL\n"); fflush(stdout);
    tgt->begin = &arr[0];
  }
  arr[0].left = tgt->end;
  arr[0].right = &arr[1];
  arr[src_size-1].right = NULL;
  arr[src_size-1].left = &arr[src_size-2];
  tgt->end = &arr[src_size-1];

  arr[0].value = 0x00;
  arr[src_size-1].value = 0x00;

  for(int i=1;i<src_size-1;++i) {
    arr[i].left = &arr[i-1];
    arr[i].right = &arr[i+1];
    arr[i].value = 0x00;
  }
  int i = 0;
  LOOP_LIST(src, iter) {
    arr[i].value = iter->value;
    ++i;
  }
  printf("DONE.\n"); fflush(stdout);
  */
}

/*
* Print List nodes starting at <l> and going right
*/
void PrintListNodes(const ListNode* l) {
  if(l==NULL) return;
  printf("%p; ", (ListData)(l->value));
  PrintListNodes(l->right);
}

/*
* Print List nodes of a given List
*/
void PrintList(const List* l) {
  printf("[ ");
  PrintListNodes(l->begin);
  printf("] ");
}

/*
* Print List nodes of a given List with trailing newline
*/
void PrintlnList(const List* l) {
  PrintList(l);
  printf("\n");
}

/*
* Make a copy of a List
*/
List CopyList( const List* l ) {
  if(l == NULL) return NewList();
  List ret = NewList();
  CopyListInto(l, &ret);
  return ret;
}


/*
* Maps (ListData)->(ListData) function on node <l> going to its right side
*/
void MapListNodes( ListNode* l, ListModifierFn mapping, int preserveValue ) {
  if(l==NULL) return;
  ListData ret = mapping(l->value);
  if(!preserveValue) {
    l->value = ret;
  }
  MapListNodes(l->right, mapping, preserveValue);
}

/*
* Iterate (ListData)->(ListData) function through the given List
*/
void IterateList(const List* l, ListModifierFn iterator) {
  if(l == NULL) return;
  if(l->begin == NULL) return;
  MapListNodes(l->begin, iterator, 1);
}

/*
* Maps (ListData)->(ListData) function on the given List
*/
void MapList(List* l, ListModifierFn mapping) {
  if(l == NULL) return;
  if(l->begin == NULL) return;
  MapListNodes(l->begin, mapping, 0);
}

/*
* Make a deep copy of a List
*/
List DeepCopyList( const List* l, ListModifierFn assigner ) {
  if(l == NULL) return NewList();
  List ret = CopyList(l);
  MapList(&ret, assigner);
  return ret;
}

/*
* Obtain first List element
*/
ListNode* GetListBegin( const List* l ) {
  if(l == NULL) return NULL;
  return l->begin;
}

/*
* Obtain last List element
*/
ListNode* GetListEnd( const List* l ) {
  if(l == NULL) return NULL;
  return l->end;
}

/*
* Remove given element from the List
*/
void DetachListElement( List* l, ListNode* node ) {
  if(node == NULL) return;

  ListNode* left_neighbour = node->left;
  ListNode* right_neighbour = node->right;

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
  DBG {printf("FREE Lists.detachListElement %p\n", node);fflush(stdout);}
  MFREE(node);
}

/*
* Create new element container not attached to any List.
*/
ListNode* NewListDetachedElement() {
  ListNode* ret = MALLOCATE(ListNode);
  DBG {printf("MALLOC Lists.newListDetachedElement %p\n", ret);fflush(stdout);}
  ret->left = NULL;
  ret->right = NULL;
  ret->value = NULL;
  return ret;
}

/*
* Check if is first/last element
*/
int IsListSideElement( ListNode* node ) {
  if(node == NULL) return 0;
  return ( (node->left == NULL) || (node->right == NULL) );
}

/*
* Check if is first element
*/
int IsListLeftSideEnd( ListNode* node ) {
  if(node == NULL) return 0;
  return (node->left == NULL);
}

/*
* Check if is last element
*/
int IsListRightSideEnd( ListNode* node ) {
  if(node == NULL) return 0;
  return (node->right == NULL);
}

/*
* Append List <src> to the left of <node> in List <tgt>
* leaving <src> empty.
* Tgt parameter MUST BE NON-NULL only if <node> is first/last element.
* Otherwise it may be NULL
*/
void InsertListAt( List* tgt, ListNode* node, List* src ) {
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
* Append a new value to the left of <node> in List <tgt>
* Tgt parameter MUST BE NON-NULL only if <node> is first/last element.
* Otherwise it may be NULL
*/
void InsertElementAt( List* tgt, ListNode* node, ListData value ) {
  List wrapper = NewList();
  PushBackList(&wrapper, value);
  InsertListAt(tgt, node, &wrapper);
  FreeList(&wrapper);
}


/*
* Transfer all right-side neighbours of <splitter> to a new List.
* Return that new List.
*/
List SplitList( List* src, ListNode* splitter ) {
  if(src == NULL) return NewList();
  List ret = NewList();
  if(splitter == NULL) {
    return ret;
  }

  ListNode* real_end = src->end;
  src->end = splitter;

  if(splitter->right != NULL) {
    ret.begin = splitter->right;
    ret.end = real_end;
    splitter->right = NULL;
    ret.begin->left = NULL;
  }
  return ret;
}

/*
* Get next element on the List.
* Returns NULL if node is the last element.
*/
ListNode* ListNext( ListNode* node ) {
  if(node==NULL) return NULL;
  return node->right;
}

/*
* Get previous element on the List.
* Returns NULL if node is the last element.
*/
ListNode* ListPrevious( ListNode* node ) {
  if(node==NULL) return NULL;
  return node->left;
}

/*
* Get value of the List element. Returns void pointer to underlying data.
* Returns NULL if node is NULL.
*/
ListData ListGetValue( ListNode* node ) {
  if(node==NULL) return NULL;
  return node->value;
}

/*
* Sets value of the List element.
* Does nothing if element is NULL.
*/
void ListSetValue( ListNode* node, ListData value ) {
  if(node==NULL) return;
  node->value = value;
}

/*
* Lists interface
*/
const ListsInterface LISTS = {
  .new = NewList,
  .free = FreeList,
  .pushFront = PushFrontList,
  .pushBack = PushBackList,
  .popFront = PopFrontList,
  .popBack = PopBackList,
  .clear = ClearList,
  .first = GetFirstListElement,
  .last = GetLastListElement,
  .size = GetListSize,
  .empty = IsListEmpty,
  .copy = CopyList,
  .copyInto = CopyListInto,
  .deepCopy = DeepCopyList,
  .iterate = IterateList,
  .print = PrintList,
  .println = PrintlnList,
  .begin = GetListBegin,
  .end = GetListEnd,
  .detachElement = DetachListElement,
  .newDetachedElement = NewListDetachedElement,
  .isSideElement = IsListSideElement,
  .insertListAt = InsertListAt,
  .insertElementAt = InsertElementAt,
  .isListEnd = IsListRightSideEnd,
  .isListBegin = IsListLeftSideEnd,
  .splitList = SplitList,
  .next = ListNext,
  .previous = ListPrevious,
  .getValue = ListGetValue,
  .setValue = ListSetValue
};
