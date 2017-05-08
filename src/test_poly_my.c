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

  // 4a + 2a^2 + 4a^2*b + 6a^2*b^2
  Poly a = P(C(1), 1);
  PolyPrintlnCard(&a);
  PolyPrintln(&a);

  Poly b = P(C(-1), 1);
  PolyPrintlnCard(&b);
  PolyPrintln(&b);

  Poly c = PolyAdd(&a, &b);
  PolyPrintlnCard(&c);
  PolyPrintln(&c);

  Poly good = C(0);
  PolyPrintlnCard(&good);
  PolyPrintln(&good);

  Poly p = P(P(C(1), 1, C(1), 10), 1, P(C(1), 1, C(1), 2), 2);
  PolyPrintlnCard(&p);
  PolyPrintln(&p);
  printf("deg = %d\n", PolyDeg(&p));

  printf("%d\n", PolyIsEq(&c, &good));

  printf(":)\n");

  return 0;
}
