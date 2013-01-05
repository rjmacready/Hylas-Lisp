% Functions

## Common Functions

## Calling Conventions

## Inlining

## Recursive Functions

One of the most common objections to using recursion in production code is that recursive solutions to problems use more stack space than iterative ones, and calling a function introduces much overhead. LLVM supports something called [Tail-call optimization](http://c2.com/cgi/wiki?TailCallOptimization), which allows some recursive functions to execute using _a single stack frame_.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .numberLines .Hylas}
(recursive fib i64 ((n i64))
  (if (< n 2)
    n
    (add (fib (sub n 1))
         (fib (sub n 2)))))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
