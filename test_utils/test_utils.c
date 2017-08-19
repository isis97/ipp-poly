/*
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
#include "test_utils.h"
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
* Export code only if its UNIT_TESTING mode
*/
#ifdef UNIT_TESTING

/*
* Buffers that mocked printf/fprintf/scanf functions write to
*/
char fprintf_buffer[256];
char printf_buffer[256];
char scanf_buffer[256];

/*
* And the buffers' corresponding write iterators
*/ 
int fprintf_position = 0;
int printf_position = 0;
int scanf_position = 0;


/* Mocked scanf function that writes to scanf_buffer */
int mock_scanf(const char *format, ...) {
    int return_value;
    va_list args;

    /* Checks if buffer iterator is valid */
    assert_true((size_t)scanf_position < sizeof(scanf_buffer));

    va_start(args, format);
    return_value = vsscanf(scanf_buffer + scanf_position,
                             format,
                             args);
    va_end(args);

    scanf_position += return_value;
    assert_true((size_t)scanf_position < sizeof(scanf_buffer));
    return return_value;
}

/* Mocked getchar that writes to scanf_buffer */
int mock_getchar() {
  if(scanf_position >= (int)sizeof(scanf_buffer)) return EOF;
  if(scanf_buffer[scanf_position] == EOF) return EOF;
  char c;
  c = *((char*)(scanf_buffer + scanf_position));
  ++scanf_position;
  return c;
}

/* Mocked printf that writes to printf_buffer */
int mock_printf(const char *format, ...) {
    int return_value;
    va_list args;

    /* Chcecks if buffer iterator is valid */
    assert_true((size_t)printf_position < sizeof(printf_buffer));

    va_start(args, format);
    return_value = vsnprintf(printf_buffer + printf_position,
                             sizeof(printf_buffer) - printf_position,
                             format,
                             args);
    va_end(args);

    printf_position += return_value;
    assert_true((size_t)printf_position < sizeof(printf_buffer));
    return return_value;
}

/* Mocked fprintf function writing to fprintf_buffer */
int mock_fprintf(FILE* const file, const char *format, ...) {
    int return_value;
    va_list args;

    assert_true(file == stderr);
    /* Checks if buffer iterator is valid */
    assert_true((size_t)fprintf_position < sizeof(fprintf_buffer));

    va_start(args, format);
    return_value = vsnprintf(fprintf_buffer + fprintf_position,
                             sizeof(fprintf_buffer) - fprintf_position,
                             format,
                             args);
    va_end(args);

    fprintf_position += return_value;
    assert_true((size_t)fprintf_position < sizeof(fprintf_buffer));
    return return_value;
}

/*
* Clears fprintf buffer
*/
void mock_clear_fprintf_buffer() {
  memset(fprintf_buffer, 0, sizeof(fprintf_buffer));
  fprintf_position = 0;
}

/*
* Clears printf buffer
*/
void mock_clear_printf_buffer() {
  memset(printf_buffer, 0, sizeof(printf_buffer));
  printf_position = 0;
}

/*
* Clears scanf buffer
*/
void mock_clear_scanf_buffer() {
  memset(scanf_buffer, 0, sizeof(scanf_buffer));
  scanf_position = 0;
}

/*
* Clears all i/o buffers
*/
void mock_clear_all_buffers() {
  mock_clear_scanf_buffer();
  mock_clear_printf_buffer();
  mock_clear_fprintf_buffer();
}

/*
* Gets content of fprintf buffer
*/
char* mock_get_fprintf_buffer() {
  return fprintf_buffer;
}

/*
* Gets content of printf buffer
*/
char* mock_get_printf_buffer() {
  return printf_buffer;
}

/*
* Gets content of scanf buffer
*/
void mock_set_scanf_buffer(const char* str) {
  strcpy( scanf_buffer, str );
  scanf_buffer[strlen(str)] = EOF;
}

#endif
