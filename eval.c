#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "wonky.h"

#define KEYVAL_APPLY 0

// We expect the following callbacks to be supplied as by the library user:

extern ExecutionType get_execution_type();

/**
 * May be e.g.:
 * - New state with only a global parent state;
 * - New state based on object arg (so we supply stack through caller state
 * - Dynamic scope or block scope (based on caller state)
 */
extern State * make_new_state_for(void * function, State * caller_state);


State * new_state() {
  State * state = malloc(sizeof(State));
  state->at = 0;
  state->code = NULL;
  state->env = NULL;
  state->stack = new_stack();
  state->code_size = 0;
  return state;
}

// As opposed to the traditional "eval" / "apply" pair,
// our organisation appears to be: eval(code) = [resolve|apply]+
// which I think neatly represents our real-world situtation.
void eval(State * state) {

  Stack * state_stack = new_stack();

  // This is our main evaluation loop.
  //
  // Inside it, we switch execution contexts (states) all the time
  // non-recursively going from one function into the other
  // until we have nothing more to execute and just call 'return'.
  //
  // As one run through the loop equals one byte code being executed,
  // this context switching can be performed easily enough.
  while (1) {
    // Whenever we find ourselves at the end of our code,
    // return to a parent state
    while (! (state->at < state->code_size)) {
      State * parent = pop(state_stack);
      if (parent == NULL) return; // all done!

      // add return values
      if (has_item(state->stack)) { // it should; every statement has a return value
        push(&(parent->stack), pop(state->stack));
      }

      // don't destroy our state unless we know it's not used e.g. in a closure
      // (instead we just delegate the whole hot potato to GC)

      // and transfer control
      state = parent;
    }

    void * value = state->code[state->at];

    state->at++; // increment early so that it points to the next item when used as continuation

    if (value == KEYVAL_APPLY) {
      void * func;

      apply:

      func = pop(state->stack);
      ExecutionType func_type = get_execution_type(func);

      if (func_type < C_CALLBACK) push(&(state->stack), func); // value instead of a real function; handle gracefully
      if (func_type == C_CALLBACK) {
        // The value is a C callback;
        // This callback may make actual nested / recursive calls to 'eval'
        // using the C stack instead of our own 'state-stack'.

        // This is all OK enough if it can at least cooperate with our tail call
        // optimization, which at our side is as simple as:
        if(state->at < state->code_size) {
          // If at end just let callee return directly to the parent state.
          // Otherwise continue as normal and push our state:
          push(&(state->stack), state);
        }

        state = make_new_state_for(func, state);
        PrimitiveCallback cb = (PrimitiveCallback) func;
        bool apply_tail = cb(state);

        // explicitly mark this state to be 'at end' for the purposes of our loop
        state->at == state->code_size;

        // however:
        if (apply_tail) {
          // primitive has left us one final expression on our stack to apply.
          // This is by way of tail call optimization as the primitive has thus released its C stack.
          //
          // As opposed to elsewhere, this solution requires an actual jump to get to the right point in code.
          //
          // Slightly counter-intuitively, we retain the primitive's context to evaluate the tail,
          // but being marked as 'at end' should prevent it from adding to the stack during evaluation.
          goto apply;
        }
      } else if (func_type == CODE) {

        // again, only push our own state if not at tail
        if(state->at < state->code_size) {
          push(&state_stack, state);
        }

        state = make_new_state_for(func, state);
        continue; // not really necessary right now; we'll loop to that spot anyway
      }
    } else {
      // IDEALLY, we only need to do this at this point.
      // This is only possible when pre-translation has resolved all labels, blocks, etc. for us
      // (but retained simple values including label references).
      push(&(state->stack), value);
    }
  }
}
