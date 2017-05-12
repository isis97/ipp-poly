/** @file
*   Interface of polynomials
*
*   @author Jakub Pawlewicz <pan@mimuw.edu.pl>, Piotr Styczyński <piotrsty1@gmail.com>
*   @copyright Uniwersytet Warszawski
*   @date 2017-04-24, 2017-05-13
*/

#ifndef __STY_COMMON_POLY_H__
#define __STY_COMMON_POLY_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "list.h"

/**
* @def POLY_TO_STRING_BUF_SIZE
*
* Maximum length of polynomial string representation
* Used for buffer creation in PolyToStrnig family functions.
*/
#define POLY_TO_STRING_BUF_SIZE 700


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
  int c; ///< Constant (free) term of polynomial
  list monos; ///< Dynamically allocated list of monomials
} Poly;

/**
  * Representation of monomial
  * Monomials are in form of  `p * x^e`.
  * Coefficient `p` is a polynomial.
  * It's understood as a polynomial of another variable (not x).
  */
typedef struct Mono
{
  Poly p; ///< monomial coefficient
  poly_exp_t exp; ///< exponent of variable
} Mono;

/**
 * Creates a new const polynomial.
 *
 * @param[in] c : const value of polynomial
 * @return polynomial
 */
extern inline Poly PolyFromCoeff(poly_coeff_t c);

/**
 * Creates a new zero polynomial.
 * (const polynomial that is equal to 0)
 *
 * @return polynomial
 */
extern inline Poly PolyZero();

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
extern inline Mono MonoFromPoly(const Poly *p, poly_exp_t e);

/**
 * Checks if given polynomial is a coefficient
 * (i.e. is constant polynomial)
 *
 * @param[in] p : polynomial
 * @return If polynomial is const?
 */
extern inline bool PolyIsCoeff(const Poly *p);

/**
 * Checks if a given polynomial is const and equal to 0.
 *
 * @param[in] p : polynomial
 * @return If polynomial is equal to zero?
 */
extern inline bool PolyIsZero(const Poly *p);

/**
 * Removes polynomial from memory.
 *
 * @param[in] p : polynomial
 */
extern void PolyDestroy(Poly *p);

/**
 * Removes monomial from memory.
 *
 * @param[in] m : monomial
 */
extern inline void MonoDestroy(Mono *m);

/**
 * Performs deep-copy of a given polynomial.
 *
 * @param[in] p : polynomial
 * @return copy of a given polynomial
 */
extern Poly PolyClone(const Poly *p);

/**
 * Performs deep-copy of a given monomial
 *
 * @param[in] m : monomial
 * @return copy of a given monomial
 */
extern inline Mono MonoClone(const Mono *m);

/**
 * Adds two polynomials.
 * Returns standalone polynomial (deep-copied).
 *
 * @param[in] p : polynomial
 * @param[in] q : polynomial
 * @return `p + q`
 */
extern Poly PolyAdd(const Poly *p, const Poly *q);

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
extern Poly PolyAddMonos(unsigned count, const Mono monos[]);

/**
 * Multiplicates two polynomials.
 * Returns standalone polynomial (deep-copied).
 *
 * @param[in] p : polynomial
 * @param[in] q : polynomial
 * @return `p * q`
 */
extern Poly PolyMul(const Poly *p, const Poly *q);

/**
 * Returns a polynomial with changes sign.
 * It's standalone polynomial (deep-copied).
 *
 * @param[in] p : polynomial
 * @return `-p`
 */
extern Poly PolyNeg(const Poly *p);

/**
 * Substracts two polynomials.
 * Returns standalone polynomial (deep-copied).
 *
 * @param[in] p : polynomial
 * @param[in] q : polynomial
 * @return `p - q`
 */
extern Poly PolySub(const Poly *p, const Poly *q);

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
extern poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx);

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
extern poly_exp_t PolyDeg(const Poly *p);

/**
 * Checks equality of two polynomials.
 *
 * @param[in] p : polynomial
 * @param[in] q : polynomial
 * @return `p = q`
 */
extern bool PolyIsEq(const Poly *p, const Poly *q);

/**
 * Calculates value of polynomial in point @p x.
 * It's done by substituting first (main) variable of polynomial with @p x.
 * The result may be a polynomial if coefficients of @p p are polynomials.
 * If it is then te indexes of variables are decremented by 1.
 * Formally for polynomial @f$p(x_0, x_1, x_2, \ldots)@f$ the result
 * is a polynomial @f$p(x, x_0, x_1, \ldots)@f$.
 *
 * @param[in] p
 * @param[in] x
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
extern Poly PolyAt(const Poly *p, poly_coeff_t x);

 /**
  * Prints polynomial @p p to standard output (stdout)
  * using PolySprintf format.
  *
  * @param[in] p : polynomial
  */
extern void PolyPrint(const Poly* p);

/**
 * Gets const (free) term of a given polynomial.
 *
 * @param[in] p : polynomial
 * @return free term of the polynomial
 */
extern inline poly_coeff_t PolyGetConstTerm(const Poly* p);

 /**
  * Converts polynomial @p p to human-readable char sequence.
  * The result is saved in array pointed by @p dest.
  *
  * Polynomials are converted using the following assumptions:
  * - print only fully simplified terms of polynomial of form
  *     @f$C*x_0^a_0*x_1^a_1*\ldots*x_n^a_n@f$
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
extern void PolySprintf(char* dest, const Poly *p);

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
extern char* PolyToString(const Poly *p);


#endif /* __POLY_H__ */
