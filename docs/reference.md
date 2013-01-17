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

## `typeof`

## `pointer`

## `unpointer`

## `indirection`

# Assembly

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

## `import`

## `from`

# Namespaces

## `namespace`
