#include "utils.h"

#ifndef STY_COMMON_STRINGS_H__
#define STY_COMMON_STRING_H__

#define STRINGS_DEFAULT_ALLOC_SIZE 107

typedef struct stringData stringData;
typedef struct strings strings;

typedef stringData* string;

struct strings {
  string (*new)();
  void (*free) ( string s );
  void (*appendTo) ( string a, string b );
  void (*clear) ( string a );
  string (*copy) ( string a );
  string (*join) ( string a, string b );
  int (*size) ( string a );
  string (*arrToString) ( char* arr );
  char* (*toArr) ( string a );
};

/*
* Lists interface object
* All functions should be accessed using Lists object
*/
const struct strings Strings;

#endif
