### T - Type Buffer Contents

T
- Types out the contents of the buffer from the current position
of *dot* through and including the next line terminator character.

*n*T
- Types *n* lines, as follows:
    - *n* > 0 - Types the *n* lines following the
current position of *dot*.
    - *n* < 0 - Types the *n* lines preceding *dot*.
    - *n* = 0 - Types the contents of
the buffer from the beginning of the line on which *dot*
is located up to *dot*.

-T
- Equivalent to -1T.

*m*,*n*T
- Types out the contents of the buffer between pointer positions *m* and *n*.

.,.+*n*T
- Types out the *n* characters immediately following *dot*. *n* should be
greater than zero.

.-*n*,.T
- Types the *n* characters immediately preceding *dot*. *n* should be
greater than zero.

HT
- Types out the entire contents of the buffer.