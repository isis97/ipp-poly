#include "test_utils.h"
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef UNIT_TESTING

/* Pomocnicze bufory, do których piszą atrapy funkcji printf i fprintf oraz
pozycje zapisu w tych buforach. Pozycja zapisu wskazuje bajt o wartości 0. */
char fprintf_buffer[256];
char printf_buffer[256];
char scanf_buffer[256];

int fprintf_position = 0;
int printf_position = 0;
int scanf_position = 0;


/* Atrapa funkcji scanf */
int mock_scanf(const char *format, ...) {
    int return_value;
    va_list args;

    /* Poniższa asercja sprawdza też, czy printf_position jest nieujemne.
    W buforze musi zmieścić się kończący bajt o wartości 0. */
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

/* Atrapa funkcji getchar */
int mock_getchar() {
  if(scanf_position >= (int)sizeof(scanf_buffer)) return EOF;
  if(scanf_buffer[scanf_position] == EOF) return EOF;
  char c;
  c = *((char*)(scanf_buffer + scanf_position));
  ++scanf_position;
  return c;
}

/* Atrapa funkcji printf */
int mock_printf(const char *format, ...) {
    int return_value;
    va_list args;

    /* Poniższa asercja sprawdza też, czy printf_position jest nieujemne.
    W buforze musi zmieścić się kończący bajt o wartości 0. */
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

/* Atrapa funkcji fprintf */
int mock_fprintf(FILE* const file, const char *format, ...) {
    int return_value;
    va_list args;

    assert_true(file == stderr);
    /* Poniższa asercja sprawdza też, czy fprintf_position jest nieujemne.
    W buforze musi zmieścić się kończący bajt o wartości 0. */
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

void mock_clear_fprintf_buffer() {
  memset(fprintf_buffer, 0, sizeof(fprintf_buffer));
  fprintf_position = 0;
}

void mock_clear_printf_buffer() {
  memset(printf_buffer, 0, sizeof(printf_buffer));
  printf_position = 0;
}

void mock_clear_scanf_buffer() {
  memset(scanf_buffer, 0, sizeof(scanf_buffer));
  scanf_position = 0;
}

void mock_clear_all_buffers() {
  mock_clear_scanf_buffer();
  mock_clear_printf_buffer();
  mock_clear_fprintf_buffer();
}

char* mock_get_fprintf_buffer() {
  return fprintf_buffer;
}

char* mock_get_printf_buffer() {
  return printf_buffer;
}

void mock_set_scanf_buffer(const char* str) {
  strcpy( scanf_buffer, str );
  scanf_buffer[strlen(str)] = EOF;
}

#endif
