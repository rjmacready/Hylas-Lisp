% Types

# Overview

<table>
    <tr>
        <th><strong>Kind</strong></th><th><strong>C/C++</strong></th><th><strong>Hylas</strong></th>
    </tr>
    <tr>
        <td><strong>Integers</strong></td><td><span>`char`, `short`, `int`, `long`, `long long`, signed and unsigned.</span></td>
        <td><span>`i1`, `i2`, `i3`... `i8388607`. (Number indicates the bit-width).<br> Signature is a property of <em>operations</em>, not types.
        Aliases exist for the most common ones.</span></td>
    </tr>
    <tr>
        <td><strong>Floating-Point</strong></td><td><span>`float`, `double`, `long double`.</span></td>
		<td><span>`half`, `float`, `double`, `fp128`, `x86_fp80`, `ppc_fp128`.</span></td>
    </tr>
    <tr>
        <td><strong>Aggregate</strong></td><td><span>Structures (Can be opaque), arrays, pointers and unions. Has void pointers.</td><td>Structures (Can be opaque) and pointers. Arrays are pointers. Doesn't have void pointers, can be implemented through coercion functions.</span></td>
    </tr>
    <tr>
        <td><strong>Standard Library</strong></td><td><span>`size_t`, `FILE*`, `_Bool`.</span></td>
		<td><span>Hash Tables, Sequences (Resizable arrays, bound-checked arrays), filesystem-independent Filepath and Process objects...</span></td>
    </tr>
</table>

# Numbers

## Integers

Integers in LLVM can have any arbitrary bit-width: Anything from 1 bit (Boolean) to eight million, three hundred thousand bits and change (Little over a gigabyte). Integers are represented by the letter 'i' followed by the bit width. Since this approach may be prone to errors, some aliases are provided:

* `bool`: i1.
* `char`, `byte`: i8 or i7, depending on the architecture.
* `short`: i16.
* `int`: i32.
* `long`: i64.
* `word`: i32 on 32-bit machines, i64 on 64-bit machines.

The final alias, `word`, should be used like one would use `size_t` on C or C++: It is the width of a machine word, which is _usually_ the width of a pointer, and thus represents the largest array you can create in the system. If you are writing a datastructure to replace Sequences or something, you should use `word` to store the size of the internal array. If you are iterating over some sequence or array, you will want to use a `word` for the iterator variable. If you use `int` like so many people do in C, you might get cut off if your array is longer than the maximum number representable in an unsigned 32-bit integer (2^32^-1). If you use a `long`, that will work on both 32-bit and 64-bit machines, but on 32-bit you will be wasting 32 bits in every integer.

Using `word` also makes porting infinitely easier. Consider this scenario: The entire infrastructure of the dystopian cyberpunk future relies on legacy Python and Hylas, and someone wants to run your program in her asteroid-sized [rod logic](http://www.halcyon.com/nanojbl/NanoConProc/nanocon2.html) computer that has 128-bit wide pointers. Hylas has got you covered: The compiler will simply measure the width of the machine word, and define `word` to `i128`.

## Floating Point

LLVM comes with a wide range of floating point types, including big ones that might find a use in some areas of computing.

# Type Signatures



# Defining Your Own

Creating your own types is simple:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(type typename type)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 

Where _typename_ is the name of the type, and _type_ is the definition: The type signature of some already existing type. For example:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(type big-integer i512)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 

To add a docstring to your type, simply do this:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.commonlisp .Hylas}
(type typename type "docstring")
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 

# Structures
