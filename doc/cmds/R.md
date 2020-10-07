### R - Move Pointer Backward

R
- Moves *dot* backward across one character. Equivalent to -C.

*n*R
- Executes the R command *n* times, as follows:
    - *n* > 0 - *dot* is moved backward across *n* characters.
    - *n* < 0 - *dot* is moved forward across *n* characters.
    - *n* = 0 - *dot* is not changed.

-R
- Equivalent to -1R.

*n*:R
- Equivalent to *n*R except that -1 is returned if the
command succeeded. If the command failed, 0 is returned and *dot*
is not changed.

:R
- Equivalent to 1:R.
