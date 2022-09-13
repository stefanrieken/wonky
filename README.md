#  Wonkybytes
The One Key-value byte code evaluator

That keyword is: 'apply' (value 0; or perhaps MAX_INT)
All other values contain or reference data / method objects / items.

Details of application may vary in detail for the needs of different languages.
As part of a pre-/mixed fix-to-postfix interpreter or compiler, this forms the
final "execution" step.

We may skip an Abstract Syntax Tree (AST) form altogether for those languages
that both allow it (simple parsing rules) and don't support user manipulation
of said AST (as with LISP macros).

        [code] -> [parse()] -> [AST (LISP linked lists)] -> [to_postfix()] -> [code] -> [eval]

# Usage

'eval' will execute a sequence of pointer sized bytecode, combined with an
environment object, as follows:

0) All bytecodes are type-tagged at their 2 least significant bits, like so:

        00 - Label reference (>> 2 to produce label's number; '0' is reserved for 'apply')
        01 - Integer (>> 2 to produce value)
        10 - Native data / code pointer (mask off to produce aligned pointer)
        11 - Primitive pointer (mask off to produce aligned pointer)

1) All values will be pushed to a stack until an 'eval' marker is met.
   This is a label reference of value zero or below; the latter causes its
   absolute label value to be pushed before proceeding to 2).

2) Upon an 'eval' marker, the top of the stack will be executed depending on its
   byecode type:

        00 - Dynamic function lookup (using a library-user supplied callback) and invocation
        01 - Unspecified: either same as above, or return own int value
        10 - Eval the native data as specified under 3)
        11 - Invoke the C callback; its form should be as specified by type PrimitiveCallback

   In common usage, each execution takes a statically defined number of arguments
   from the stack and pushes a single result in return.

3) In the case of native data being evaluated, the VM will assume it presents
   as another sequence of byte codes, and execute it as if a fresh call to
   'eval' was made, but allowing the library-user to extend or replace the
   environment object as fits the situation, again by means of a callback.

   As with executing in-line integers, executing native data of any other type
   is an error from which the VM may recover by simply returning its value.

   In general, the preparation of a coherently interlinked environment for this
   low-level execution engine is the responsability of the caller.

4) Upon reaching the end of the code string, the value on top of the stack is
   returned. In case of a nested call, this is pushed to the stack of the caller,
   whose execution is subsequently resumed. Otherwise, this value is returned
   as a final result.

5) 'eval' implements tail call optimization both on native code and primitive
   callbacks. Neither our own stack, nor that of C, should grow as a result of
   (complicated) tail recursion.


## Dynamic invocations

Before dynamic invocation, our bytecode enviroment is (suprisingly) static, in
the sense that we can directly point to the location of every variable in the
system; and new (dynamic) values are solely obtained as the return value of an
expression.

If (all or some) calls are to be resolved dynamically, by definition this means
that we cannot pre-select their implementation in bytecode. Instead, we would
want to 'execute' the label, as such:

        ('message object) // lookup and execute; equiv.: object.message();

What's fortunate here, is that the data type ('label reference') itself points
out that this particular call must be a dynamic lookup. So from the point of
bytecode, we can remain language-agnostic, supporting both dynamic and static
lookup at no extra cost. (Though from a language perspective, unbounded mixing
of both styles usually results in a disastrous user experience.)

As CLOS has demonstrated, the above syntax extension may actually be mimicked
using a common LISP macro, that extends into an expression such as:

        ((lookup object 'message) object)

To be perfectly honest, if the call to 'lookup'  -a static global method- is
sufficiently performant, this is a decent strategy. CLOS goes on to implement
things like this overloaded lookup itself through more complicated macros. In
short, the only true critique on CLOS is that it is nested in the static-lookup
LISP environment, resulting in that disastrous mixing of styles we mentioned
earlier.

In spite of what CLOS demonstrates, if messaging is a basic requirement (and
macros only secondary), it makes sense to have it as a built-in convenience, as
opposed to a built-on one. And seeing how elegantly it can be supported on the
bytecode level, I see little reason not to.


## Bytecode consequences

In theory we could do without type-tagging our bytecode as long as the value
being pointed to (at runtime) is type-tagged itself (or otherwise recognizable).
In practice this means that our bytecode must exist purely of pointers (relative
indices) to type-tagged data.

This extra indirection may quickly become awkward for any value that could be
represented in-line at no extra cost: (small) integers, label references and,
if compressible to this form, perhaps even pointers to primitive functions.

There are many ways to introduce this type-tagging without requiring the
addition of a full byte or word to our bytecode, but a particularly prolific
one appears to be using the least significant bits in word-aligned pointers,
which on a 32-bit system gives us 2 bits' worth of type information for free;
the only cost worth considering being that of human readability of this binary
data.

(If we want to keep byte code typing at a minimum, we could just distinguish
between pointers and in-line integers, and treat the latter as label references
as well. Dynamic lookup, then, consists of establishing whether the function
argument is a direct integer, and subsequently treat it as a label reference.)

We use the negative values as a shorthand for direct application of the message,
saving a step in relation to using the 'apply' keyvalue (a zero).

## Lookup semantics

Common dynamic lookup would find an implementation through the first argument,
which is the basic form of object-oriented overloading. If multiple potiential
matches are found, they may or may not be further distinguished based on
argument type, if this data is available for the method.

(This is called multiple overloading, or "multimethods". Eventually it may
depend on the internal organisation of this lookup whether people will tend to
qualify it as "object oriented" or as "dynamic function matching"; which is a
point about programming paradigms I am keen to make.)

A cost-effective solution to support at least overloading by number of arguments
alone, is to add the number of arguments to the method's label in bytecode, so
that the above user-written ('message object) actually translates into:

        ('message1 object)

Should you have local access to this function, and therefore not require dynamic
lookup, the same translation should still take place. We may have (and want) to
break with this transparency when accessing the label directly.

        (message object) // => (message1 object)
        (send x 'message1)

The above raises another question, which is what to do when a message is invoked
without any arguments (message0). It seems like it would make it a dynamic, but
not object-oriented lookup in the current environment, which may not make much
sense if this environment is statically available at runtime. In light of this
evidence, we will presenly assume this behaviour to be undefined / unsupported.

On a final, practical note: as the core bytecode interpreter doesn't care about
the details of the lookup function, it is very well possible to start out simple
by only supporting basic object-oriented overloading.

## Lambdas vs blocks

If we call a native code block, how do we find out what kind of environment it
needs?

- A code block uses or extends the existing environment. In its representation,
  it is a plain LISP list (compiled to bytecode).
- A Lambda is the return value of a call to 'lambda', and contains both a code
  block and the Environment where it was created (= 'lexical scoping'), which
  it uses / extends instead. In present implementations I simply chain (cons)
  the environment in front of the code block. So: a Lambda is recognizable by
  the type of its first list item.
