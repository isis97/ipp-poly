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
 * 
 * @param  value input value
 * @param  exp   input exponent
 * @return       value ^ exp
 */
static inline long MathFastPowLong(long value, long exp) {
  if(exp == 0) return 1;
  if(exp == 1) return value;
  if(value == 0) return 0;
  if(value == 1) return 1;
  if(value == -1) {
    if(exp % 2 == 0) return 1;
    return -1;
  }
  long result = value;
  long base = value;
  while (exp) {
    if (exp & 1)
      result = result * base;
    exp >>= 1;
    base = base * base;
  }
  return result;
}


#endif /* __STY_COMMON_MATH_UTILS_H__ */
