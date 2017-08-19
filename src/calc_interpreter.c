/*
*  Calculator code interpreter.
*  Operations specification is available in README file.
*
*  Usage:
*  @code 
*     #include <calc_interpreter.h>
*      ...
*     // Create new interpreter instance
*     InterpreterState calc = InterpreterNew(NULL); // Report no errors
*
*     // Parse polynomial from input
*     Poly* p = MALLOCATE(Poly);
*     p = InterpreterParsePoly(state);
*
*     // Print loaded polynomial
*     PolyPrint(&p);
*
*     // Cleanup
*     PolyDestroy(&p);
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
#include "calc_interpreter.h"

#define NUMBER_MIN LONG_MIN
#define NUMBER_MAX LONG_MAX

/*
 Create new empty instance of interpreter runtime state.
*/
InterpreterState InterpreterNew(FILE* err_out) {
  if(err_out == NULL) err_out = stderr;
  return (InterpreterState) {
    .err_out = err_out,
    .char_buffer = '0',
    .error_type = NO_ERROR,
    .poly_stack = StackNew(),
    .input_col = 1,
    .input_row = 1,
    .prev_input_col = 1,
    .prev_input_row = 1,
    .error_col = 0,
    .error_row = 0,
    .critical_error_flag = false
  };
}

/*
* Report an error to the interpreter
*/
void InterpreterReportError(InterpreterState* state, InterpreterErrorType error_type) {
  if(state->error_type != NO_ERROR) return;
  state->error_type = error_type;
  if(1) {
    state->error_row = state->prev_input_row;
    state->error_col = state->prev_input_col;
  } else {
    state->error_row = state->input_row;
    state->error_col = state->input_col;
  }
}

/*
* Report a critical error to the interpreter
*/
void InterpreterReportCriticalError(InterpreterState* state, InterpreterErrorType error_type) {
  InterpreterReportError(state, error_type);
  state->critical_error_flag = true;
}

/*
* Report force return event
*/
void InterpreterReportForceReturn(InterpreterState* state) {
  InterpreterReportCriticalError(state, PROCESS_FORCE_RETURN);
}

/*
* Remove all errors flags
*/
void InterpreterClearError(InterpreterState* state) {
  state->error_type = NO_ERROR;
  state->error_row = 0;
  state->error_col = 0;
  state->critical_error_flag = false;
}

/*
* Check for interpreter errors
*/
bool InterpreterWasError(InterpreterState* state) {
  return (state->error_type != NO_ERROR);
}

/*
* Check for interpreter critical errors
*/
bool InterpreterWasCriticalError(InterpreterState* state) {
  return (state->critical_error_flag && state->error_type != NO_ERROR);
}

/*
* Request some parameters to be on stack
*/
void InterpreterRequestStackParams(InterpreterState* state, const int number_of_params) {
  if(StackSize(&(state->poly_stack)) < number_of_params) {
    InterpreterReportError(state, STACK_UNDEFLOW);
    return;
  }
}

/*
* Check if a function is command begin
*/
bool InterpreterIsCommandBegin(const char c) {
  return (c>='a' && c<='z') || (c>='A' && c<='Z');
}

/*
* Check if currently parsed character is a digit
*/
bool InterpreterCurrentIsDigit(InterpreterState* state) {
    return (state->char_buffer >= '0') && (state->char_buffer <= '9');
}

/*
* Read input until new line is reached
*/
void InterpreterReadUntilNewLine(InterpreterState* state) {
  do {
    state->char_buffer = getchar();
  } while(state->char_buffer != EOF && state->char_buffer != '\n');
}

/*
* Read next character from input
*/
char InterpreterNextChar(InterpreterState* state) {
  state->char_buffer = getchar();
  state->prev_input_row = state->input_row;
  state->prev_input_col = state->input_col;
  if(state->char_buffer == '\n') {
    state->input_col = 1;
    ++state->input_row;
  } else {
    ++state->input_col;
  }
  return state->char_buffer;
}

/*
* Try to parse a number at current location with set numerical limits
*/
long long InterpreterParseNumber(InterpreterState* state, InterpreterErrorType error_when_failed, long long maximum, long long minimum) {
  bool neg_flag = false;

  if(state->char_buffer == '-') {
    InterpreterNextChar(state);
    neg_flag = true;
  }
  if(!InterpreterCurrentIsDigit(state)) {
    InterpreterReportError(state, error_when_failed);
    return 0;
  } else {
    long long accumulator = 0;
    do {
      const long long delta = (state->char_buffer-'0');

      if(neg_flag) {
        if(accumulator < minimum / 10) {
          InterpreterReportError(state, error_when_failed);
          return 0;
        }
        accumulator *= 10;
        if(accumulator < minimum + delta) {
          InterpreterReportError(state, error_when_failed);
          return 0;
        }
        accumulator -= delta;
      } else {
        if(accumulator > maximum / 10) {
          InterpreterReportError(state, error_when_failed);
          return 0;
        }
        accumulator *= 10;
        if(accumulator > maximum - delta) {
          InterpreterReportError(state, error_when_failed);
          return 0;
        }
        accumulator += delta;
      }
      InterpreterNextChar(state);

      if((neg_flag && accumulator>0) || (!neg_flag && accumulator<0)) {
        InterpreterReportError(state, error_when_failed);
        return 0;
      }

    } while(InterpreterCurrentIsDigit(state));
    return accumulator;
  }
}


/*
* ZERO stack operation impl
*/
void InterpreterOpZero(InterpreterState* state) {
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_COMMAND);
    return;
  }
  Poly* p = MALLOCATE(Poly);
  *p = PolyZero();
  StackPush(&(state->poly_stack), p);
}

/*
* IS_COEFF stack operation impl
*/
void InterpreterOpIsCoeff(InterpreterState* state) {
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_COMMAND);
    return;
  }
  if(PolyIsCoeff((Poly*) StackFirst(&(state->poly_stack)))) {
    printf("1\n");
  } else {
    printf("0\n");
  }
}

/*
* IS_ZERO stack operation impl
*/
void InterpreterOpIsZero(InterpreterState* state) {
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_COMMAND);
    return;
  }
  if(PolyIsZero((Poly*) StackFirst(&(state->poly_stack)))) {
    printf("1\n");
  } else {
    printf("0\n");
  }
}

/*
* CLONE stack operation impl
*/
void InterpreterOpClone(InterpreterState* state) {
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_COMMAND);
    return;
  }
  Poly* cln = MALLOCATE(Poly);
  *cln = PolyClone((Poly*) StackFirst(&(state->poly_stack)));
  StackPush(&(state->poly_stack), cln);
}

/*
* ADD stack operation impl
*/
void InterpreterOpAdd(InterpreterState* state) {
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_COMMAND);
    return;
  }
  Poly* ret = MALLOCATE(Poly);
  Poly* a = (Poly*) StackPop(&(state->poly_stack));
  Poly* b = (Poly*) StackPop(&(state->poly_stack));

  *ret = PolyAdd(a, b);

  PolyDestroy(a);
  PolyDestroy(b);
  free(a);
  free(b);

  StackPush(&(state->poly_stack), ret);
}

/*
* MUL stack operation impl
*/
void InterpreterOpMul(InterpreterState* state) {
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_COMMAND);
    return;
  }
  Poly* ret = MALLOCATE(Poly);
  Poly* a = (Poly*) StackPop(&(state->poly_stack));
  Poly* b = (Poly*) StackPop(&(state->poly_stack));

  *ret = PolyMul(a, b);

  PolyDestroy(a);
  PolyDestroy(b);
  free(a);
  free(b);

  StackPush(&(state->poly_stack), ret);
}

/*
* POW stack operation impl
*/
void InterpreterOpPow(InterpreterState* state) {
  long long x = InterpreterParseNumber(state, WRONG_VALUE, NUMBER_MAX, NUMBER_MIN);
  if(InterpreterWasError(state)) return;
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_VALUE);
    return;
  }

  Poly* ret = MALLOCATE(Poly);
  Poly* a = (Poly*) StackPop(&(state->poly_stack));

  *ret = PolyPow(a, x);

  PolyDestroy(a);
  free(a);

  StackPush(&(state->poly_stack), ret);
}


/*
* COMPOSE stack operation impl
*/
void InterpreterOpCompose(InterpreterState* state) {
  long long count = InterpreterParseNumber(state, WRONG_COUNT, (long long)UINT_MAX, 0);
  if(InterpreterWasError(state)) return;


  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_COUNT);
    return;
  }

  if((count > (long long)UINT_MAX) || (count < 0)) {
    InterpreterReportError(state, WRONG_COUNT);
    return;
  }

  if(StackSize(&(state->poly_stack)) < (count+1)) {
    InterpreterReportError(state, STACK_UNDEFLOW);
    return;
  }

  Poly* p = StackPop(&(state->poly_stack));
  Poly composition_table[count];
  for(int i=0;i<count;++i) {
    Poly* x = StackPop(&(state->poly_stack));
    composition_table[i] = *x;
    free(x);
  }

  Poly* ret = MALLOCATE(Poly);
  *ret = PolyCompose(p, count, composition_table);
  StackPush(&(state->poly_stack), ret);

  PolyDestroy(p);
  free(p);

  for(int i=0;i<count;++i) {
    PolyDestroy(&composition_table[i]);
  }

}

/*
* NEG stack operation impl
*/
void InterpreterOpNeg(InterpreterState* state) {
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_COMMAND);
    return;
  }
  Poly* ret = MALLOCATE(Poly);
  Poly* a = (Poly*) StackPop(&(state->poly_stack));

  *ret = PolyNeg(a);

  PolyDestroy(a);
  free(a);

  StackPush(&(state->poly_stack), ret);
}

/*
* SUB stack operation impl
*/
void InterpreterOpSub(InterpreterState* state) {
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_COMMAND);
    return;
  }
  Poly* ret = MALLOCATE(Poly);
  Poly* a = (Poly*) StackPop(&(state->poly_stack));
  Poly* b = (Poly*) StackPop(&(state->poly_stack));

  *ret = PolySub(a, b);

  PolyDestroy(a);
  PolyDestroy(b);
  free(a);
  free(b);

  StackPush(&(state->poly_stack), ret);
}

/*
* IS_EQ stack operation impl
*/
void InterpreterOpIsEq(InterpreterState* state) {
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_COMMAND);
    return;
  }
  Poly* a = (Poly*) StackPop(&(state->poly_stack));
  Poly* b = (Poly*) StackPop(&(state->poly_stack));
  StackPush(&(state->poly_stack), b);
  StackPush(&(state->poly_stack), a);

  if(PolyIsEq(a, b)) {
    printf("1\n");
  } else {
    printf("0\n");
  }
}

/*
* DEG stack operation impl
*/
void InterpreterOpDeg(InterpreterState* state) {
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_COMMAND);
    return;
  }
  printf("%d\n", PolyDeg((Poly*) StackFirst(&(state->poly_stack))));
}

/*
* DEG_BY stack operation impl
*/
void InterpreterOpDegBy(InterpreterState* state) {
  long long x = InterpreterParseNumber(state, WRONG_VARIABLE, NUMBER_MAX, NUMBER_MIN);
  if(InterpreterWasError(state)) return;
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_VARIABLE);
    return;
  }
  if(x < 0 || x > (long long)UINT_MAX) {
    InterpreterReportError(state, WRONG_VARIABLE);
    return;
  }

  printf("%d\n", PolyDegBy((Poly*) StackFirst(&(state->poly_stack)), x));
}

/*
* AT stack operation impl
*/
void InterpreterOpAt(InterpreterState* state) {
  long long x = InterpreterParseNumber(state, WRONG_VALUE, NUMBER_MAX, NUMBER_MIN);
  if(InterpreterWasError(state)) return;
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_VALUE);
    return;
  }

  Poly* a = (Poly*) StackPop(&(state->poly_stack));
  Poly* ret = MALLOCATE(Poly);
  *ret = PolyAt(a, x);
  //PolyPrint(ret);
  StackPush(&(state->poly_stack), ret);

  PolyDestroy(a);
  free(a);
}


/*
* Print poly in interpreter manner
*/
void InterpreterPrintPolyRec(Poly* p, poly_coeff_t freeTerm) {
  if(PolyIsCoeff(p)) {
    printf("%ld", PolyGetConstTerm(p) + freeTerm);
  } else {
    int index = 0;
    LOOP_LIST(&(p->monos), i) {
      Mono* mono = (Mono*) ListGetValue(i);
      if(index != 0) {
        printf("+");
        printf("(");
        InterpreterPrintPolyRec(&(mono->p), 0);
        printf(",");
        printf("%d)", mono->exp);
        ++index;
      } else if(mono->exp == 0) {
        printf("(");
        InterpreterPrintPolyRec(&(mono->p), p->c + freeTerm);
        printf(",");
        printf("%d)", mono->exp);
        ++index;
      } else if(p->c+freeTerm != 0){
        printf("(%ld,0)", p->c + freeTerm);
        printf("+(");
        InterpreterPrintPolyRec(&(mono->p), 0);
        printf(",");
        printf("%d)", mono->exp);
        ++index;
      } else if(mono->exp != 0) {
        printf("(");
        InterpreterPrintPolyRec(&(mono->p), 0);
        printf(",");
        printf("%d)", mono->exp);
        ++index;
      }
    }
  }
}

/*
* Print poly in interpreter manner
*/
void InterpreterPrintPoly(Poly* p) {
  InterpreterPrintPolyRec(p, 0);
}

/*
* PRINT stack operation impl
*/
void InterpreterOpPrint(InterpreterState* state) {
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_COMMAND);
    return;
  }
  InterpreterPrintPoly((Poly*) StackFirst(&(state->poly_stack)));
  printf("\n");
}

/*
* Printer function used by DUMP command
*/
static inline void* InterpreterPolyStackPrinter(void* data) {
  PolyPrint((Poly*)data);
  return NULL;
}

/*
* DUMP stack operation impl
*/
void InterpreterOpDump(InterpreterState* state) {
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_COMMAND);
    return;
  }
  StackPrint(&(state->poly_stack), InterpreterPolyStackPrinter);
  printf("\n");
}

/*
* CLEAN stack operation impl
*/
void InterpreterOpClean(InterpreterState* state) {
  if(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterReportError(state, WRONG_COMMAND);
    return;
  }
  while(!StackEmpty(&(state->poly_stack))) {
    Poly* a = (Poly*)StackPop(&(state->poly_stack));
    PolyDestroy(a);
    free(a);
  }
}

/*
* POP stack operation impl
*/
void InterpreterOpPop(InterpreterState* state) {
  Poly* a = (Poly*)StackPop(&(state->poly_stack));
  PolyDestroy(a);
  free(a);
}

/*
* EXIT stack operation impl
*/
void InterpreterOpForceReturn(InterpreterState* state) {
  InterpreterReportForceReturn(state);
}

/*
* Number of all valid commands
*/
#define INTERPRETER_COMMANDS_COUNT 19

/*
* All command bindings
*/
static const InterpreterCommandBinding INTERPRETER_COMMANDS[INTERPRETER_COMMANDS_COUNT] = {
  { .required_params = 0, .command = "ZERO",     .action = InterpreterOpZero        },
  { .required_params = 1, .command = "IS_COEFF", .action = InterpreterOpIsCoeff     },
  { .required_params = 1, .command = "IS_ZERO",  .action = InterpreterOpIsZero      },
  { .required_params = 1, .command = "CLONE",    .action = InterpreterOpClone       },
  { .required_params = 2, .command = "ADD",      .action = InterpreterOpAdd         },
  { .required_params = 2, .command = "MUL",      .action = InterpreterOpMul         },
  { .required_params = 1, .command = "NEG",      .action = InterpreterOpNeg         },
  { .required_params = 2, .command = "SUB",      .action = InterpreterOpSub         },
  { .required_params = 2, .command = "IS_EQ",    .action = InterpreterOpIsEq        },
  { .required_params = 1, .command = "DEG",      .action = InterpreterOpDeg         },
  { .required_params = 1, .command = "DEG_BY",   .action = InterpreterOpDegBy       },
  { .required_params = 1, .command = "AT",       .action = InterpreterOpAt          },
  { .required_params = 1, .command = "PRINT",    .action = InterpreterOpPrint       },
  { .required_params = 1, .command = "POP",      .action = InterpreterOpPop         },
  { .required_params = 1, .command = "POW",      .action = InterpreterOpPow         },
  { .required_params = 0, .command = "COMPOSE",  .action = InterpreterOpCompose     },
  { .required_params = 0, .command = "DUMP",     .action = InterpreterOpDump        },
  { .required_params = 0, .command = "CLEAN",    .action = InterpreterOpClean       },
  { .required_params = 0, .command = "EXIT",     .action = InterpreterOpForceReturn }
};

/*
* Try to parse mono at current character
*/
Mono InterpreterParseMono(InterpreterState* state) {
  Poly fact = PolyZero();
  if(InterpreterCurrentIsDigit(state) || state->char_buffer == '-') {
    long long coeff = InterpreterParseNumber(state, INVALID_POLY_INPUT, NUMBER_MAX, NUMBER_MIN);
    if(InterpreterWasError(state)) {
      return MonoZero();
    }
    fact = PolyFromCoeff(coeff);
  } else if(state->char_buffer == '(') {
    fact = InterpreterParsePoly(state);
    if(InterpreterWasError(state)) {
      return MonoZero();
    }
  } else {
    InterpreterReportError(state, INVALID_POLY_INPUT);
    return MonoZero();
  }
  if(state->char_buffer != ',') {
    InterpreterReportError(state, INVALID_POLY_INPUT);
    return MonoZero();
  }
  poly_coeff_t exp = 0;
  InterpreterNextChar(state);
  exp = (poly_coeff_t)InterpreterParseNumber(state, INVALID_POLY_INPUT, UINT_MAX, 0);
  if(InterpreterWasError(state)) {
    return MonoZero();
  }
  Mono ret = MonoFromPoly(&fact, exp);
  return ret;
}


/*
* Try to parse poly at current character
*/
Poly InterpreterParsePoly(InterpreterState* state) {
  Poly p = PolyZero();

  bool mono_first_flag = true;
  bool allow_add=true;
  while(true) {
    if(state->char_buffer == '-' || InterpreterCurrentIsDigit(state)) {
      poly_coeff_t coeff = (poly_coeff_t)InterpreterParseNumber(state, INVALID_POLY_INPUT, NUMBER_MAX, NUMBER_MIN);
      if(InterpreterWasError(state)) {
        return p;
      }
      return PolyFromCoeff(coeff);
    } else if(state->char_buffer == '(') {
      if(!allow_add) {
        InterpreterReportError(state, INVALID_POLY_INPUT);
        return p;
      }
      InterpreterNextChar(state);
      Mono m = InterpreterParseMono(state);
      if(InterpreterWasError(state)) {
        return p;
      }
      if(state->char_buffer != ')') {
        InterpreterReportError(state, INVALID_POLY_INPUT);
        return p;
      }
      InterpreterNextChar(state);
      PolyInsertMono(&p, m);
      mono_first_flag=false;
      allow_add=false;
    } else if(state->char_buffer == '+') {
      allow_add=true;
      if(mono_first_flag) {
        InterpreterReportError(state, INVALID_POLY_INPUT);
        return p;
      }
      InterpreterNextChar(state);
      if(state->char_buffer != '(') {
        InterpreterReportError(state, INVALID_POLY_INPUT);
        return p;
      }
    } else {
      break;
    }
  }
  return p;
}


/*
* Try to parse a command at current character
*/
void InterpreterParseCommand(InterpreterState* state) {

  char buffer [INTERPRETER_MAX_COMMAND_BUFFER_SIZE];
  for(int i=0;i<INTERPRETER_MAX_COMMAND_BUFFER_SIZE;++i) buffer[i] = '\0';

  int buffer_i = 0;

  while(state->char_buffer != '\n' && state->char_buffer != ' ') {
    if(buffer_i >= INTERPRETER_MAX_COMMAND_BUFFER_SIZE) {
      InterpreterReportError(state, WRONG_COMMAND);
      return;
    }
    buffer[buffer_i] = state->char_buffer;
    InterpreterNextChar(state);
    ++buffer_i;
  }
  if(state->char_buffer == ' ') {
    InterpreterNextChar(state);
  }

  for(int i=0;i<INTERPRETER_COMMANDS_COUNT;++i) {
    if(strcmp(buffer, INTERPRETER_COMMANDS[i].command) == 0) {
      InterpreterRequestStackParams( state, INTERPRETER_COMMANDS[i].required_params );
      if(InterpreterWasError(state)) {
        return;
      }
      INTERPRETER_COMMANDS[i].action( state );
      return;
    }
  }

  InterpreterReportError(state, WRONG_COMMAND);
  return;
}


/*
* Read characters until EOF or new line is reached
*/
void InterpreterSeekLineEnd(InterpreterState* state) {
  while(state->char_buffer != '\n' && state->char_buffer != EOF) {
    InterpreterNextChar(state);
  }
}

/*
* Print human readable representation of an interpreter error
*/
void InterpreterPrintError(InterpreterState* state) {
  if(state->error_type == PROCESS_FORCE_RETURN) {
    fprintf(state->err_out, "TERMINATED\n");
    return;
  }
  if(state->critical_error_flag) {
    fprintf(state->err_out, "CRITICAL ");
  }
  switch(state->error_type) {
    case PROCESS_FORCE_RETURN:
    break;
    case STACK_UNDEFLOW:
      fprintf(state->err_out, "ERROR %d STACK UNDERFLOW\n", state->error_row);
    break;
    case WRONG_COUNT:
      fprintf(state->err_out, "ERROR %d WRONG COUNT\n", state->error_row);
    break;
    case WRONG_COMMAND:
      fprintf(state->err_out, "ERROR %d WRONG COMMAND\n", state->error_row);
    break;
    case WRONG_VARIABLE:
      fprintf(state->err_out, "ERROR %d WRONG VARIABLE\n", state->error_row);
    break;
    case WRONG_VALUE:
      fprintf(state->err_out, "ERROR %d WRONG VALUE\n", state->error_row);
    break;
    case NO_ERROR:
    break;
    case INVALID_POLY_INPUT:
      fprintf(state->err_out, "ERROR %d %d\n", state->error_row, state->error_col);
    break;
    default:
      fprintf(state->err_out, "UNKNOWN ERROR %d %d\n", state->error_row, state->error_col);
  }
}

/*
* Deallocator for polynomials in the stack
*/
void* InterpreterStackDeallocator(void* element) {
  Poly* p = (Poly*) element;
  PolyDestroy(p);
  free(p);
  return NULL;
}

/*
* Cleanup interpreter before terminating.
*/
void InterpreterCleanup(InterpreterState* state) {
  StackDestroyDeep(&(state->poly_stack), InterpreterStackDeallocator);
}
