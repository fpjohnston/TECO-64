### TECO-64 - Deletion Commands

The following summarizes the text deletion commands, which permit deletion of
single characters, groups of adjacent characters, single lines, or groups of
adjacent lines.

#### Delete commands

| Command | Function |
| ------- | -------- |
| D | Delete the first character following the current position of *dot*. |
| *n*D | Execute the D command *n* times. <br><br> If *n* > 0, the *n* characters following *dot* are deleted.  <br><br> If *n* < 0, the *n* characters preceding *dot* are deleted. <br><br> If *n* = 0, the command is ignored. |
| -D | Equivalent to -1D. |
| *m*,*n*D | Equivalent to *m*,*n*K. |
| *n*:D | Same as *n*D but returns -1 if the command succeeds, and 0 if the command failed because the range of characters to be deleted fell outside the edit buffer. |
| *n*@FD/*text*/ | Search for the *n*th occurrence of *text* and delete it. *n* may be negative. |
| @FD/*text*/ | Equivalent to 1@FD/*text*/. |
| @FR// | Equivalent to -*n*D where *n*  is the length of the last insert, get or search command. See the description of the @FR/*text*/ command for more details. |

#### Kill commands

| Command | Function |
| ------- | -------- |
| K | Deletes the contents of the buffer from the current position of the buffer pointer through and including the next line terminator character. |
| *n*K | Executes the K command *n* times. <br><br>If *n* is positive, the *n* lines following *dot* are deleted. <br><br>If *n* is negative, the *n* lines preceding *dot* are deleted. <br><br>If *n* is zero, the contents of the buffer from the beginning of the line on which the pointer is located up to the pointer is deleted. <br><br>It is not an error if more lines are specified than occur when a boundary of the edit buffer is encountered. |
| -K | Equivalent to -1K. |
| *m*,*n*K | Deletes the contents of the buffer between pointer positions *m* and *n*. *dot* moves to the point of the deletion. The ?POP error message is issued if either *m* or *n* is out of range. |
| *n*FK/*text*/ | Search for the *n*th occurrence of *text, and delete all characters in the edit buffer between the pointer positions before and after the search. |
| @FK/*text*/ | Equivalent to 1FK/*text*/. |
| HK | Deletes the entire contents of the buffer. |