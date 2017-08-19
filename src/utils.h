/** @file
*  Various utilities. (C99 standard)
*
* THIS FILE SHOULD BE ALWAYS INCLUDED AS FIRST ONE!
* This is only in case of compilation with UNIT_TESTING
* Because then all important definitions are replaced with a traps
* used for testing.
*
* The unit tests for the library take advantage of UNIT_TESTING macro.
*
* If you DO NOT WANT the traps to be emmited then
* define DISABLE_TRAPS macro before including this file :)
*
* Unit tests should also define UNIT_TEST it marks the file
* no as part of libary to be tested but rater an external code.
* It works just as DISABLE_TRAPS macro. :)
*
* @author Piotr Styczyński <piotrsty1@gmail.com>
* @copyright MIT
* @date 2017-05-13
*/
#include <time.h>
#include <stdint.h>

#ifndef DEBUG
/**
* @def DEBUG
* Debug symbol definitions
*
* Use DEBUG 0 to disable debug macro DBG
* Use DEBUG 1 to enable debug macro DBG
*/
#define DEBUG 0
#endif /* DEBUG */

#ifndef DBG
/**
* @def DBG
* Macro used for debug purposes
* The code block after it will be executed
* only if DEBUG macro was set to 1
*
* Works as if clause e.g.
* @code
* DBG {
*   printf("Hello");
* }
* @endcode
*/
#define DBG if(DEBUG)
#endif /* DBG */

/*
* This part of code emit traps to replace ordinary functions with
* some other code (diagnostic).
*/


/* Disable all traps */

#if defined(DISABLE_TRAPS) || defined(UNIT_TESTING)

#ifdef printf
#undef printf
#endif /* printf */

#ifdef getchar
#undef getchar
#endif /* getchar */

#ifdef scanf
#undef scanf
#endif /* scanf */

#ifdef fprintf
#undef fprintf
#endif /* fprintf */

#ifdef assert
#undef assert
#endif /* assert */

#ifdef calloc
#undef calloc
#endif /* calloc */

#ifdef malloc
#undef malloc
#endif /* malloc */

#ifdef realloc
#undef realloc
#endif /* realloc */

#ifdef free
#undef free
#endif /* free */

#ifdef main
#undef main
#endif /* main */

#endif


/* Emit traps only if they are not disabled */
#ifndef DISABLE_TRAPS
/* If this is being built in testing mode */
#ifdef UNIT_TESTING


#include <stdio.h>
#include <stdlib.h>

/*
* Redirect printf to a function in
* the test application so it's possible to
* test the standard output.
*/
#define printf(...) mock_printf(__VA_ARGS__)
extern int mock_printf(const char *format, ...);

#define getchar() mock_getchar()
extern int mock_getchar();

#define scanf(...) mock_scanf(__VA_ARGS__)
extern int mock_scanf(const char *format, ...);

/*
* Redirect fprintf to
* a function in the test application so it's possible to
* test error messages.
*/
#define fprintf(...) mock_fprintf(__VA_ARGS__)
extern int mock_fprintf(FILE * const file, const char *format, ...);

/*
* Redirect assert to mock_assert() so
* assertions can be caught by cmocka.
*/
#define assert(expression) \
    mock_assert((int)(expression), #expression, __FILE__, __LINE__)
void mock_assert(const int result, const char* expression, const char *file,
                 const int line);

 /*
 * Redirect calloc, malloc, realloc and free to _test_malloc, _test_calloc,
 * _test_realloc and _test_free, respectively, so cmocka can check for memory
 * leaks. 
 */
 #define calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)

 #define malloc(size) _test_malloc(size, __FILE__, __LINE__)

 #define realloc(ptr, size) _test_realloc(ptr, size, __FILE__, __LINE__)

 #define free(ptr) _test_free(ptr, __FILE__, __LINE__)

 void* _test_calloc(size_t number_of_elements, size_t size, const char* file, const int line);
 void* _test_malloc(size_t size, const char* file, const int line);
 void* _test_realloc(void* ptr, const size_t size, const char* file, const int line);
 void _test_free(void* ptr, const char* file, const int line);


/*
* All functions in this object need to be exposed to the test application,
* so redefine static to nothing. Do not do it - it dangerous!
*/
//#define static


#ifndef UNIT_TEST
/*
* Function main is defined in the unit test so redefine name of the main
* function here.
*/
#define main(...) calculator_main(__VA_ARGS__)
int calculator_main(int argc, char *argv[]);
#endif /* UNIT_TEST */


#endif /* UNIT_TESTING */
#endif /* DISABLE_TRAPS */
