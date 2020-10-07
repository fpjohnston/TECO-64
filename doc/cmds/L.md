### L - Move Lines or Return Number of Lines

The L command command performs two functions. If executed without a colon
modifer, it moves *dot* forward or backward the specified number
of lines. If executed with a colon modifier, it returns a value equivalent
to the number of lines before or after *dot*.

L
- Advances *dot* forward across the next line terminator
(line feed, vertical tab, or form feed) and positions it at the
beginning of the next line.

*n*L
- Executes the L command *n* times, as follows:
    - *n* > 0 - *dot* is moved forward *n* lines.
    - *n* < 0 - *dot* is moved backward *n* lines.
    - *n* = 0 - *dot* is moved to the beginning of the
line on which it is currently positioned.

-L
- Equivalent to -1L.

0:L
- Returns the total number of lines in buffer. *dot* is not moved.

:L
- Equivalent to 0:L.

-1:L
- Returns the number of lines preceding dot.

1:L
- Returns the number of lines following dot.


