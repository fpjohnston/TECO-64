## TECO-64 - Error Messages

If TECO is unable to execute a command, either because of a syntax,
system, or other error, it aborts execution of the command, prints an
error message, ignores all following commands, resets its state, and
prints its prompt to indicate that the user can enter new commands.

All error messages are of the form:

?XXX *message*

where XXX is an error code and *message* is a description of the error.
Depending on the setting of the EH flag, more information may also be
printed.

Some messages mention a specific character or string of characters
associated with the error. In these error messages, TECO represents
any non-printing special characters as described in (TBD).

TECO can also print warning messages. These do not abort the
command being executed.

| Message | Description |
| ------- | ----------- |
| <nobr>\%Superseding existing file</nobr> | The file specified to an EW command already exists, and the old copy will be deleted if the file is closed with an EC or EX command. The EK command may be used to undo the EW command. |
| <nobr>\%Search failure in loop</nobr> | A search command has failed inside a loop. A ; (semi-colon) command immediately following the search command can be used to suppress this message. After printing the message, the iteration is terminated, i.e., TECO simulates an F> command. |

The following error messages are listed alphabetically by their three-letter
code.

| Error | Message            | Description |
| ----- | ------------------ | ----------- |
| <nobr>?ARG</nobr> | <nobr>Improper Arguments</nobr> | Three or more numeric arguments were specified (e.g., a,b,c or H,c). |
| <nobr>?ATS</nobr> | <nobr>Invalid or extraneous at sign</nobr> | An at sign preceded a command that does not allow at signs, or there were too many at signs specified for the command. |
| <nobr>?BAT</nobr> | <nobr>Bad tag '!x!'</nobr> | An O command was specified with an invalid tag. Tags may only contain graphic ASCII characters other than commas (which are reserved for computed GOTOs). Leading and trailing spaces are ignored, but embedded spaces are not allowed. |
| <nobr>?BNI</nobr> | <nobr>\> not in iteration</nobr> | There is a close angle bracket not matched by an open angle bracket somewhere to its left. (Note: an iteration in a macro stored in a Q-register must be complete within the Q-register.) |
| <nobr>?CHR</nobr> | <nobr>Invalid character for command</nobr> | A non-ASCII character preceded an EE command. |
| <nobr>?COL</nobr> | <nobr>Invalid or extraneous colon</nobr> | A colon preceded a command that does not allow colons, or there were too many colons specified for the command. |
| <nobr>?DIV</nobr> | <nobr>Division by zero</nobr> | An attempt was made to divide a number by zero. |
| <nobr>?DPY</nobr> | <nobr>Display mode initialization error</nobr> | An error occurred during initialization of display mode. |
| <nobr>?DTB</nobr> | <nobr>Delete too big</nobr> | An nD command has been attempted which is not contained within the current page. |
| <nobr>?DUP</nobr> | <nobr>Duplicate tag '!x!'</nobr> | An O command found a duplicate tag within the command string. |
| <nobr>?EGC</nobr> | <nobr>EG command is too long</nobr> | An EG command was longer than 200 characters. |
| <nobr>?FIL</nobr> | <nobr>Invalid file 'x'</nobr> | An attempt was made to open a directory, FIFO, socket, or similar file specification instead of a regular file. |
| <nobr>?FNF</nobr> | <nobr>File not found 'filespec'</nobr> | The requested input file could not be located. If this occurred within a macro the colon modified ER or EB command may be necessary. |
| <nobr>?ICE</nobr> | <nobr>Invalid ^E command in search argument</nobr> | A search argument contains a ^E command that is either not defined or incomplete. The only valid ^E commands in search arguments are: ^EA, ^ED, ^EV, ^EW, ^EL, ^ES, ^E<NNN>, and ^E[A,B,C,...]. |
| <nobr>?IE1</nobr> | <nobr>Invalid E1 command</nobr> | An invalid E1 command has been executed. The E1&32 bit may not be set while executing an old-style EI command, and may not be reset while executing a new-style EI command. |
| <nobr>?IEC</nobr> | <nobr>Invalid character 'x' after E</nobr> | An invalid E command has been executed. The E character must be followed by an alphabetic to form a legal E command (i.e., ER or EX). |
| <nobr>?IFC</nobr> | <nobr>Invalid character 'x' after F</nobr> | An invalid F command has been executed. |
| <nobr>?IFE</nobr> | <nobr>Ill-formed numeric expression</nobr> | The numeric expression preceding a command doesn't make sense. For example, 5+ isn't a complete expression. |
| <nobr>?IFN</nobr> | <nobr>Invalid character 'x' in filename</nobr> | The filespec as an argument to one of the E commands is unacceptable to the system. The file specification must be appropriate to the system in use. |
| <nobr>?IIA</nobr> | <nobr>Invalid insert arg</nobr> | A command of the form "nItext`" was attempted. Combining character (nI`) and text (Itext`) insertions is invalid. |
| <nobr>?ILL</nobr> | <nobr>Invalid command 'x'</nobr> | An attempt has been made to execute an invalid TECO command. |
| <nobr>?ILN</nobr> | <nobr>Invalid number</nobr> | An 8 or 9 has been entered when the radix of TECO is set to octal. |
| <nobr>?IMA</nobr> | <nobr>Invalid m argument</nobr> | An m argument was provided to a command which does not allow it. |
| <nobr>?INA</nobr> | <nobr>Invalid n argument</nobr> | An n argument was provided to a command which does not allow it. |
| <nobr>?INI</nobr> | <nobr>Initialization error</nobr> | A fatal error occurred during TECO initialization. |
| <nobr>?IQC</nobr> | <nobr>Invalid " character</nobr> | One of the valid " commands did not follow the quote character. |
| <nobr>?IQN</nobr> | <nobr>Invalid Q-register name 'x'</nobr> | An invalid Q-register name was specified in one of the Q-register commands. |
| <nobr>?IRA</nobr> | <nobr>Invalid radix argument to ^R</nobr> | The argument to a ^R radix command must be 8, 10, or 16. |
| <nobr>?ISA</nobr> | <nobr>Invalid search arg</nobr> | The argument preceding a search command is 0. This argument must not be 0. |
| <nobr>?ISS</nobr> | <nobr>Invalid search string</nobr> | One of the search string special characters (^Q, ^V, ^W, etc.) would have modified the search string delimiter (usually ESCAPE). |
| <nobr>?IUC</nobr> | <nobr>Invalid character 'x' following ^</nobr> | The character following an ^ must have ASCII value between 64 and 95 inclusive, or between 96 and 122 inclusive. |
| <nobr>?KEY</nobr> | <nobr>Key 'x' not found</nobr> | The text argument to an FM command specified an invalid function, cursor, keypad, or other special key. |
| <nobr>?MAP</nobr> | <nobr>Missing ’</nobr> | Every conditional (opened with the " command) must be closed with the ’ command. |
| <nobr>?MAT</nobr> | <nobr>No matching files</nobr> | No match was found for the file specification for an EN command. |
| <nobr>?MEM</nobr> | <nobr>Memory overflow</nobr> | Insufficient memory available to complete the current command. Make sure the Q-register area does not contain much unnecessary text. Breaking up the text area into multiple pages might be useful. See (TBD) |
| <nobr>?MLP</nobr> | <nobr>Missing left parenthesis</nobr> | There is a right parenthesis that is not matched by a corresponding left parenthesis. |
| <nobr>?MRA</nobr> | <nobr>Missing right angle bracket</nobr> | There is a left angle bracket that has no matching right angle bracket. An iteration must be complete within the macro or command. |
| <nobr>?MRP</nobr> | <nobr>Missing right parenthesis</nobr> | There is a right parenthesis that is not matched by a corresponding left parenthesis. |
| <nobr>?MSC</nobr> | <nobr>Missing start of conditional</nobr> | A ’ command (end of conditional) was encountered. Every ’ command must be matched by a preceding " (start of conditional) command. |
| <nobr>?NAB</nobr> | <nobr>No arg before ^_</nobr> | The ^_ command must be preceded by either a specific numeric argument or a command that returns a numeric value. |
| <nobr>?NAC</nobr> | <nobr>No arg before ,</nobr> | A command has been executed in which a , is not preceded by a numeric argument. |
| <nobr>?NAE</nobr> | <nobr>No arg before =</nobr> | The =, = =, or = == command must be preceded by either a specific numeric argument or a command that returns a numeric value. |
| <nobr>?NAP</nobr> | <nobr>No arg before )</nobr> | A right parenthesis has been encountered and is not properly preceded by a specific numeric argument or a command that returns a numeric value. |
| <nobr>?NAQ</nobr> | <nobr>No arg before "</nobr> | The " commands must be preceded by a single numeric argument on which the decision to execute the following commands or skip to the matching ’ is based. |
| <nobr>?NAS</nobr> | <nobr>No arg before ;</nobr> | The ; command must be preceded by a single numeric argument on which the decision to execute the following commands or skip to the matching \> is based. |
| <nobr>?NAT</nobr> | <nobr>Cannot have n argument and text string</nobr> | Both an n argument and a string were specified for a command which does not allow more than one. |
| <nobr>?NAU</nobr> | <nobr>No arg before U</nobr> | The U command must be preceded by either a specific numeric argument or a command that returns a numeric value. |
| <nobr>?NCA</nobr> | <nobr>Negative argument to ,</nobr> | A comma was preceded by a negative number. |
| <nobr>?NFI</nobr> | <nobr>No file for input</nobr> | Before issuing an input command, such as Y, it is necessary to open an input file by use of a command such as ER or EB. |
| <nobr>?NFO</nobr> | <nobr>No file for output</nobr> | Before issuing an output command such as N search or P it is necessary to open an output file by use of a command such as EW or EB. |
| <nobr>?NOD</nobr> | <nobr>Display mode support not enabled</nobr> | It was not possible to enable display mode, because TECO was not compiled to include support for it. |
| <nobr>?NON</nobr> | <nobr>No n argument after m argument</nobr> | An m argument was not followed by an n argument. |
| <nobr>?NOT</nobr> | <nobr>O command has no tag</nobr> | No tag was found for an O command. |
| <nobr>?NPA</nobr> | <nobr>Negative or 0 argument to P</nobr> | A P command was preceded by a negative or 0 argument. |
| <nobr>?NYA</nobr> | <nobr>Numeric argument with Y</nobr> | The Y command must not be preceded by either a numeric argument or a command that returns a numeric value. |
| <nobr>?NYI</nobr> | <nobr>Not yet implemented</nobr> | A command was issued which tries to invoke a feature not available on this implementation of TECO. |
| <nobr>?OFO</nobr> | <nobr>Output file already open</nobr> | A command has been executed which tried to create an output file, but an output file currently is open. It is typically appropriate to use the EC or EK command as the situation calls for to close the output file. |
| <nobr>?PDO</nobr> | <nobr>Push-down list overflow</nobr> | The command string has become too complex. Simplify it. |
| <nobr>?PES</nobr> | <nobr>Attempt to Pop Empty Stack</nobr> | A ] command (pop off q-register stack into a q-register) was encountered when there was nothing on the q-register stack. |
| <nobr>?POP</nobr> | <nobr>Attempt to move pointer off page with 'x'</nobr> | A J, C or R command has been executed which attempted to move the pointer off the page. The result of executing one of these commands must leave the pointer between 0 and Z, inclusive. The characters referenced by a D or m,nX command must also be within the buffer boundary. |
| <nobr>?SNI</nobr> | <nobr>; not in iteration</nobr> | A ; command has been executed outside of an open iteration bracket. This command may only be executed within iteration brackets. |
| <nobr>?SRH</nobr> | <nobr>Search failure 'text'</nobr> | A search command not preceded by a colon modifier and not within an iteration has failed to find the specified "text". After an S search fails the pointer is left at the beginning of the buffer. After an N or _ search fails the last page of the input file has been input and, in the case of N, output, and the buffer is cleared. In the case of an N search it is usually necessary to close the output file and reopen it for continued editing. |
| <nobr>?SYS</nobr> | <nobr>System error message</nobr> | A system call failed. The error message text explains the error. |
| <nobr>?TAG</nobr> | <nobr>Missing tag '!tag!'</nobr> | The specified tag for an O command cannot be found. This tag must be in the same macro level as the O command referencing it. |
| <nobr>?UTC</nobr> | <nobr>Unterminated command 'x'</nobr> | This is a general error which is usually caused by an unterminated insert, search, or filespec argument, an unterminated ^A message, an unterminated tag or comment (i.e., unterminated ! construct), or a missing ’ character which closes a conditional execution command. |
| <nobr>?UTM</nobr> | <nobr>Unterminated macro</nobr> | This error is the same as the ?UTC error except that the unterminated command was executing from a Q-register (i.e., it was a macro). (Note: An entire command sequence stored in a Q-register must be complete within the Q-register.) |
| <nobr>?XAB</nobr> | <nobr>Execution aborted</nobr> | Execution of TECO was aborted. This is usually due to the typing of \<CTRL/C\>. |
| <nobr>?YCA</nobr> | <nobr>Y command aborted</nobr> | An attempt has been made to execute an Y or _ search command with an output file open, that would cause text in the edit buffer to be erased without outputting it to the output file. The ED command controls this check. |
