#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "stack.h"

#define CHUNK_SIZE 10

Stack * new_stack() {
  Stack * stack = malloc(sizeof(Stack));
  stack->values = malloc(sizeof(void *) * CHUNK_SIZE);
  stack->size = 0;
}

void push(Stack ** stack, void * value) {
//  printf("pushing %d\n", (intptr_t) value);

  (*stack)->values[(*stack)->size] = value;
  (*stack)->size++;
  if (((*stack)->size % CHUNK_SIZE) == 0) {
    // N.b. this looks like a grow-only allocation strategy, but
    // after shrinking and growing a little again, it may actually
    // still result in the stack being downsized...
    (*stack) = realloc(stack, sizeof(void *) * ( (*stack)->size + CHUNK_SIZE));
  }
}

void * pop(Stack * stack) {
//  printf("popping\n");

  if (stack->size <=0) {
    return NULL;
  }
  return stack->values[--(stack->size)];
}
