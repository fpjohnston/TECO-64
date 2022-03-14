## TECO-64 - Insertion Commands

The following lists all of the text insertion commands. These commands cause the
character or characters specified in the command to be inserted into the edit buffer
at the current position of the buffer pointer. Following execution of an insertion
command, the pointer will be positioned immediately after the last character of
the insertion.

The length of an insertion command is limited primarily by the amount of
memory available for command string storage. When command string space
is about to run out, TECO will ring the terminal’s bell after each
character that is typed. Attempting to enter too many characters into
the current command string causes unpredictable results to occur and
should be avoided.

As explained [here](action.md), certain characters are filtered out by the
operating system and/or may perform special functions, and some characters are
immediate action commands and have special effect. If you want to insert such
characters into the edit buffer, use the *n*I\` command described in the following
table. It will insert any ASCII character into the buffer, including the special
characters that could not ordinarily be typed at a terminal.

| Command | Function |
| ------- | -------- |
| I*text*\` | The specified text string is entered into the buffer at the current position of the pointer, with the pointer positioned immediately after the last character of the insertion. |
| @I/*text*/ | Equivalent to the I command. |
| *n*I\` | Insert the single character whose ASCII code is *n* into the buffer at the current position of the buffer pointer. *n* is taken modulo 256. *n*I\` is used to insert characters that are not available on the user’s terminal or special characters such as DELETE which may not be inserted from a terminal with the standard I command. |
| *n*@I// | Equivalent to the *n*I\` command. |
| *m*,*n*I\` | Equivalent to executing the *n*I\' command *m* times. <br><br>If *m* is not specified, one character is inserted. <br><br>If *m* is 0, nothing is inserted. <br><br>If *m* is negative, then an IIA (invalid insert argument) error occurs. |
| *m*,*n*@I// | Equivalent to the *m*,*n*I\` command. |
| \<TAB\>*text*\` | Equivalent to the I command except that the \<TAB\> is part of the text which is inserted into the buffer. |
| FR*text*\` | Equivalent to -*n*DI/*text*/, where *n* is obtained from the most recent occurrence of the following: (a) the length of the most recent string found by a successful search command, (b) the length of the most recent text string inserted (including insertions from the FS, FN, or FR commands), or (c) the length of the string retrieved by the most recent "G" command. In effect, the last string inserted or found is replaced with *text*, provided that the pointer has not been moved. After execution of this command, the buffer pointer is positioned immediately after *text*. |
| @FR/*text*/ | Equivalent to FR*text*\`.
| *n*FR*text*\` | Equivalent to *n*DI*text*\`. If *n* is less than 0, characters preceding the buffer pointer are deleted and replaced with *text*. If *n* is greater than 0, characters following the buffer pointer are deleted and replaced with *text*. In either case, the buffer pointer is left positioned immediately after *text* upon completion of this command. |
| *n*@FR/*text*/ | Equivalent to *n*FR*text*\`. |
| *m*,*n*FR*text*\` | Equivalent to *m*,*n*DI*text*\`. The characters between buffer positions m and n are deleted and replaced with *text*. The buffer pointer is left positioned immediately after *text* upon completion of this command. |
| *m*,*n*@FR/*text*/ | Equivalent to *m*,*n*FR*text*\`. |
| *n*\\ | The backslash command preceded by an argument inserts the value of *n* into the edit buffer at the current position of the pointer, leaving the pointer positioned after the last digit of the insertion. The insertion is either signed decimal, unsigned octal, or unsigned hexadecimal. Note that \\ is a "bidirectional" command. *n*\\ inserts a string into text while \\ without a numeric argument returns a numeric result. |
