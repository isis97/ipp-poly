#include "utils.h"
#include "calc_interpreter.h"


/*
 Try to parse one line of input
*/
void ReadInputLine(InterpreterState* state) {
  InterpreterNextChar(state);
  while(state->char_buffer != '\n' && state->char_buffer != EOF) {
    if((state->char_buffer>='a' && state->char_buffer<='z')
      || (state->char_buffer>='A' && state->char_buffer<='Z')) {

      // Try to parse command
      InterpreterParseCommand(state);
      if(InterpreterWasError(state)) {
        InterpreterSeekLineEnd(state);
        return;
      }

      // Interpreter must've parsed all of the line or
      // something is wrong
      if(state->char_buffer != '\n' && state->char_buffer != EOF) {
        InterpreterReportError(state, WRONG_COMMAND);
        InterpreterSeekLineEnd(state);
        return;
      }
    } else {
      // Try to parse polynomial
      Poly* p = MALLOCATE(Poly);
      *p = InterpreterParsePoly(state);
      if(InterpreterWasError(state)) {
        InterpreterSeekLineEnd(state);
        PolyDestroy(p);
        free(p);
        return;
      }

      // Interpreter must've parsed all of the line or
      // something is wrong
      if(state->char_buffer != '\n' && state->char_buffer != EOF) {
        InterpreterReportError(state, INVALID_POLY_INPUT);
        InterpreterSeekLineEnd(state);
        PolyDestroy(p);
        free(p);
        return;
      }

      // Pushed parsed poly to the stack
      StackPush(&(state->poly_stack), p);
    }
  }
}

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  // Create new instance of parser
  InterpreterState instance = InterpreterNew(NULL);
  InterpreterState* state = &instance;

  //
  // Parse input from stdin continously
  //
  while(state->char_buffer != EOF) {
    // Read next line
    ReadInputLine(state);

    // Check if should terminate
    if(InterpreterWasCriticalError(state)) {
      InterpreterPrintError(state);
      // Cleanup then exit
      InterpreterCleanup(state);
      return 1;
    }

    // Read to end of line and clear error
    if(InterpreterWasError(state)) {
      InterpreterPrintError(state);
      InterpreterSeekLineEnd(state);
    }
    InterpreterClearError(state);
  }

  // Cleanup then exit
  InterpreterCleanup(state);
  return 0;
}
