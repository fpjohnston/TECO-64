### L - Move Lines

L
- Advances the pointer forward across the next line terminator
(line feed, vertical tab, or form feed) and positions it at the
beginning of the next line.

*n*L
- Executes the L command *n* times. A positive value of n
advances the pointer to the beginning of the nth line following
its current position. A negative value of *n* moves the pointer
backwards to the beginning of the nth complete line preceding
its current position. If *n* is zero, the pointer is moved to the
beginning of the line on which it is currently positioned.

-L
- Equivalent to -1L.

### L - Line Count

0:L
- Total number of lines in buffer.

:L
- Equivalent to 0:L.

-1:L
- Number of lines preceding dot.

1:L
- Number of lines following dot.


