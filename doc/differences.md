### TECO-64 - TECO C Differences

#### Overview

The following describes the known differences in behavior between TECO-64
version 200 and TECO C version 147 (not including the new or enhanced
commands described in other documentation).

#### Features Omitted

- ^P command, used to set debugging parameters.
- EZ command, used to read or set mode control bits. Most bits have not
been implemented, but alternative functionality has been provided for
the following:
    - EZ&32 - Add line between text and command windows. Equivalent to E4&2
in TECO-64 (note also E4&4 to include status).
    - EZ&128 - Do not stop read on FF. Equivalent to E3&1 in TECO-64.
    - EZ&256 - Use Unix-style newline terminators. See description of E3&2,
E3&4, and E3&8 in TECO-64 to achieve the same behavior.

#### Features Added

- EE command.
- MLP and MRP errors, for missing left and right parentheses, respectively.

#### Features Changed

- Use of an EI command inside of an indirect command file is now allowed and does not cause a crash.
- 0EJ returns the process ID, not the parent process ID. 0:EJ has been
added to return the parent process ID.
- The EG and EX commands implement file protection by issuing an error
to prevent loss of data if the text buffer is not empty.
- The setting of the EU flag does not affect the output of echoed
characters in commands.
- Commands that do not use numeric values (e.g., apostrophe) do not pass
those values to subsequent commands.
- Hexadecimal digits are not allowed in expressions if the radix is set to 16.
