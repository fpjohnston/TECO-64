### R - Move Pointer Backward

R
- Moves the pointer backward across one character.

nR
- Executes the R command n times. If n is positive, the pointer
is moved backward across n characters. If n is negative, the
pointer is moved forward across n characters. If n is zero, the
position of the pointer is not changed.

-R
- Equivalent to -1R.

n:R
- Same as the nR command except that a value is returned. If
the command succeeded, then a value of -1 is returned. If the
command failed, then the buffer pointer is not moved and a
value of 0 is returned.

:R
- Equivalent to 1:R.
