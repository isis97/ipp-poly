#include <stdio.h>
#include "poly.h"
#include "list.h"
#include "poly.c"
#include "list.c"

int main() {

  printf("Hello test.\n");
  /*
    4a + 2a^2 + 4a^2*b + 6a^2*b^2
  */
  Poly a = PolyAddMonos(2, (Mono[2]) {
    {
      .exp = 1,
      .p = PolyFromCoeff(4)
    },
    {
      .exp = 2,
      .p = PolyAddMonos(3, (Mono[3]) {
        {
          .exp = 0,
          .p = PolyFromCoeff(2)
        },
        {
          .exp = 1,
          .p = PolyFromCoeff(4)
        },
        {
          .exp = 2,
          .p = PolyFromCoeff(6)
        }
      })
    }
  });
  PolyPrint(&a);
  /*
   4a^3 + (2b^3 + 4b^4)*a^2 + 2a
  */
  Poly b = PolyAddMonos(3, (Mono[3]) {
    {
      .exp = 3,
      .p = PolyFromCoeff(4)
    },
    {
      .exp = 2,
      .p = PolyAddMonos(2, (Mono[2]) {
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
  Poly c = PolyMul(&a, &b);
  printf("MUL DONE\n");fflush(stdout);
  PolyPrint(&c);
  //Poly b = PolyFromCoeff(4);
  printf(":)\n");

  return 0;
}
