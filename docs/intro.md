% Introduction

# Why Lisp?

# Basic Concepts

Without further ado:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(printf "Hello, world!")
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Hylas -- Lisp in general -- has no syntax. Those two parentheses are all you will ever need. This is the sum total of Lisp syntax:

![](../res/img/forms.svg)

That is all there is to the syntax. No special characters, no `=` vs `==` errors, none of it. Unlike other languages, Hylas makes no syntactic distinction between, say, calls to user-defined functions and the compiler's own built-in, primitive statements. Consider:

# Function Calls

## C/C++

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.cpp}
fn(arg1, arg2, ...);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## Python

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.python}
fn(arg1, arg2, ...)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## Hylas

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(fn arg1 arg2 ...)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Statements

## C/C++

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.cpp}
 while(expression)
{
  ...code...
}

type fn(type arg1, type arg2, ...)
{
  ...code...
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## Python

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.python}
 while(expression):
  ...code...

def fn(arg1, arg2, ...):
  ...code...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 

## Hylas

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(function fn type ((arg1 type) (arg2 type))
  ...code...)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 

So that's all the syntax there is. Now let's go on to the important stuff.