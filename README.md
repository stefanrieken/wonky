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


## Relation to Selfish byte code

 o Both byte codes should translate names to 'name+num args' under the hood, if
   only to allow overloading based on number of arguments (the stack should
   normally work itself out).

 o Selfish byte code DOES allow for label references as in Wonky: by means of an
   explicit extra datatype.

 o Selfish byte code uses '0' as an explicit clear-stack between expressions.
   This is less of a requirement if either the target language or its translation
   does not implicitly sequence multiple expressions as one.

 o Selfish byte code uses negative numbers to mean 'apply(lookup(-number), stack)' .
   This rules out direct use of pointers (unless they can be limited to positives).
   The combined lookup-and-apply also rules out the execution of unnamed values on
   stack (function selection expressions). This can quite simply be added in
   translation (add explicit call to e.g. 'apply' if the preceding item was itself
   an invocation):

   LISP:
     (if (< x 0) - +) x 1)
   =>
     1 x + - 0 x .< .if .apply
   => translate back:
     (apply (if (< x 0) - +) x 1)

   => to Selfish:
     x.apply( (x < 0).if(#-, #+) , 1);

## Open questions applicable to either VM:
   - If object-based lookup, where do we lookup a zero-arg func?
   - Do we at all need to refer to local vars by name?

     Example:

     // context a:
     (define a 3)

     (define foo (f)
       (let (b 4) // 'b' is a known location at compile time
         (f b)    // position of arg 'f' in memory is known as well
       )
     )

     (foo (lambda(x) (add x a) ) ) // 'foo' and 'a' are also known locations at compile time


So in short, the data location for all variables is known at compile time,
even though their references may be spread over different (parent) contexts.
Overall, this implies that our code may shift (either in runtime translation
or in basic definition)  from listing names => item positions (=> direct pointers)

(In this case, the 'label' data type should not normally pop up in our
pre-translated stream anymore, and so can be used explicitly for label
references as well.)

NOTE: in case of no local vars by name, then calls should still be referenced
by label. Lookup may be externalized by callback(!), going so far as having
the Environment struct abstract to this library.
In this case our knowledge of type may also be extremely limited!
(data, primitive, code; MAYBE lambda as a separate case (maybe not!))


