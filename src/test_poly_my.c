#include "list.h"
#include "poly.h"
#include "poly.c"
#include "list.c"
#include <stdarg.h>


#define C PolyFromCoeff
Poly MakePolyHelper(int dummy, ...)
{
    va_list list;
    va_start(list, dummy);
    unsigned count = 0;
    while (true)
    {
        va_arg(list, Poly);
        if (va_arg(list, int) < 0)
            break;
        count++;
    }
    va_start(list, dummy);
    Mono *arr = calloc(count, sizeof(Mono));
    for (unsigned i = 0; i < count; ++i)
    {
        Poly p = va_arg(list, Poly);
        arr[i] = MonoFromPoly(&p, va_arg(list, int));
        assert(i == 0 || arr[i].exp > arr[i - 1].exp);
    }
    Poly closing_zero = va_arg(list, Poly);
    va_end(list);
    PolyDestroy(&closing_zero);
    Poly res = PolyAddMonos(count, arr);
    free(arr);
    return res;
}
#define P(...) MakePolyHelper(0, __VA_ARGS__, PolyZero(), -1)





int main() {

  printf("Hello test.\n");

  Poly p = P(C(5), 0, P(C(2), 0, C(5), 2), 1, P(C(6), 0, C(6), 5, C(7), 7), 2);
  PolyPrint(&p);printf("\n");
  Poly p2 = PolyAt(&p, 3);
  PolyPrint(&p2);printf("\n");
  Poly p3 = PolyAt(&p2, 1);
  PolyPrint(&p3);printf("\n");
  Poly p4 = PolyAt(&p3, 0);
  PolyPrint(&p4);printf("\n");
  printf(":)\n");

  PolyDestroy(&p);
  PolyDestroy(&p2);
  PolyDestroy(&p3);
  PolyDestroy(&p4);

  return 0;
}
