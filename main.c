#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>

#include "wonky.h"

/**
 * This is intended a demo for WonkyType.
 */
 

bool add(State * state) {
  printf("in add! Stack size: %d\n", state->stack->size);
  
  intptr_t a = (intptr_t) pop(state->stack);
  intptr_t b = (intptr_t) pop(state->stack);
  push(&(state->stack), (void*) (a+b));
  return false;
}

// This neatly begs the question how we expect the user to produce the execution type from value.
// It works with TMMH of course, but is that how we want it here?
ExecutionType get_execution_type(void * value) {
  printf("exec type for %d asked\n", (intptr_t) value);
  if (((intptr_t) value) == 1) return LITERAL;
  else return C_CALLBACK;
}

State * make_new_state_for(void * function, State * caller_state) {
  printf("\"make new state for\"; simply returning callerstate\n");
  return caller_state;
}

int main (int argc, void ** argv) {
  State * state = new_state();

  state->code = (void *[]) {(void *) 1, (void *) 1, (void *) add, (void *) 0 };
  state->code_size = 4;
  eval(state);
  
  printf("Result of 1+1: %d\n", (intptr_t) pop(state->stack));
  return 0;
}

