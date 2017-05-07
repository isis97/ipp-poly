#include "strings.h"
#include <stdlib.h>
#include <string.h>


struct stringData {
  char* data;
  int length;
  int allocLength;
};

string newString() {
  string s = (string) malloc(sizeof(stringData));
  s->data = (char*) malloc(STRINGS_DEFAULT_ALLOC_SIZE*sizeof(char)); //TODO: WARN
  s->allocLength = STRINGS_DEFAULT_ALLOC_SIZE;
  s->length = 0;
  return s;
};

void resizeStringToLength(string s, int newSize) {
  if(s->allocLength >= newSize) return;
  s->data = (char*) realloc(s->data, newSize*sizeof(char));
  s->allocLength = newSize;
}

void resizeString(string s) {
  resizeStringToLength(s, s->allocLength+(s->allocLength)/2);
}

void freeString(string s) {
  free(s->data);
  free(s);
}

void appendToString(string a, string b) {
  resizeStringToLength(a, a->length+b->length+7);
  strcat(a->data, b->data);
  a->length += b->length;
}

void clearString(string a) {
  for(int i=0;i<a->allocLength;++i) {
    (a->data)[i] = 0x00;
  }
  a->length = 0;
}

string copyString(string a) {
  string b = newString();
  resizeStringToLength(b, a->allocLength);
  strcpy(a->data, b->data);
  b->length = a->length;
  return b;
}

string joinStrings(string a, string b) {
  string c = copyString(a);
  appendToString(c, b);
  return c;
}

int sizeOfString(string a) {
  return a->length;
}

string arrToString(char* str) {
  const int len = (int) strlen(str);
  string ret = newString();
  resizeStringToLength(ret, len+7);
  for(int i=0;i<len;++i) {
    (ret->data)[i] = str[i];
  }
  ret->length = len;
  return ret;
}

char* stringToArr( string s ) {
  return s->data;
}

const strings Strings = {
  .new = newString,
  .free = freeString,
  .appendTo = appendToString,
  .clear = clearString,
  .copy = copyString,
  .join = joinStrings,
  .size = sizeOfString,
  .arrToString = arrToString,
  .toArr = stringToArr
};
