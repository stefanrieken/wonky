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
  // In general, for reference values (which both of these are)
  // we expect the type to be derivable via the reference.
  if (value == add) return true;
  else return false;
}

State * make_new_state_for(void * function, State * caller_state) {

  if (function == add) {
    printf("\"make new state for\" add; simply returning caller state\n");
    return caller_state;
  } else {
    printf("\"make new state for\" myfunc\n");
    State * state = new_state();
    state->code = function;
    state->code_size = 4; // TODO how do I know this? (Answer: size should be derivable from code block contents.)

    return state;
  }
}


int main (int argc, void ** argv) {
  // In this test we use a rather optimistic mix of direct integers and full pointer values.
  // In actual reality we might be able to combine direct (integer) values with memory indices (relative pointers).
  // The idea for now is that we retain some flexibility (and bragging rights) if we
  // at least try to keep any required data type encoding transparent to the eval loop.
  // Suggestion though:
  // 00xxxxxx... = positive int
  // 01xxxxxx... = primitive pointer
  // 10xxxxxx... = code or other data pointer (more info at pointer)
  // 11xxxxxx... = negative int (two's complement)

  void * myfunc = (void *[]) {(void *) 1, (void *) 1, add, (void *) 0 };
  
  State * state = new_state();
  state->code = (void *[]) {myfunc, 0 };
  state->code_size = 2;
  state = eval(state);
  
  printf("Result of 1+1: %d\n", (intptr_t) pop(state->stack));
  return 0;
}

