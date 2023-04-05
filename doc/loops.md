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

- *n*&lt;command string>

where "command string" is the sequence of commands to be iterated and *n* is
the number of iterations.

- If *n* is omitted, then no limit is placed on the number of iterations.
- If *n* is less than or equal to 0, then the iteration is not executed at all; command control skips to the closing angle bracket.
- If *n* is greater than 0, then the iteration is performed *n* times.

| Command | Function |
| ------- | -------- |
| &lt; | Marks the start of an infinite loop. |
| *n*&lt; | Marks the start of a loop that will be executed *n* times (unless otherwise terminated by another command. |
| > | Marks the end of a loop, and causes TECO to bump the current iteration count by 1 and test to see if the resulting count is equal to the maximum count permitted for the iteration (specified as an argument before the matching &lt;). If the iteration count has not expired, then control returns to the command following the &lt; at the beginning of the current iteration. If the iteration has expired, then command execution continues with the character following the >. If this command is encountered outside of an iteration (within the current macro level), then the ?BNI error message is issued. |
| F> | Causes TECO to branch (flow) to the end of the current iteration. TECO effectively resumes execution at the matching >. The iteration count is tested as usual. If it has not expired, control returns back to the start of the iteration with the count having been incremented by 1. If the count was up, the iteration is exited and control continues with the first command after the >. If this command is encountered outside of an iteration, it has the same effect as the &lt;*delim*>&lt;*delim*> command. |
| F&lt; | Causes TECO to branch (flow) to the start of the current iteration. TECO effectively resumes execution at the first command following the &lt; at the beginning of the current iteration. The iteration count is not affected. If this command is issued outside of an iteration, it causes TECO to branch back to the start of the current command string (in the current macro level). |

### Search Commands in Loops

Search commands inside command loops are treated specially. If a search
command which is not preceded by a colon modifier is executed within a
command loop and the search fails, a warning message is printed,
the command loop is exited immediately and the command following the
right angle bracket of the loop is the next command to be executed. If
an unmodified search command in a command loop is immediately followed
by a semicolon, it is treated as if it were a colon-modified search.

| Command | Function |
| ------- | -------- |
| \; | Causes TECO to branch out of the current iteration, if the immediately preceding search (or search and replace) command failed. In that case, control resumes at the character following the matching > at the end of the current iteration. On the other hand, if the preceding search succeeded, command execution continues with the character following the ;. If this command is encountered from outside of an iteration (in the current macro level), then the ?SNI error message is issued. |
| n; | Causes TECO to branch out of the current iteration if the value of *n* is greater than or equal to 0. In that case, command execution resumes at the character following the matching > at the end of the current iteration. On the other hand, if *n* is less than 0, command execution continues with the character following the ;. If this command is encountered from outside of an iteration (in the current macro level), then the ?SNI error message is issued. |
| :; | Causes TECO to branch out of the current iteration if the immediately preceding search (or search and replace) command succeeded. In that case, control resumes at the character following the matching > at the end of the current iteration. On the other hand, if the preceding search failed, command execution continues with the character following the ;. If this command is encountered from outside of an iteration (in the current macro level), then the ?SNI error message is issued. |
| *n*:; | Causes TECO to branch out of the current iteration if the value of *n* is less than 0. In that case, command execution resumes at the character following the matching > at the end of the current iteration. On the other hand, if *n* is greater than or equal to 0, command execution continues with the character following the ;. If this command is encountered from outside of an iteration (in the current macro level), then the ?SNI error message is issued. |


