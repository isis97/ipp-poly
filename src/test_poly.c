#include <stdio.h>
#include "poly.h"
#include "list.h"
#include "poly.c"
#include "list.c"

int main() {

  printf("Hello test.\n");
  Poly a = PolyFromCoeff(3);
  PolyPrint(&a);
  //Poly b = PolyFromCoeff(4);
  printf(":)\n");

  return 0;
}
