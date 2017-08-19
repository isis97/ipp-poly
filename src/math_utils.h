/** @file
*  Collection of useful mathematical utilities.
*
*  @author Piotr Styczyński <piotrsty1@gmail.com>
*  @copyright MIT
*  @date 2017-05-13
*/
#include "utils.h"
#include <stdio.h>

#ifndef __STY_COMMON_MATH_UTILS_H__
#define __STY_COMMON_MATH_UTILS_H__

/**
* Fast exponentiation for long values.
* Assumes 0^0 = 0
* 
* @param[in] value : input value
* @param[in] exp   :  input exponent
* @return value ^ exp
*/
static inline long MathFastPowLong(long value, long exp) {
  // Special cases when we do not any calculation
  if(value == 0) return 0;     // 0^y = 0
  if(exp == 0) return 1;       // x^0 = 1 (x!=0)
  if(exp == 1) return value;   // x^1 = x
  if(value == 1) return 1;     // 1^y = 1
  if(value == -1) {
    if(exp % 2 == 0) return 1; // (-1)^y = 1  (  2|y  ) 
    return -1;                 // (-1)^y = -1 (not 2|y)
  }
  // Fast exponentiation implementation
  long result = 1;
  long base = value;
  while (exp) {
    if (exp & 1) {
      result = result * base;
      exp -= 1;
    }
    exp >>= 1;
    base = base * base;
  }
  return result;
}


#endif /* __STY_COMMON_MATH_UTILS_H__ */
