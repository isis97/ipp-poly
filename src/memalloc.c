/*
*  Error-detecing memory allocators.
*
*  @author Piotr Styczyński <piotrsty1@gmail.com>
*  @copyright MIT
*  @date 2017-05-13
*/
#include <errno.h>
#include <assert.h>
#include <stdlib.h>

/*
* Allocate block of `size` bytes.
*/
void* AllocateMemoryBlock(int size) {
  assert(size > 0);

  int old_errno = errno;
  errno = 0;
  void* data = malloc(size);
  assert(errno == 0);
  assert(data != NULL);
  errno = old_errno;

  return data;
}

/*
* Allocate array of `count` blocks each of `size` bytes.
*/
void* AllocateMemoryBlockArray(int count, int size) {
  assert(count > 0);
  assert(size > 0);

  int old_errno = errno;
  errno = 0;
  void* data = malloc(size * count);
  assert(errno == 0);
  assert(data != NULL);
  errno = old_errno;

  return data;
}

/*
* Free block
*/
void FreeMemoryBlock(void* p) {
  assert(p != NULL);
  int old_errno = errno;
  errno = 0;
  free(p);
  assert(errno == 0);
  errno = old_errno;
}
