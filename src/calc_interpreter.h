/** @file
*  Calculator code interpreter.
*
*  Usage:
*  @code
*     #include <calc_interpreter.h>
*      ...
*     // Create new interpreter instance
*     InterpreterState calc = InterpreterNew(NULL); // Report no errors
*
*     //
*     // Parse continously code from stdin
*     // This is a blocking function
*     //
*     int exit_code = InterpreterReadInput(calc);
*
*  @endcode
*
*
*  @author Piotr Styczyński <piotrsty1@gmail.com>
*  @copyright MIT
*  @date 2017-05-13
*/
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "stack.h"
#include "memalloc.h"
#include "poly.h"

#ifndef __STY_COMMON_INTERPRETER_H__
#define __STY_COMMON_INTERPRETER_H__

/**
 * @def INTERPRETER_MAX_COMMAND_BUFFER_SIZE
 *
 * Maximum length of a valid command
*/
#define INTERPRETER_MAX_COMMAND_BUFFER_SIZE 25

/**
 * Runtime properties of interpreter.
*/
typedef struct InterpreterState InterpreterState;

/**
 * Binding of command.
**/
typedef struct InterpreterCommandBinding InterpreterCommandBinding;

/**
 * Defines relation between command name and action function
**/
struct InterpreterCommandBinding {
  char* command; ///< Name of the command
  void (*action)(InterpreterState*); ///< Function invoked when the command is executed
  int required_params; ///< Required numbers of elements on the stack
};

/**
 * Types of errors
**/
typedef enum {
  STACK_UNDEFLOW, ///< Theres not enough polynomails on stack
  WRONG_COMMAND, ///< Invalid command was used
  WRONG_VARIABLE, ///< Invalid variable id was used
  WRONG_VALUE, ///< Invalid numerical value was used
  WRONG_COUNT, ///< Wrong count given to command
  NO_ERROR, ///< No error was reported
  INVALID_POLY_INPUT, ///< Invalid poly specification
  PROCESS_FORCE_RETURN ///< Process was forcefully closed
} InterpreterErrorType;

/**
 * Runtime properties of interpreter
*/
struct InterpreterState {
  FILE* err_out; ///< Stream to write errors to
  char char_buffer; ///< Input buffer
  InterpreterErrorType error_type; ///< Error flag
  int input_col; ///< Number of currently parsed column
  int input_row; ///< Number of currently parsed row
  int prev_input_col; ///< Number of previously parsed column
  int prev_input_row; ///< Number of previously parsed row
  int error_col; ///< Column where flagged error happened
  int error_row; ///< Row where flagged error happened
  bool critical_error_flag; ///< Is this error crititcal
  Stack poly_stack; ///< Stack of polynomials
};


/**
 * Create new empty instance of interpreter runtime state.
 * Interpreter instance is created for given FILE* error output.
 *
 * @param[in] err_out : Error output stream
 * @return         New interpreter instance
 */
InterpreterState InterpreterNew(FILE* err_out);

/**
 * Reports an error to the interpreter.
 *
 * @param[in] state : interpreter instance
 * @param[in] error_type type of error to be reported
 */
void InterpreterReportError(InterpreterState* state, InterpreterErrorType error_type);

/**
 * Report a critical error to the interpreter
 * When a critical error is reaised the interpreter will stop parsing and quit.
 *
 * @param[in] state      : Interpreter instance
 * @param[in] error_type : Type of error to be reported
 */
void InterpreterReportCriticalError(InterpreterState* state, InterpreterErrorType error_type);


/**
 * Report force return request to the parser.
 * The parser will report PROCESS_FORCE_RETURN error.
 * And then terminate forcefully.
 *
 * @param[in] state : Interpreter instance
 */
void InterpreterReportForceReturn(InterpreterState* state);

/**
 * Clear error flag.
 *
 * @param[in] state : Interpreter instance
 */
void InterpreterClearError(InterpreterState* state);

/**
 * Check if interpreter reported an error.
 * The errors are held as a single flag.
 *
 * @param  state : Interpreter instance
 * @return If there was any error reported?
 */
bool InterpreterWasError(InterpreterState* state);

/**
 * Check if interpreter reported a critical error.
 * The errors are held as a single flag.
 *
 * @param  state : Interpreter instance
 * @return If there was any critical error reported?
 */
bool InterpreterWasCriticalError(InterpreterState* state);

/**
 * Check if there are at least @p number_of_params objects on internal stack.
 *
 * @param state            : Interpreter instance
 * @param number_of_params : Number of object that must reside on stack
 */
void InterpreterRequestStackParams(InterpreterState* state, const int number_of_params);

/**
 * Read data until new line is reached.
 *
 * @param state : Interpreter instance
 */
void InterpreterReadUntilNewLine(InterpreterState* state);

/**
 * Read next character from input.
 *
 * @param state : Interpreter instance
 * @return next character from input
 */
char InterpreterNextChar(InterpreterState* state);


/**
 * Try to parse a number at current location with set numerical limits.
 * An error of specified type is raised:
 *   - number is parsed until any non-digit character is met
 *   - number starting from non-digit will trigger an error
 *   - maximum/minimum values overflow will trigger an error
 *
 * @param  state             : Interpreter instance
 * @param  error_when_failed : Type of error raised when the number cannot be parsed
 * @param  maximum           : Maximum value
 * @param  minimum           : Minimum value
 * @return parsed number value
 */
long long InterpreterParseNumber(InterpreterState* state, InterpreterErrorType error_when_failed, long long maximum, long long minimum);

/**
 * Print poly in interpreter manner.
 * The format is as follows:
 * - if polyonomial is constant then print just its value
 * - if not then print ((`POLY0`,`EXP0`),(`POLY1`,`EXP1`)...(`POLYN`,`EXPN`))
 *   where `POLYN` is n-th monomial coefficent
 *   and `EXPN` is its variable exponent.
 *
 * Examples:
 * @code
 *   // Constant polynomials
 *   Poly0;                                //  0
 *   PolyC( 12 );                          //  12
 *
 *   // More complex polynomials
 *   PolyP( PolyC(12), 2 );                // (12,2)
 *   PolyP( PolyC(3), 4 );                 // (3,4)
 *   PolyP( PolyC(2), 5, PolyC(1), 7 );    // ((2,5),(1,7))
 *   PolyP( PolyP( PolyC( 1, 3 ), 4 ), 5 ) // (((1,3),4),5)
 *
 *   // Polynomials with free term
 *   PolyP( PolyC(12), 0 );                // 12
 *   PolyP( PolyC(12), 0, PolyC(13), 1 )   // ((12,0),(13,1))
 * @endcode
 *
 * @param p : Polynomial to be printed
 */
void InterpreterPrintPoly(Poly* p);

/**
 * Try to parse poly at current character.
 * The format of polynomial  is as follows:
 * - if polyonomial is constant then print just its value
 * - if not then print ((`POLY0`,`EXP0`)+(`POLY1`,`EXP1`)+...+(`POLYN`,`EXPN`))
 *   where `POLYN` is n-th monomial coefficent
 *   and `EXPN` is its variable exponent.
 *
 * This function is used together with InterpreterParseMono to
 * parse polynomials recursively.
 *
 * Examples:
 * @code
 *   // Constant polynomials
 *   Poly0;                                //  0
 *   PolyC( 12 );                          //  12
 *
 *   // More complex polynomials
 *   PolyP( PolyC(12), 2 );                // (12,2)
 *   PolyP( PolyC(3), 4 );                 // (3,4)
 *   PolyP( PolyC(2), 5, PolyC(1), 7 );    // ((2,5)+(1,7))
 *   PolyP( PolyP( PolyC( 1, 3 ), 4 ), 5 ) // (((1,3),4),5)
 *
 *   // Polynomials with free term
 *   PolyP( PolyC(12), 0 );                // 12
 *   PolyP( PolyC(12), 0, PolyC(13), 1 )   // ((12,0)+(13,1))
 * @endcode
 *
 * If the input is not valid then INVALID_POLY_INPUT error is raised.
 *
 * @param  state : Interpreter instance
 * @return Parsed polynomial
 */
Poly InterpreterParsePoly(InterpreterState* state);


/**
 * Try to parse monomial at current character.
 * The format of monomial is as follows:
 * `(COEFF,EXP)`
 * Where `COEFF` is polynomial coefficent in interpreter format.
 * (for more see InterpreterParsePoly)
 * And exp is number - variable exponent.
 *
 * This function is used together with InterpreterParsePoly to
 * parse polynomials recursively.
 *
 * Examples:
 * @code
 *   // Constant polynomials
 *   Poly0;                                //  0
 *   PolyC( 12 );                          //  12
 *
 *   // More complex polynomials
 *   PolyP( PolyC(12), 2 );                // (12,2)
 *   PolyP( PolyC(3), 4 );                 // (3,4)
 *   PolyP( PolyC(2), 5, PolyC(1), 7 );    // ((2,5),(1,7))
 *   PolyP( PolyP( PolyC( 1, 3 ), 4 ), 5 ) // (((1,3),4),5)
 *
 *   // Polynomials with free term
 *   PolyP( PolyC(12), 0 );                // 12
 *   PolyP( PolyC(12), 0, PolyC(13), 1 )   // ((12,0),(13,1))
 * @endcode
 *
 * If the input is not valid then INVALID_POLY_INPUT error is raised.
 *
 * @param  state : Interpreter instance
 * @return Parsed monomial
 */
Mono InterpreterParseMono(InterpreterState* state);

/**
 * Try to parse a command at current character
 *
 * @param state : Interpreter instance
 */
void InterpreterParseCommand(InterpreterState* state);

/**
 * Read characters until EOF or new line is reached.
 *
 * @param state : Interpreter instance
 */
void InterpreterSeekLineEnd(InterpreterState* state);

/**
 * Print human readable representation of an interpreter error.
 *
 * @param state : Interpreter instance
 */
void InterpreterPrintError(InterpreterState* state);

/**
 * Cleanup interpreter before terminating.
 *
 * @param state : Interpreter instance
 */
void InterpreterCleanup(InterpreterState* state);

#endif /* __STY_COMMON_INTERPRETER_H__ */
