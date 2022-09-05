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

typedef bool (* PrimitiveCallback) (State * state);

State * new_state();
State * eval(State * state);

#endif /*WONKY_H*/
