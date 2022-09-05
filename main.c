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

bool is_primitive(void * value) {
  // ATM this test only uses a primitive and no regular code.
  // In general, for reference values (which both of these are)
  // we expect the type to be derivable via the reference.
  return true;
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

