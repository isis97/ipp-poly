/** @file
*  Utilities for unit tests.
*
* This header file provides trap-helpers.
* The `utils.h` and `test_utils.h` symbiosis works as follows:
*  - `utils.h` change original calls to the trapped ones
*  - `test_utils.h` provided implementation of these functions
*
*  The `printf` function is for example trappes into `mock_printf`.
*  The trap function does not print anything but rather redirects
*  the output into the internal output buffer.
*
* Then you can call `main` from `calc_poly.c`
* (it will be trapped into `calculator_main`).
* Capture it output / provide input from string.
* And perform some unit testing. :)
*
* @author Piotr Styczyński <piotrsty1@gmail.com>
* @copyright MIT
* @date 2017-05-13
*/


#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <cmocka.h>
#include <assert.h>

#ifndef __STY_COMMON_TEST_UTILS_H__
#define __STY_COMMON_TEST_UTILS_H__

/**
 * @def ARRAY_LENGTH
 * Macro that calculates array Length
 * (does not work on pointer-degenerated arrays!)
 *
 * @param  x some array
 * @return   Length of array x
 */
#define ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))

/**
 * @def DISABLE_TRAPS
 *
 * Macro defined to make sure traps are enabled/disabled
 * in correct places of program.
 */
#ifdef DISABLE_TRAPS // Make sure traps are enabled
#undef DISABLE_TRAPS
#endif

#include "utils.h" // Install traps
#include "poly.h"  // All includes here are now trapped

#define DISABLE_TRAPS // Uninstall traps now
#include "utils.h"    // Uninstall traps
#undef DISABLE_TRAPS



/**
 * Assersion checking equality of two polynomials
 *
 * @param  POLY_A : Polynomial to be compared
 * @param  POLY_B : Polynomial to be compared
 * @return If the given polynomials are equal?
 */
#define assert_poly_equal(POLY_A, POLY_B) \
if(!PolyIsEq(POLY_A, POLY_B)) { \
  char a_str[250]; \
  char b_str[250]; \
  PolySprintf(a_str, (POLY_A)); \
  PolySprintf(b_str, (POLY_B)); \
  fail_msg("Poly {%s} != {%s}", a_str, b_str); \
}

/**
 * Main function trap for unit test.
 * The normal main function is changed into `calculator_main`.
 *
 * @param  argc : Pass argc main argument
 * @param  argv : Pass argv main argument
 * @return Program exit code
 */
extern int calculator_main(int argc, char *argv[]);

/**
 * Trap function for scanf.
 * Returns data from internal buffer.
 *
 * @param  format : Format string
 * @return (as scanf) number of assigned fields.
 */
int mock_scanf(const char *format, ...);

/**
 * Getchar function trap.
 * Returns data from internal buffer.
 *
 * @return Character read from stdin.
 */
int mock_getchar();

/**
 * Printf function trap.
 * Captures output and stores in internal buffer.
 *
 * @param  format : Format string
 * @return (as printf) number of printed characters.
 */
int mock_printf(const char *format, ...);

/**
 * Fprintf function trap.
 * Captures output and stores in internal buffer.
 *
 * @param  file   : Output stream
 * @param  format : Format string
 * @return (as printf) number of printed characters.
 */
int mock_fprintf(FILE* const file, const char *format, ...);

/**
 * Clears internal output buffer for `fprintf`.
 */
void mock_clear_fprintf_buffer();

/**
 * Clears internal output buffer for `printf`.
 */
void mock_clear_printf_buffer();

/**
 * Clears internal output buffer for `scanf`.
 */
void mock_clear_scanf_buffer();

/**
 * Clears all internal buffers.
 */
void mock_clear_all_buffers();

/**
 * Return contents of internal `fprintf` buffer.
 *
 * @return Contents of internal buffer
 */
char* mock_get_fprintf_buffer();

/**
 * Return contents of internal `printf` buffer.
 *
 * @return Contents of internal buffer
 */
char* mock_get_printf_buffer();

/**
 * Puts contents in internal `scanf` input buffer.
 *
 * @param str : Input string
 */
void mock_set_scanf_buffer(const char* str);

/**
 * Helper to call `calculator_main` with dummny args and
 * provided string input.
 *
 * The output is then compared to the provided expected output
 * and the expected exit code must match with the returned one.
 *
 * If one of the expectancy is not met then some assertion here fails.
 *
 * @param stdin_input        : Provided input (stdin) string
 * @param stdout_expected    : Expected (stdout) output
 * @param stderr_expected    : Expected (stderr) output
 * @param exit_code_expected : Expected exit code
 */
static inline void mock_run_calc_main( const char * stdin_input, const char* stdout_expected, const char * stderr_expected, const int exit_code_expected ) {
  const char *args[] = { "calc_poly" };
  mock_clear_all_buffers();
  mock_set_scanf_buffer(stdin_input);

  int exit_code = calculator_main(ARRAY_LENGTH(args), (char **)args);
  assert_int_equal(exit_code, exit_code_expected);
  assert_string_equal(mock_get_printf_buffer(), stdout_expected);
  assert_string_equal(mock_get_fprintf_buffer(), stderr_expected);
}

#endif
