#ifndef STACK_H
#define STACK_H

typedef struct Stack {
  void ** values;
  int size;
} Stack;

Stack * new_stack();
void push(Stack ** stack, void * value);
void * pop(Stack * stack);

#define has_item(stack) (stack->size > 0)

#endif /* STACK_H */
