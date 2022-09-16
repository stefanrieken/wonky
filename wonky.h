#ifndef WONKY_H
#define WONKY_H

#include <stdbool.h>
#include "stack.h"

typedef struct State {
  int at;
  int code_size;
  void ** code; // pre-compiled to postfixed
  void * env;
  Stack * stack;
} State;

typedef bool (* PrimitiveCallback) (State * state);

typedef enum BasicType {
  LABEL,
  INTEGER,
  NATIVE,
  PRIMITIVE
} BasicType;

#define APPLY 0

State * new_state();
State * eval(State * state);

#endif /*WONKY_H*/
