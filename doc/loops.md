## TECO-64 - Looping Commands

Command strings in TECO may be iterated by placing them within angle
brackets to create a command loop. Loops may be nested so that one command
loop contains another command loop, which, in turn, contains other command
loops, and so on. The maximum depth to which command loops may be nested
is finite, due to memory limitations, but it does not have a specific limit.

If a command loop is preceded by a numeric argument, that number specifies
how many times the loop is executed. If a loop is not preceded by a
numeric argument, the loop will be executed until it is terminated
through some other mechanism, such as a semi-colon (;) command
following a search command.

The general form of the command loop is:

- *n*\<*command string*\>

where "command string" is the sequence of commands to be iterated and *n* is
the number of iterations.

- If *n* is supplied, then no limit is placed on the number of iterations.
- If *n* is <= 0, then the iteration is not executed at all; command control skips to the closing angle bracket.
- If *n* is > 0, then the iteration is performed *n* times.

~~Search commands inside command loops are treated specially. If a search
command which is not preceded by a colon modifier is executed within a
command loop and the search fails, a warning message is printed,
the command loop is exited immediately and the command following the
right angle bracket of the loop is the next command to be executed. If
an unmodified search command in a command loop is immediately followed
by a semicolon, it is treated as if it were a colon-modified search.~~