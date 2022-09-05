#ifndef WONKY_H
#define WONKY_H

#include "stack.h"

typedef struct State {
  int at;
  int code_size;
  void ** code; // pre-compiled to postfixed
  void * env;
  Stack * stack;
} State;

// There may be more types in the external system,
// but this is all we need to know.
typedef enum ExecutionType {
  LITERAL,
  C_CALLBACK,
  CODE
} ExecutionType;


typedef bool (* PrimitiveCallback) (State * state);

State * new_state();
void eval(State * state);

#endif /*WONKY_H*/
