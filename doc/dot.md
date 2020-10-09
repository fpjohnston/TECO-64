### TECO-64 - Dot Commands

The following list all of the commands that move the buffer pointer (*dot*).
These commands may be used to move *dot* to a position between any two
characters in the buffer, but they will not move *dot* across a buffer
boundary. If any R or C command attempts to move *dot* backward beyond
the beginning of the buffer or forward past the end of the buffer, the command is
ignored and an error message is printed. If any L command attempts to exceed
the buffer boundaries, *dot* is positioned at the boundary which would
have been exceeded and no error message is printed.

#### Character commands

| Command | Function |
| ------- | -------- |
| C | Advances *dot* forward across one character. |
| *n*C | Executes the C command *n* times, as follows: <br><br>If *n* \> 0, *dot* is moved forward across *n* characters. <br><br>If *n* \< 0, *dot* is moved backward across *n* characters. <br><br>If *n* = 0, *dot* is not changed. |
| -C | Equivalent to -1C. |
| *n*:C | Equivalent to *n*C except that -1 is returned if the command succeeded. If the command failed, 0 is returned and *dot* is not changed. |
| :C | Equivalent to 1:C. |

#### Jump commands

| Command | Function |
| ------- | -------- |
| *n*J | Moves *dot* to a position immediately following the *n*th character in the buffer. *n* must be non-negative. |
| J | Moves *dot* to a position immediately preceding the first character in the buffer. Equivalent to 0J. |
| ZJ | Moves *dot* to a position immediately following the last character in the buffer. |
| *n*:J | Same as the *n*J command except that -1 is returned if new position of *dot* is inside the buffer. If the new position of *dot* is outside the buffer, 0 is returned and *dot* is not changed. |
| :J | Equivalent to 0:J. |

#### Line commands

| Command | Function |
| ------- | -------- |
| L | Advances *dot* forward across the next line terminator (line feed, vertical tab, or form feed) and positions it at the beginning of the next line. |
| *n*L | Executes the L command *n* times, as follows: <br><br>If *n* \> 0, *dot* is moved forward *n* lines. <br><br>If *n* \< 0, *dot* is moved backward *n* lines. <br><br>If *n* = 0, *dot* is moved to the beginning of the line on which it is currently positioned. |
| -L | Equivalent to -1L. |

#### Reverse commands

| Command | Function |
| ------- | -------- |
| R | Moves *dot* backward across one character. Equivalent to -C. |
| *n*R | Executes the R command *n* times, as follows: <br><br>If *n* \> 0, *dot* is moved backward across *n* characters. <br><br>If *n* \< 0, *dot* is moved forward across *n* characters. <br><br>If *n* = 0, *dot* is not changed. |
| -R | Equivalent to -1R. |
| *n*:R | Equivalent to *n*R except that -1 is returned if the command succeeded. If the command failed, 0 is returned and *dot* is not changed. |
| :R | Equivalent to 1:R. |

