### C - Move Pointer Forward

C
- Advances *dot* forward across one character.

*n*C
- Executes the C command *n* times, as follows:
    - *n* > 0 - *dot* is moved forward across *n* characters.
    - *n* < 0 - *dot* is moved backward across *n* characters.
    - *n* = 0 - *dot* is not changed.

*n*:C
- Equivalent to *n*C except that -1 is returned if the
command succeeded. If the command failed, 0 is returned and *dot*
is not changed.

:C
- Equivalent to 1:C.

-C
- Equivalent to -1C.
