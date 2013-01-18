% Language Reference

# Basic Operations

## `if`

## `def`

## `global`

# Basic Mathematics

## `add`

## `fadd`

## `sub`

## `fsub`

## `mul`

## `fmul`

## `udiv`

## `sdiv`

## `fdiv`

## `urem`

## `srem`

## `frem`

# Operations on Bitfields

## `shl`

## `lshr`

## `ashr`

## `bitwise-and`

## `bitwise-or`

## `bitwise-xor`

## `byte-swap`

## `count-ones`

# Type Coercion

## `truncate`

## `extend`

## `sextend`

## `zextend`

## `pointer->integer`

## `integer->pointer`

## `size`

## `bitcast`

## `cast`

# Functions


# Types

## `type`

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(type typename type-definition)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(type typename type-definition (doc "docstring"))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## `structure`

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(structure struct-name
	(member type)
	(member type)
	...)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(structure struct-name
    (member type)
	(member type)
	...
	(doc "docstring"))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## `make-structure`

## `access`

## `array`

## `nth`

# Generic Types

# Operations on Types

These operations, except for `typeof` don't take objects as arguments, but type signatures. They all act like macros: They don't actually _return_ things, they replace themselves with their output, but 'return' is still used.

## `typeof`

Returns the type of the argument.

```
(typeof 10) -> i64
(typeof (printf "Hello, World!")) -> i32
(sequence (typeof 10)) -> (sequence i64)
```

## `pointer`

Adds one level of pointer indirection to the argument.

```
i32 -> i32*
```

## `unpointer`

Removes one level of pointer indirection from the argument.

```
i64* -> i64
float** -> float*
```

## `indirection`

Returns the indirection level of the argument (In C parlance, the number of asterisks).

```
(indirection (pointer i64)) -> 1
(indirection (pointer (pointer i64))) -> 2
(indirection i64) -> 0
```

# Assembly

These operations allow you to embed LLVM Intermediate Representation or pure assembly into the code.

## `LLVM`

## `inline-LLVM`

## `asm`

## `inline-asm`

# Memory Management

## `create`

## `reallocate`

## `destroy`

## `allocate`

## `store`

## `load`

## `address`

# Metaprogramming

## `macro`

## `full-macro`

## `word-macro`

## `inspect`

# Externals

## `foreign`

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## `link`

Dynamically links to a foreign library.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(link "libSDL.so")
(link "/usr/lib/libkhtml.so.5")
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## `import`

Imports a `.hylas` file given its filepath.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(import "../include/regexp.hylas")
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## `from`

A nice abstraction over import.

The following:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(from Data from Serialization Formats import YAML)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

is essentially the same as doing:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(import "Data/Serialization\ Formats/YAML.hylas")
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Namespaces

## `namespace`

Opens a new namespace. All new symbols (Function, variable definitions, etc.) until the end of this form will be prefixed by all the namespace names in the namespace stack, separated by a single colon (':').

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(namespace Bank
    (doc "This namespace holds structures and functions related to the banking system of...")
    (enum AccountStatus ...)

    (structure Account
        (balance    double)
        (status     Bank:AccountStatus))
    
    (function createAccount Bank:Account ((name     string)
                                          (balance  double))
        ...))
        
(Bank:createAccount "John Smith" 314.5)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Nested namespaces:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(namespace Foo
  (namespace Bar
	(function Baz ...)))

(Foo:Bar:Baz ...)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Introspection