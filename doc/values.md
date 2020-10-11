### TECO-64 - Special Numeric Values

TECO maintains several state variables, which may be used as numeric
arguments to TECO commands. When the command is executed, the current value
of the designated variable becomes the numeric argument of the command.

Some of the characters which stand for specific values associated with the
edit buffer have been introduced earlier in this manual. For example, the dot
character (.), which represents the current pointer position, may be used in
the argument of a T command. The command ".,.+5T" causes the 5 characters
following the buffer pointer to be typed out. When this command is executed,
the number of characters preceding the buffer pointer is substituted in each
case for *dot*. The addition is then carried out, and the command is executed
as though it were of the form *m*,*n*T.

The following lists all of the characters which have special numeric values.
Any of these characters may be used as numeric argument in place of the value
it represents.

| Character(s) | Function |
| ------------ | -------- |
| B | Always equivalent to zero. Thus, B represents the position preceding the first character in the buffer. |
| Z | Equivalent to the number of characters currently contained in the buffer. Thus, Z represents the position following the last character in the buffer. |
| . | *dot* is equivalent to the number of characters between the beginning of the buffer and the current position of the pointer. Thus *dot* represents the current position of the pointer. |
| H | Equivalent to the numeric pair "B,Z", or "from the beginning of the buffer up to the end of the buffer." Thus, H represents the whole buffer. |
| *n*A | Equivalent to the ASCII code for the .+*n*+1th character in the buffer (that is, the character to the right of buffer pointer position .+*n*). The expression -1A is equivalent to the ASCII code of the character immediately preceding the pointer and 0A is equivalent to the ASCII code of the character immediately following the pointer (the current character). If the character position referenced lies outside the bounds of the edit buffer, this command returns a -1. |
| *n*:L | Returns a count of buffer lines, according to the value of *n*. *dot* is not moved.<br><br>If *n* = 0, returns the total number of lines in the buffer. <br><br>If *n* \< 0, returns the number of lines preceding *dot*. <br><br>If *n* \> 0, Returns the number of lines following *dot*. |
| :L | Equivalent to 0:L. |
| M*q* | The M*q* command (execute the contents of the text storage area of Q-register *q as a command string) may return a numeric value if the last command in the string returns a numeric value and is not followed by an ESCape.
| Q*q* | Equivalent to the numeric value stored in Q-register *q*. |
| :Q*q* | Equivalent to the number of text bytes stored in Q-register *q*. |
| \\ | Backslash is equivalent to the numeric value of the digit string in the edit buffer at the current pointer position, interpreted in the current radix. The pointer is moved to the end of the digit string. |
| \^B | \<CTRL/B\> is equivalent to the current date via the equation ((year - 1900)\*16+month)\*32+day. |
| \^E | \<CTRL/E\> determines whether or not the P command (and related operations) append a form feed to the contents of the buffer on output. If \^E is -1, a form feed is appended; if \^E is 0, no form feed is appended. Each time text is read into the edit buffer TECO sets the \^E flag. If the text read terminated due to a form feed (i.e., if the edit buffer was loaded with a "complete page"), \^E is set to -1. If the text read terminated because the buffer was filled to capacity before a form feed was encountered or because there was no form feed (i.e., the edit buffer was not loaded with a "complete page"), \^E is set to 0. <br><br>You can set the value of the \^E flag directly, overriding the value set by the most recent buffer read. Note that any non-zero value will be treated as equivalent to -1. |
| \^H | \<CTRL/H\> is either equivalent to the number of seconds since midnight (if E1&2 is clear), or the number of milliseconds since midnight (if E1&2) is set). |
| \^N | \<CTRL/N\> is the end of file flag. It is equivalent to -1 if the file open on the currently selected input stream is at end of file, and 0 otherwise. |
| *n*\^Q | *n*^QC is identical to *n*L. This command returns the number of characters between the buffer pointer and the nth line separator (both positive and negative). This command converts line oriented command argument values into character-oriented argument values. Used after an expression. |
| \^S | \<CTRL/S\> is equivalent to the negative of the length of the last insert, string found, or string inserted with a G command, whichever occurred last. To back up the pointer to the start of the last insert, string found, etc., type \^SC. |
| \^T | \<CTRL/T\> is equivalent to the ASCII code for the next character typed at the terminal. Every \^T command executed causes TECO to pause and accept one character typed at the terminal. See the ET flag description for variations. |
| \^Y | \<CTRL/Y\> is equivalent to ".+^S,.", the *m*,*n* numeric argument spanning the text just searched for or inserted. This value may be used to recover from inserting a string in the wrong place. Type "^YXSFR`" to store the string in Q-register S and remove it from the buffer. You can then position the pointer to the right place and type "GS" to insert the string. |
| \^Z | \<CTRL/Z\> is equivalent to the total space occupied by text in the Q-registers (including the command line currently being executed). |
| \^\^x | The combination of the Control-caret (double caret or double up-arrow) followed by any character is equivalent to the value of the ASCII code for that character. The "x" in this example may be any character that can be typed in to TECO. |

#### Mode Control Flags

The following mode control flags return numeric values. The use of these flags is
described in (TBD).

| Flag | Function |
| ---- | -------- |
| E1 | Current value of the extended features flag. |
| E2 | Current value of the command restrictions flag. |
| E3 | Current value of the file operations flag. |
| E4 | Current value of the display mode flag. |
| ED | Current value of the edit level flag. |
| EH | Current value of the help level flag. |
| EO | Major version number for TECO-64 (200+). |
| 0:EO | Equivalent to EO. |
| -1:EO | Minor version number for TECO-64. |
| -2:EO | Patch version number for TECO-64. |
| ES | Current value of the search verification flag. |
| ET | Current value of the type out control flag. |
| EU | Current value of the upper/lower case flag. |
| EV | Current value of the edit verify flag. |
| \^X | Current value of the search mode flag. |