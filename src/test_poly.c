#include <stdio.h>
#include "poly.h"
#include "list.h"
#include "poly.c"
#include "list.c"

int main() {

  printf("Hello test.\n");
  Poly a = PolyFromCoeff(3);
  PolyPrint(&a);
  /*
   4a^3 + (2b^3 + 4b^4)*a^2 + 2a
  */
  Poly b = PolyAddMonos(3, (Mono[]) {
    {
      .exp = 3,
      .p = PolyFromCoeff(4)
    },
    {
      .exp = 2,
      .p = PolyAddMonos(2, (Mono[]) {
        {
          .exp = 3,
          .p = PolyFromCoeff(2)
        },
        {
          .exp = 4,
          .p = PolyFromCoeff(4)
        }
      })
    },
    {
      .exp = 1,
      .p = PolyFromCoeff(2)
    }
  });
  PolyPrint(&b);
  //Poly b = PolyFromCoeff(4);
  printf(":)\n");

  return 0;
}
