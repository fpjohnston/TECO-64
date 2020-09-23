### C - Move Pointer Forward

C
- Advances the pointer forward across one character.

nC
- Executes the C command n times. If n is positive, the pointer
is moved forward across n characters. If n is negative, the
pointer is moved backward across n characters. If n is zero,
the pointer position is not changed.

n:C
- Same as nC except that a value is returned. If the command
succeeded, -1 is returned. If the command failed, the pointer
does not move and a value of 0 is returned.

:C
- Equivalent to 1:C.

-C
- Equivalent to -1C.
