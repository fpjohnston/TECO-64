### CTRL/T - Type character

*n*^T
- Types out to the terminal the character whose ASCII value is *n*. Whatever
normal type-out conversions may currently be in effect and applicable (such
as translation of control characters to up-arrow format) are done. The value
of *n* is used modulo 256 (except if it is -1; see below).

*n*:^T
- Outputs to the terminal the character whose ASCII value is *n*. Output is
done in "one-shot" binary mode; no type-out translations are done. The value
of *n* is used modulo 256 (except if it is -1; see below).

-1^T
- Types CR/LF. Equivalent to 13^T 10^T.

*m*,*n*^T
- Specifies a repeat count of *m* for the character whose ASCII value
is *n*. *n* may be -1.
