![Logo](http://eudoxia0.github.com/Hylas-Lisp/img/logo.svg)

Hylas is a statically-typed, [wide-spectrum](http://en.wikipedia.org/wiki/Wide-spectrum_language), JIT-compiled dialect of Lisp that combines the performance and control of low-level languages with the advanced metaprogramming features of languages like Lisp.

# Examples

**Recursive, Tail Call-Optimized Fibonacci Function:**

```lisp
(function fib i64 ((n i64))
  (if (< n 2)
    n
    (add (fib (sub n 1))
         (fib (sub n 2)))))
```

**Calling a foreign function:**

```lisp
(foreign C printf i32 (pointer i8) ...)

(printf "Hello, world! This is a double, in scientific notation: %e", 3.141592)
```

**Using a foreign library:**

```lisp
(link "libSDL.so")

(foreign C SDL_Init void i64)
(foreign C SDL_Delay void i64)
(foreign C SDL_Quit void)

(structure SDL_Color
  (r            byte)
  (g            byte)
  (b            byte)
  (unused       byte))
```

# Types

<table>
    <tr>
        <td><strong>Kind</strong></td><td><strong>C</strong></td><td><strong>Hylas</strong></td>
    </tr>
    <tr>
        <td><strong>Integers</strong></td><td><code>char</code>, <code>short</code>, <code>int</code>, <code>long</code>, <code>long long</code>, signed and unsigned.</td>
        <td>i1, i2, i3... i8388607. (Number indicates the bit-width).<br> Signature is a property of <em>operations</em>, not types.
        Aliases exist for the most common ones:
        <ul>
        <li> <code>bool</code>: i1</li>
        <li> <code>char</code>, byte: i8 or i7, depending on the architecture</li>
        <li> <code>short</code>: i16</li>
        <li> <code>int</code>: i32</li>
        <li> <code>long</code>: i64</li>
        <li> <code>word</code>: i32 on 32-bit machines, i64 on 64-bit machines (Width of a machine word)</li>
        </ul>
        </td>
    </tr>
    <tr>
        <td><strong>Floating-Point</strong></td><td><span>float, double, long double</span></td><td><span>half, float, double, fp128, x86_fp80, ppc_fp128</span></td>
    </tr>
    <tr>
        <td><strong>Aggregate</strong></td><td><span>Structures (Can be opaque), arrays, pointers and unions. Has void pointers.</td><td>Structures (Can be opaque) and pointers. Arrays are pointers. Doesn't have void pointers, can be implemented through coercion functions.</span></td>
    </tr>
    <tr>
        <td><strong>Standard Library</strong></td><td><code>size_t</code>, <code>FILE*</code>, <code>_Bool</code></td><td><span>Hash Tables, Sequences (Resizable arrays, bound-checked arrays), filesystem-independent Filepath and Process objects...</span></td>
    </tr>
</table>
