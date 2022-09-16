#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "wonky.h"

/**
 * This is intended a demo for WonkyType.
 */


bool add(State * state) {
  printf("in add! Stack size: %d\n", state->stack->size);

  intptr_t a = (intptr_t) pop(state->stack) >> 2;
  intptr_t b = (intptr_t) pop(state->stack) >> 2;
  printf("a: %ld b: %ld\n", a, b);
  push(&(state->stack), (void*) (((a+b) << 2)|INTEGER));
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
    printf("\"make new state for\" add at %p; simply returning caller state\n", function);
    return caller_state;
  } else {
    printf("\"make new state for\" myfunc at %p\n", function);
    State * state = malloc(sizeof(State));
    state->at = 0;
    state->code =function;
    state->env = NULL;
    state->stack = caller_state->stack;
    state->code_size = 4; // TODO how do I know this? (Answer: size should be derivable from code block contents.)

    return state;
  }
}


int main (int argc, char ** argv) {
  // In this test we use a rather optimistic mix of direct integers and full pointer values.
  // In actual reality we might be able to combine direct (integer) values with memory indices (relative pointers).
  // The idea for now is that we retain some flexibility (and bragging rights) if we
  // at least try to keep any required data type encoding transparent to the eval loop.
  // Suggestion though:
  // 00xxxxxx... = positive int
  // 01xxxxxx... = primitive pointer
  // 10xxxxxx... = code or other data pointer (more info at pointer)
  // 11xxxxxx... = negative int (two's complement)

  PrimitiveCallback addptr = add;

  intptr_t myfunc[] = {(1<<2)|INTEGER, (1<<2)|INTEGER, ((intptr_t)&addptr)|PRIMITIVE, APPLY };
printf("add: %p myfunc: %p\n", add, myfunc);
  State * state = new_state();
  state->code = (void *)(intptr_t[]) {((intptr_t)myfunc)|NATIVE, APPLY };
  state->code_size = 2;
  state = eval(state);

  printf("Result of 1+1: %ld\n", (intptr_t) pop(state->stack) >> 2);
  return 0;
}
