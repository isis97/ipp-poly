/** @file
*   Interface of polynomials
*
*   @author Piotr Styczyński <piotrsty1@gmail.com>
*   @copyright MIT
*   @date 2017-05-13
*/
#ifndef __STY_COMMON_POLY_H__
#define __STY_COMMON_POLY_H__

#include "utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include "memalloc.h"
#include "dynamic_lists.h"

/**
* @def POLY_TO_STRING_BUF_SIZE
*
* Maximum length of polynomial string representation
* Used for buffer creation in PolyToStrnig family functions.
*/
#define POLY_TO_STRING_BUF_SIZE 700

/**
 * @def PolyC
 *
 * Macro for creating constant poly from given value.
 * It's sugar syntax for writing `PolyFromCoeff`
 * (yeah I made it, I saved 8 bytes!) :))
 *
 * It's for use with PolyP - the calls looks much cleaner.
 * Example usage:
 * @code
 *
 *    // Constant polynomial
 *    Poly p1 = PolyC(12);
 *
 *    // One variable polynomials
 *    Poly p2 = PolyP( PolyC(5), 2, PolyC(5), 4 ); // 5a^2 + 5a^4
 *
 *    // Nested polynomials
 *    Poly p3 = PolyP( PolyC(5), 2, PolyP( PolyC(2), 2 ), 4 ); // 5a^2 + (2b^2)*a^4
 *
 *    PolyDestroy(&p1); //
 *    PolyDestroy(&p2); // Cleanup
 *    PolyDestroy(&p3); //
 *
 * @endcode
 */
#define PolyC PolyFromCoeff

/**
 * @def PolyP
 *
 * Macro for creating polynomials from given mono lists.
 * It's syntax sugar for calling PolyBuild.
 *
 * Input list must be the form of:
 *
 * POLY1, EXP1, POLY2, EXP2 ...
 *
 * Each polynomial is paired with matching exponent value (poly_exp_t).
 *
 * The function utilizes PolyAddMonos functionality.
 *
 * The resulting poly has the form of:
 * `(POLY1)t^(EXP1) + (POLY2)t^(EXP2) ...`
 *
 * It's for use with PolyC and Poly0 - the calls looks much cleaner.
 * Example usage:
 * @code
 *
 *    // Constant polynomial
 *    Poly p1 = PolyC(12);
 *
 *    // One variable polynomials
 *    Poly p2 = PolyP( PolyC(5), 2, PolyC(5), 4 ); // 5a^2 + 5a^4
 *
 *    // Nested polynomials
 *    Poly p3 = PolyP( PolyC(5), 2, PolyP( PolyC(2), 2 ), 4 ); // 5a^2 + (2b^2)*a^4
 *
 *    PolyDestroy(&p1); //
 *    PolyDestroy(&p2); // Cleanup
 *    PolyDestroy(&p3); //
 *
 * @endcode
 *
 */
#define PolyP(...) PolyBuild(0, __VA_ARGS__, PolyZero(), -1)

/**
 * @def Poly0
 *
 * Macro for creating constant zero poly.
 *
 * It's for use with PolyP and PolyC - the calls looks much cleaner.
 * Example usage:
 * @code
 *
 *    // Constant zero polynomial
 *    Poly p1 = Poly0;
 *
 *    // One variable polynomials
 *    Poly p2 = PolyP( PolyC(5), 2, Poly0, 4 ); // 5a^2 + 5a^4
 *
 *    // Nested polynomials
 *    Poly p3 = PolyP( PolyC(5), 2, PolyP( PolyC(2), 2 ), 4 ); // 5a^2 + (2b^2)*a^4
 *
 *    PolyDestroy(&p1); //
 *    PolyDestroy(&p2); // Cleanup
 *    PolyDestroy(&p3); //
 *
 * @endcode
 */
#define Poly0 (PolyZero())

/**
 * @def PolyL
 *
 * Macro for creating array of polynomials using list initializer.
 *
 * It's for use with PolyP and PolyC family - the calls looks much cleaner.
 * Example usage:
 * @code
 *
 *    Poly parr[] = PolyL( PolyC(2), PolyP( PolyC(2), 2 ) );
 *    // Array containing 2 and 2a^2
 *
 *    // Cleanup the array contents
 *    PolyDestroyArray(&parr);
 *
 * @endcode
 */
#define PolyL(...) ((Poly[]){__VA_ARGS__})



/** Type of polynomial coefficients */
typedef long poly_coeff_t;

/** Type of polynomial exponents */
typedef int poly_exp_t;

/**
 * Representation of polynomial
 * All polynomials are build from their constant term
 * and list of monomials (of variable powers higher than 0)
 */
typedef struct Poly {
  poly_coeff_t c; ///< Constant (free) term of polynomial
  List monos; ///< Dynamically allocated list of monomials
} Poly;

/**
  * Representation of monomial
  * Monomials are in form of  `p * x^e`.
  * Coefficient `p` is a polynomial.
  * It's understood as a polynomial of another variable (not x).
  */
typedef struct Mono {
  Poly p; ///< monomial coefficient
  poly_exp_t exp; ///< exponent of variable
} Mono;

/** Iterator for iterating through monos of poly **/
typedef Mono* PolyIterator;

/**
 * Iterator function interacting through monos indexed collection.
 * It's used to iterate over monos.
 * For each call there's the mono value that is currently iterated and the
 * index number.
 *
 * All monos in poly are ordered from lower exponents to the higher.
 * They are indexed as C arrays starting from 0.
 *
 * @param[in] index : Index of monomial
 * @param[in] mono : Monomial value
 */
typedef void (*PolyMonosInterator)(int index, Mono mono);


/**
 * Creates a new const polynomial.
 *
 * @param[in] c : const value of polynomial
 * @return polynomial
 */
static inline Poly PolyFromCoeff(poly_coeff_t c) {
  return (Poly){ .c = c, .monos = ListNew() };
}

/**
 * Creates a new zero polynomial.
 * (const polynomial that is equal to 0)
 *
 * @return zero polynomial
 */
static inline Poly PolyZero() {
  return (Poly){ .c = 0, .monos = ListNew() };
}

/**
 * Creates a new zero monomial 0*x^0.
 *
 * @return zero monomial
 */
static inline Mono MonoZero() {
  return (Mono) { .exp = 0, .p = PolyZero() };
}

/**
 * Creates monomial `p * x^e`.
 * Captures data pointed by @p p.
 *
 * Note: Capturing data means that you cannot now
 *       reference/access this data manually
 *       (only through newly created object)
 *
 * @param[in] p : polynomial - coefficient of new monomial
 * @param[in] e : exponent
 * @return monomial `p * x^e`
 */
static inline Mono MonoFromPoly(const Poly *p, poly_exp_t e) {
    return (Mono) { .exp = e, .p = *p };
}

/**
 * Iterates over all monomials included in the given polynomial.
 *
 * For each call of interating function there's given the mono value
 * that is currently iterated and the index number.
 *
 * All monos in poly are ordered from lower exponents to the higher.
 * They are indexed as C arrays starting from 0.
 *
 * @param[in] p        : Poly to be interated
 * @param[in] iterator : Iterator function that will be called for all monomials
 */
static inline void PolyIterateMonos(Poly* p, PolyMonosInterator iterator) {
  int j = 0;
  if(p->c != 0) {
    iterator(j, (Mono){ .exp = 0, .p = PolyFromCoeff(p->c) });
    ++j;
  }
  LOOP_LIST(&(p->monos), i) {
    Mono* m = (Mono*) ListGetValue(i);
    iterator(j, *m);
    ++j;
  }
}

/**
 * Checks if given polynomial is a coefficient
 * (i.e. is constant polynomial)
 *
 * @param[in] p : polynomial
 * @return If polynomial is const?
 */
static inline bool PolyIsCoeff(const Poly *p) {
  assert(p != NULL);
  return ListEmpty(&(p->monos));
}

/**
 * Extract polynomial free term.
 *
 * @param[in]  : p Input polynomial
 * @return Constant (free) term of polynomial
 */
static inline poly_coeff_t PolyGetConstTerm(const Poly* p) {
  return p->c;
}

/**
 * Checks if a given polynomial is const and equal to 0.
 *
 * @param[in] p : polynomial
 * @return If polynomial is equal to zero?
 */
static inline bool PolyIsZero(const Poly *p) {
  assert(p!=NULL);
  if(!PolyIsCoeff(p)) return false;
  return p->c == 0;
}


/**
 * Removes polynomial from memory.
 *
 * @param[in] p : polynomial
 */
void PolyDestroy(Poly *p);

/**
 * Removes monomial from memory.
 *
 * @param[in] m : monomial
 */
static inline void MonoDestroy(Mono *m) {
  if(m==NULL) return;
  PolyDestroy(&(m->p));
}

/**
 * Shallow-copy value to the given polynomial pointer freeing old one.
 *
 * WARN: This function IS VERY UNSAFE inproperly used.
 *       Please read the following documentation:
 *
 * In normal case assigning pointer value without clean left memory leaks.
 * Consider this bad pracitice example:
 * @code
 *   Poly a = PolyP( PolyC(2), 2 );
 *   Poly b = PolyP( PolyC(5), 0, PolyC(5), 3 );
 *
 *   a = b; // MEMORY LEAK
 *          // An original polynomial cannot be freed now
 *
 *   PolyDestroy(&a);
 *   PolyDestroy(&b);
 * @endcode
 *
 * Now you can replace the value (assign new one):
 * @code
 *   Poly a = PolyP( PolyC(2), 2 );
 *   Poly b = PolyP( PolyC(5), 0, PolyC(5), 3 );
 *
 *   PolyReplace(&a, b); // a = b assigment with memory managing
 *
 *   PolyDestroy(&a); // Now free new value of a
 * @endcode
 *
 * Now notice that:
 *
 *   - This function frees old pointer value so multiple pointers to one
 *     polynomial may be problematic as you use PolyReplace
 *     (the value under pointer would be destroyed :<)
 *
 *   - It DOES NOT perform deep copy instead it's shallow copy of given value
 *     that's why IT'S VERY UNSAFE when used multiple times on same value. :<
 *
 * @param[in] p        : Pointer to be assigned
 * @param[in] newValue : New value of that pointer
 */
static inline void PolyReplace(Poly* p, Poly newValue) {
  PolyDestroy(p);
  *p = newValue;
}

/**
 * Performs deep-copy of a given polynomial.
 *
 * @param[in] p : polynomial
 * @return copy of a given polynomial
 */
Poly PolyClone(const Poly *p);

/**
 * Performs deep-copy of a given monomial
 *
 * @param[in] m : monomial
 * @return copy of a given monomial
 */
static inline Mono MonoClone(const Mono *m) {
  assert(m!=NULL);
  return (Mono) {.exp = m->exp, .p = PolyClone(&(m->p))};
}

/**
 * Adds two polynomials.
 * Returns standalone polynomial (deep-copied).
 *
 * @param[in] p : polynomial
 * @param[in] q : polynomial
 * @return `p + q`
 */
Poly PolyAdd(const Poly *p, const Poly *q);

/**
 * Sums array of monomials of size @p count.
 * Captures data pointed by @p monos array.
 *
 * Note: Capturing data means that you cannot now
 *       reference/access objects of @p monos
 *       directly (only through returned polynomial)
 *       But array itself is not captured so you
 *       MUST FREE IT MANUALLY.
 *
 * @param[in] count : number of monomials
 * @param[in] monos : array of monomials
 * @return polynomial that is a sum of given monomials
 */
Poly PolyAddMonos(unsigned count, const Mono monos[]);

/**
 * Multiplicates two polynomials.
 * Returns standalone polynomial (deep-copied).
 *
 * @param[in] p : polynomial
 * @param[in] q : polynomial
 * @return `p * q`
 */
Poly PolyMul(const Poly *p, const Poly *q);

/**
 * Returns a polynomial with changes sign.
 * It's standalone polynomial (deep-copied).
 *
 * @param[in] p : polynomial
 * @return `-p`
 */
Poly PolyNeg(const Poly *p);

/**
 * Substracts two polynomials.
 * Returns standalone polynomial (deep-copied).
 *
 * @param[in] p : polynomial
 * @param[in] q : polynomial
 * @return `p - q`
 */
Poly PolySub(const Poly *p, const Poly *q);

/**
 * Determines the degree of polynomial with respect to the given variable
 * (-1 for zero polynomial).
 * Variables are indexed starting from 0.
 * Variable with 0 index means the main variable of that polynomial.
 * Higher numbers means the variables contained in coefficients of
 * that polynomial.
 *
 * You can understand it as recrusrion. All variables in polynomial
 * coefficients that are not nested in any others are these with 0 index.
 * All nested directly in them are these with numbered with 1.
 * All nexted in directly them are these ... with 2. etc.
 *
 * For variables index higher than degree of polynomial 0 is returned
 * - or -1 if the polynomial itself is a zero one
 * (polynomial is const for that variable as in normal math)
 *
 * @param[in] p : polynomial
 * @param[in] var_idx : index of variable
 * @return degree of polynomial @p p with respect to variable with index @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx);

/**
 * Returns degree of polynomial
 * (-1 for zero polynomial)
 *
 * Degree of polynomial is a degree of polynomial in witch all variables are
 * supposed to be equal. (maximum of sum of exponents of all variables)
 *
 * @param[in] p : polynomial
 * @return degree if polynomial @p p
 */
poly_exp_t PolyDeg(const Poly *p);

/**
 * Checks equality of two polynomials.
 *
 * @param[in] p : polynomial
 * @param[in] q : polynomial
 * @return `p = q`
 */
bool PolyIsEq(const Poly *p, const Poly *q);

/**
 * Calculates value of polynomial in point @p x.
 * It's done by substituting first (main) variable of polynomial with @p x.
 * The result may be a polynomial if coefficients of @p p are polynomials.
 * If it is then te indexes of variables are decremented by 1.
 * Formally for polynomial @f$p(x_0, x_1, x_2, \ldots)@f$ the result
 * is a polynomial @f$p(x, x_0, x_1, \ldots)@f$.
 *
 * Returned value is standalone polynomial (deep-copied).
 *
 * @param[in] p
 * @param[in] x
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
Poly PolyAt(const Poly *p, poly_coeff_t x);

/**
* Prints polynomial @p p to standard output (stdout)
* using PolySprintf format.
*
* @param[in] p : polynomial
*/
void PolyPrint(const Poly* p);

/**
 * Gets const (free) term of a given polynomial.
 *
 * @param[in] p : polynomial
 * @return free term of the polynomial
 */
poly_coeff_t PolyGetConstTerm(const Poly* p);

/**
* Converts polynomial @p p to human-readable char sequence.
* The result is saved in array pointed by @p dest.
*
* Polynomials are converted using the following assumptions:
* - print only fully simplified terms of polynomial of form
*     @f$C*x_0^{a_0}*x_1^{a_1}*\ldots*x_n^{a_n}@f$
* - encode variables using charactes a..z
*     Variable with index 0 is a, 1 is b etc.
* - encode math operators in ascii manner e.g `x^a * x^b`
* - skip 1 wherever possible
* - do not print terms multiplied by 0
* - do not print sign wherever possible
*
* @param[in] dest : char array pointer
* @param[in] p : polynomial
*/
void PolySprintf(char* dest, const Poly *p);

/**
 * Converts polynomial @p p to human-readable char sequence.
 * Uses PolySprintf formatting.
 * The generated char-sequence is held in null-terminated array
 * returned by the function.
 *
 * WARN: You must free returned array when it is not needed anymore.
 *
 * @param[in] p : polynomial
 * @return char* containing generated string
 */
char* PolyToString(const Poly *p);

/**
 * Add one monomial to the given polynomial.
 *
 * This function modifies existing polynomial to contain also the given mono.
 * You must notice that the monomial captures the given monomial.
 *
 * Note: Capturing data means that you cannot now
 *       reference/access this data manually
 *       (only through newly created object)
 *
 * It's because this function DOES NOT PERFORM DEEP-COPY of monomial.
 *
 * @param p    Input polynomial
 * @param mono Monomial to be inserted
 */
void PolyInsertMono(Poly* p, Mono mono);

/**
 * Calculate polynomial exponent.
 *
 * This function calculates `poly ^ exp`.
 * Obviously exp is only a number if it would be a polynomial
 * then the result would not be polynomial anymore.
 *
 * Returned value is standalone polynomial (deep-copied).
 *
 * @param[in]  p   Input polynomial
 * @param[in]  exp Exponent value
 * @return     Polynomial power.
 */
Poly PolyPow(const Poly* p, poly_exp_t exp);

/**
 * Compose given polynomials to one polynomial.
 * Composition take place as follows:
 * - each occurence of main variable of polynomial
 *   is substituted with the matching polynomial from array
 *   It means that in place of the first occurence of main variable
 *   we put the first polynomial from array.
 *   The occurences of main variable are ordered
 *   as the monomials (in rising exponent order).
 * - if there's no matching polynomial for main variable occurence then
 *   we assume it's poly equal to zero so all the monomial will be zeroed.
 *
 * Example:
 * @code
 *    Poly p = PolyP( PolyC(2), 1, PolyC(4), 2, PolyP( PolyC(1), 2 ), 3 );
 *    // p = 2*a + 4*a^2 + (b^2)*a^3
 *
 *    Poly list[] = { PolyC(1), PolyP( PolyC(2), 1 ) };
 *    // list[0] = 1
 *    // list[1] = 2a
 *
 *    // Call compose
 *    Poly composed = PolyCompose(p, 1, list);
 *
 *    //
 *    // p = 2*a + 4*a^2 + (b^2)*a^3
 *    // \/
 *    // p = 2*(a_1) + 4*(a_2)^2 + (b^2)*(a_3)^3
 *    //
 *    // compose do the following substitutions:
 *    //   a_1 -> list[0] Present
 *    //   a_2 -> list[1] Present
 *    //   a_3 -> list[2] Not available so assume = 0
 *    //
 *    // So:
 *    //   a_1 -> 1
 *    //   a_2 -> 2a
 *    //   a_3 -> 0
 *    //
 *    // Then the resulting polynomial is:
 *    //   composed =  2*(1) + 4*(2a)^2 + (b^2)*(0)^3
 *    //   composed =  2 + 16a^2
 *    //
 *
 *    // Now we must free the list
 *    PolyDestroyArray(list);
 *
 *    // And the polynomials
 *    PolyDestroy(&p);
 *    PolyDestroy(&composed);
 *
 * @endcode
 *
 * @param[in]  p     Input polynomial
 * @param[in]  count Length of substitution list
 * @param[in]  x     Substitution list
 * @return       Poly composed from input polyonimal and the given list
 */
Poly PolyCompose(const Poly *p, unsigned count, const Poly x[]);

/**
 * Variadic function to create poly from given input.
 * Input must be the form of:
 *
 * POLY1, EXP1, POLY2, EXP2 ...
 *
 * Each poly is paired with matching exponent value (poly_exp_t).
 *
 * The function utilizes PolyAddMonos functionality.
 *
 * The resulting poly has the form of:
 * `(POLY1)t^(EXP1) + (POLY2)t^(EXP2) ...`
 *
 * @param[in]  dummy   Dummy int parameter (for variadic compatibility)
 * @return         Poly build from the provided input
 */
static inline Poly PolyBuild(int dummy, ...) {
    va_list list;
    va_start(list, dummy);
    unsigned count = 0;
    while (true) {
        va_arg(list, Poly);
        if (va_arg(list, int) < 0)
            break;
        count++;
    }
    va_start(list, dummy);
    Mono *arr = calloc(count, sizeof(Mono));
    for (unsigned i = 0; i < count; ++i) {
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

/**
 * Destroys the array of polynomials.
 * It DOES NOT free the array itself, but destroy its all polynomial elements.
 *
 * @param[in] length Length of array to be destroyed
 * @param[in] parr   Array of polynomials
 */
static inline void PolyDestroyArray(const int length, Poly* parr) {
  for(int i=0;i<length;++i) {
    PolyDestroy(parr);
  }
}

#endif /* __POLY_H__ */
