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

TECO can also print warning messages. These messages do not abort the
command being executed.

| Error message | Description |
| ------------- | ----------- |
| \%Superseding existing file | Indicates that the file to be created as the result of an EW command already exists. If the output file is closed the old copy of the file will be deleted. The EK command may be used to reverse the EW command. |
| \%Search fail in iter | Indicates that a search command has failed inside iteration brackets. A ; (semi-colon) command immediately following the search command can typically be used to suppress this message. After printing the message, the iteration is terminated, i.e., TECO simulates a 0; command. |

The following error messages are listed alphabetically by their three-letter
code.

| Error code | Message            | Explanation |
| ---------- | ------------------ | ----------- |
| ARG        | Improper Arguments | Three or more numeric arguments were specified (e.g., a,b,c or H,c). |
| ATS        | Illegal at sign, or too many at signs | An at sign preceded a command that does not allow at signs, or there were too many at signs specified for the command." |
| BNI        | \> not in iteration | There is a close angle bracket not matched by an open angle bracket somewhere to its left. (Note: an iteration in a macro stored in a Q-register must be complete within the Q-register.) |
| BOA        | O argument is out of range | The argument for an O command was out of range. |
| CHR        | Invalid character for command | A non-ASCII character preceded an EE command. |
| COL        | Illegal colon, or too many colons | A colon preceded a command that does not allow colons, or there were too many colons specified for the command. |
| DIV        | Division by zero | An attempt was made to divide a number by zero. |
| DPY        | Display mode initialization error | An error occurred during initialization of display mode. |
| DTB        | Delete too big | An nD command has been attempted which is not contained within the current page. |
| DUP        | Duplicate tag "!x!" | An O command found a duplicate tag within the command string. |
| EGC        | EG command is too long | An EG command was longer than 200 characters. |
| FIL        | Illegal file "x" | An attempt was made to open a directory, FIFO, socket, or similar file specification instead of a regular file. |
| FNF        | File not found "filespec" | The requested input file could not be located. If this occurred within a macro the colon modified ER or EB command may be necessary. |
| ICE        | Illegal ^E command in search argument | A search argument contains a ^E command that is either not defined or incomplete. The only valid ^E commands in search arguments are: ^EA, ^ED, ^EV, ^EW, ^EL, ^ES, ^E<NNN>, and ^E[A,B,C,...]. |
| IE1        | Illegal E1 command | An invalid E1 command has been executed. The E1&32 bit may not be set while executing an old-style EI command, and may not be reset while executing a new-style EI command. |
| IEC        | Illegal character "x" after E | An invalid E command has been executed. The E character must be followed by an alphabetic to form a legal E command (i.e., ER or EX). |
| IFC        | Illegal character "x" after F | An invalid F command has been executed. |
| IFE        | Ill-formed numeric expression | The numeric expression preceding a command doesn't make sense. For example, 5+ isn't a complete expression. |
| IFN        | Illegal character "x" in filename | The filespec as an argument to one of the E commands is unacceptable to the system. The file specification must be appropriate to the system in use. |
| IIA        | Illegal insert arg | A command of the form "nItext`" was attempted. Combining character (nI`) and text (Itext`) insertions is illegal. |
| ILL        | Illegal command "x" | An attempt has been made to execute an invalid TECO command. |
| ILN        | Illegal number | An 8 or 9 has been entered when the radix of TECO is set to octal. |
| IMA        | Illegal m argument | An m argument was provided to a command which does not allow it. |
| INA        | Illegal n argument | An n argument was provided to a command which does not allow it. |
| INI        | Initialization error | A fatal error occurred during TECO initialization. |
| IOA        | Illegal O argument | The argument for an O command was <= 0. |
| IQC        | Illegal " character | One of the valid " commands did not follow the ". Refer to Section 5.14, Conditional Execution Commands, for the legal set of commands. |
| IQN        | Illegal Q-register name "x" | An illegal Q-register name was specified in one of the Q-register commands. |
| IRA        | Illegal radix argument to ^R | The argument to a ^R radix command must be 8, 10, or 16. |
| ISA        | Illegal search arg | The argument preceding a search command is 0. This argument must not be 0. |
| ISS        | Illegal search string | One of the search string special characters (^Q, ^V, ^W, etc.) would have modified the search string delimiter (usually ESCAPE). |
| IUC        | Illegal character "x" following ^ | The character following an ^ must have ASCII value between 64 and 95 inclusive, or between 96 and 122 inclusive. |
| MAP        | Missing ’ | Every conditional (opened with the " command) must be closed with the ’ command. |
| MAT        | No matching files | No match was found for the file specification for an EN command. |
| MEM        | Memory overflow | Insufficient memory available to complete the current command. Make sure the Q-register area does not contain much unnecessary text. Breaking up the text area into multiple pages might be useful. See (TBD) |
| MLP        | Missing ( | There is a right parenthesis that is not matched by a corresponding left parenthesis. |
| MRA        | Missing Right Angle Bracket | There is a left angle bracket that has no matching right angle bracket. An iteration must be complete within the macro or command. |
| MRP        | Missing ) | There is a right parenthesis that is not matched by a corresponding left parenthesis. |
| MSC        | Missing Start of Conditional | A ’ command (end of conditional) was encountered. Every ’ command must be matched by a preceding " (start of conditional) command. |
| NAB        | No arg before ^_ | The ^_ command must be preceded by either a specific numeric argument or a command that returns a numeric value. |
| NAC        | No arg before , | A command has been executed in which a , is not preceded by a numeric argument. |
| NAE        | No arg before = | The =, = =, or = == command must be preceded by either a specific numeric argument or a command that returns a numeric value. |
| NAP        | No arg before ) | A right parenthesis has been encountered and is not properly preceded by a specific numeric argument or a command that returns a numeric value. |
| NAQ        | No arg before " | The " commands must be preceded by a single numeric argument on which the decision to execute the following commands or skip to the matching ’ is based. |
| NAS        | No arg before ; | The ; command must be preceded by a single numeric argument on which the decision to execute the following commands or skip to the matching \> is based. |
| NAT        | Cannot have n argument and text string | Both an n argument and a string were specified for a command which does not allow more than one. |
| NAU        | No arg before U | The U command must be preceded by either a specific numeric argument or a command that returns a numeric value. |
| NCA        | Negative argument to , | A comma was preceded by a negative number. |
| NFI        | No file for input | Before issuing an input command, such as Y, it is necessary to open an input file by use of a command such as ER or EB. |
| NFO        | No file for output | Before issuing an output command such as N search or P it is necessary to open an output file by use of a command such as EW or EB. |
| NOD        | Display mode support not enabled | It was not possible to enable display mode, because TECO was not compiled to include support for it. |
| NON        | No n argument after m argument | An m argument was not followed by an n argument. |
| NOT        | O command has no tag | No tag was found for an O command. |
| NPA        | Negative or 0 argument to P | A P command was preceded by a negative or 0 argument. |
| NYA        | Numeric argument with Y | The Y command must not be preceded by either a numeric argument or a command that returns a numeric value. |
| NYI        | Not yet implemented | A command was issued which tries to invoke a feature not available on this implementation of TECO. |
| OFO        | Output file already open | A command has been executed which tried to create an output file, but an output file currently is open. It is typically appropriate to use the EC or EK command as the situation calls for to close the output file. |
| PDO        | Push-down list overflow | The command string has become too complex. Simplify it. |
| PES        | Attempt to Pop Empty Stack | A ] command (pop off q-register stack into a q-register) was encountered when there was nothing on the q-register stack. |
| POP        | Attempt to move pointer off page with "x" | A J, C or R command has been executed which attempted to move the pointer off the page. The result of executing one of these commands must leave the pointer between 0 and Z, inclusive. The characters referenced by a D or m,nX command must also be within the buffer boundary. |
| SNI        | ; not in iteration | A ; command has been executed outside of an open iteration bracket. This command may only be executed within iteration brackets. |
| SRH        | Search failure "text" | A search command not preceded by a colon modifier and not within an iteration has failed to find the specified "text". After an S search fails the pointer is left at the beginning of the buffer. After an N or _ search fails the last page of the input file has been input and, in the case of N, output, and the buffer is cleared. In the case of an N search it is usually necessary to close the output file and reopen it for continued editing. |
| SYS        | System error message | A system call failed. The error message text explains the error. |
| TAG        | Missing Tag !tag! | The tag !tag! specified by an O command cannot be found. This tag must be in the same macro level as the O command referencing it. |
| UTC        | Unterminated command "x" | This is a general error which is usually caused by an unterminated insert, search, or filespec argument, an unterminated ^A message, an unterminated tag or comment (i.e., unterminated ! construct), or a missing ’ character which closes a conditional execution command. |
| UTM        | Unterminated macro | This error is the same as the ?UTC error except that the unterminated command was executing from a Q-register (i.e., it was a macro). (Note: An entire command sequence stored in a Q-register must be complete within the Q-register.) |
| XAB        | Execution aborted | Execution of TECO was aborted. This is usually due to the typing of \<CTRL/C\>. |
| YCA        | Y command aborted | An attempt has been made to execute an Y or _ search command with an output file open, that would cause text in the edit buffer to be erased without outputting it to the output file. The ED command controls this check. |
