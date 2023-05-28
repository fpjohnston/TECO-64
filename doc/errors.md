## TECO-64 - Error Messages

<style>
    span {
        white-space: nowrap;
    }
</style>

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
any non-printing special characters as described [here](conventions.md).

TECO can also print warning messages. These do not abort the
command being executed.

| Message | Description |
| ------- | ----------- |
| <span>\%Superseding existing file</span> | The file specified to an EW command already exists, and the old copy will be deleted if the file is closed with an EC or EX command. The EK command may be used to undo the EW command. |
| <span>\%Search failure in loop</span> | A search command has failed inside a loop. A ; (semi-colon) command immediately following the search command can be used to suppress this message. After printing the message, the iteration is terminated, i.e., TECO simulates an F> command. |

The following error messages are listed alphabetically by their three-letter
code.

| Error | Message            | Description |
| ----- | ------------------ | ----------- |
| <span>?ARG</span> | <span>Improper arguments</span> | Three or more numeric arguments are given (e.g., 1,2,3D or H,cT). |
| <span>?ATS</span> | <span>Invalid or extraneous at-sign</span> | At sign not allowed for command, or more than one at sign specified. |
| <span>?BAT</span> | <span>Bad tag '!foo!'</span> | An O command was specified with an invalid tag. Tags may only contain graphic ASCII characters (excluding commas which are reserved for computed GOTOs). Leading and trailing spaces are ignored, but embedded spaces are not allowed. |
| <span>?BNI</span> | <span>Right angle bracket not in iteration</span> | A right angle bracket was not matched by a left angle bracket (Note: a loop in a Q-register macro must be complete within the macro.) |
| <span>?CHR</span> | <span>Invalid character for command</span> | The numeric argument to an EE command was not an ASCII character. |
| <span>?CMD</span> | <span>An EG or EZ command is too long</span> | An EG or EZ command was longer than 4095 characters. |
| <span>?COL</span> | <span>Invalid or extraneous colon</span> | A colon preceded a command that does not allow colons, or there were too many colons specified for the command. |
| <span>?CPQ</span> | <span>Can't pop into Q-register</span> | A ] command has been executed and there is nothing saved on the Q-register push down list. |
| <span>?DET</span> | <span>(error message)</span> | An error occurred when attempting to detach TECO from the user's terminal with a 0,64 ET command (or equivalent command), or if the detach feature is not supported in the current operating environment. |
| <span>?DIV</span> | <span>Division by zero</span> | An expression tried to divide a number by zero. |
| <span>?DPY</span> | <span>Display mode error</span> | Display mode support is either missing or disabled. |
| <span>?DTB</span> | <span>Delete too big</span> | A D command attempted to delete text outside the current page. |
| <span>?DUP</span> | <span>Duplicate tag '!foo!'</span> | An O command found a duplicate tag within the command string. |
| <span>?ERR</span> | <span>(error message)</span> | A system call failed. The error message text explains the error. |
| <span>?EXT</span> | <span>Extended feature not enabled</span> | A command attempted to use an extended feature which is not currently enabled. |
| <span>?FIL</span> | <span>Invalid file 'foo'</span> | An attempt was made to open a directory, FIFO, socket, or similar file specification instead of a regular file. |
| <span>?FNF</span> | <span>File not found 'foo'</span> | The requested input file could not be located. If this occurred within a macro, the colon-modified command may be necessary. |
| <span>?IAA</span> | <span>Invalid A argument</span> | The argument preceding a :A command is negative or zero. |
| <span>?ICE</span> | <span>Invalid ^E command in search argument</span> | A search argument contains a ^E command that is either not defined or incomplete. |
| <span>?IEC</span> | <span>Invalid character '*x*' after E</span> | An invalid E command has been executed. |
| <span>?IFC</span> | <span>Invalid character '*x*' after F</span> | An invalid F command has been executed. |
| <span>?IFE</span> | <span>Ill-formed numeric expression</span> | The numeric expression preceding a command doesn't make sense. For example, 5+ isn't a complete expression. |
| <span>?IFN</span> | <span>Invalid character '*x*' in filename</span> | A file name contained one or more invalid characters. Check your operating system documentation to determine what characters are valid for file names. |
| <span>?IIA</span> | <span>Invalid insert argument</span> | A command of the form "nItext`" was attempted. This combination of character and text insertion is not allowed. |
| <span>?ILL</span> | <span>Invalid command '*x*'</span> | An attempt has been made to execute an invalid TECO command. |
| <span>?ILN</span> | <span>Invalid number</span> | An 8 or 9 was used in a digit string for an octal number, or a 0x or 0X prefix was not followed by a hexadecimal number. |
| <span>?IMA</span> | <span>Invalid m argument</span> | An m argument was provided to a command which does not allow it. |
| <span>?INA</span> | <span>Invalid n argument</span> | An n argument was provided to a command which does not allow it. |
| <span>?IPA</span> | <span>Invalid P argument</span> | The argument preceding a P or PW command is negative or zero. |
| <span>?IQC</span> | <span>Invalid quote character</span> | One of the valid " qualifiers did not follow the quote character. |
| <span>?IQN</span> | <span>Invalid Q-register name '*x*'</span> | An invalid Q-register name was specified in one of the Q-register commands. |
| <span>?IRA</span> | <span>Invalid radix argument to ^R</span> | The argument to a ^R radix command must be 8, 10 or 16. |
| <span>?ISA</span> | <span>Invalid search argument</span> | The argument preceding a search command is 0. This argument must not be 0. |
| <span>?ISS</span> | <span>Invalid search string</span> | One of the search string special characters (^Q, ^V, ^W, etc.) would have modified the search string delimiter (usually <ESC>). |
| <span>?IUC</span> | <span>Invalid character '*x*' following ^</span> | The character following a ^ must have an ASCII value between 64 and 95 inclusive, or between 141 and 172 inclusive. |
| <span>?KEY</span> | <span>Keyword 'foo' not found</span> | An invalid keyword was specified for an F1, F2, F3, F4, FM, or FQ command. |
| <span>?LOC</span> | <span>Invalid location for tag '!foo!'</span> | An O command inside a loop or nested loop cannot jump backward before the start of the outermost loop. It also cannot jump into the middle of any loop it is not currently inside of. |
| <span>?MAP</span> | <span>Missing apostrophe</span> | Every conditional (started with the " command) must be closed with the ' command. |
| <span>?MAX</span> | <span>Internal program limit reached</span> | Loops and conditionals are limited to a maximum of 32 levels, macros are limited to a depth of 64 levels, and file names are limited to 4095 characters. |
| <span>?MEM</span> | <span>Memory overflow</span> | Insufficient memory available to complete the current command. |
| <span>?MLP</span> | <span>Missing left parenthesis</span> | There is a right parenthesis trhat is not matched by a corresponding left parenthesis. |
| <span>?MQN</span> | <span>Missing Q-register name</span> | A command or match control construct did not include a required Q-register name. |
| <span>?MRA</span> | <span>Missing right angle bracket</span> | There is a left angle bracket that has no matching right angle bracket. A loop must be complete within a macro or command string. |
| <span>?MRP</span> | <span>Missing right parenthesis</span> | There is a left parenthesis that is not matched by a corresponding right parenthesis. |
| <span>?MSC</span> | <span>Missing start of conditional</span> | A ' command (end of conditional) was encountered. Every ' command must be matched by a preceding " (start of a conditional) command. |
| <span>?NAB</span> | <span>No argument before 1's complement operator</span> | The ^_ command must be preceded by either a specific numeric argument or a command that returns a numeric value. |
| <span>?NAC</span> | <span>No argument before comma</span> | A command has been executed in which a , is not preceded by a numeric argument. |
| <span>?NAE</span> | <span>No argument before equals sign</span> | The =, ==, or === command must be preceded by either a specific numeric argument or a command that returns a numeric value. |
| <span>?NAP</span> | <span>No argument before right parenthesis</span> | A right parenthesis has been encountered and is not properly preceded by a specific numeric argument or a command that returns a numeric value. |
| <span>?NAQ</span> | <span>No argument before quote</span> | The " commands must be preceded by a single numeric argument on which the decision to execute the following commands or skip to the matching ' is based. |
| <span>?NAS</span> | <span>No argument before semi-colon</span> | The ; command must be preceded by a single numeric argument on which the decision to execute the following commands or skip to the matching > is based. |
| <span>?NAU</span> | <span>No argument before U command</span> | The U command must be preceded by either a specific numeric argument or a command that returns a numeric value. |
| <span>?NCA</span> | <span>Negative argument to comma</span> | A comma was preceded by a negative number. |
| <span>?NFI</span> | <span>No file for input</span> | Before issuing an input command, such as Y, it is necessary to open an input file by use of a command such as ER or EB. |
| <span>?NFO</span> | <span>No file for output</span> | Before issuing an output command, such as N or or P, it is necessary to open an output file with a command such as EW or EB. |
| <span>?NON</span> | <span>No n argument after m argument</span> | An m argument was not followed by an n argument. |
| <span>?NOT</span> | <span>O command has no tag</span> | No tag was found for an O command. |
| <span>?NPA</span> | <span>Negative or 0 argument to P</span> | A P command was preceded by a negative or 0 argument. |
| <span>?NYA</span> | <span>Numeric argument with Y</span> | The Y command must not be preceded by either a numeric argument or a command that returns a numeric value. |
| <span>?NYI</span> | <span>Not yet implemented</span> | A command was issued that is not yet implemented in this version of TECO, or not enabled in this build. |
| <span>?OFO</span> | <span>Output file already open</span> | A command has been executed which tried to create an output file, but an output file currently is open. It is typically appropriate to use the EC or EK command as the situation calls for to close the output file. |
| <span>?PDO</span> | <span>Push-down list overflow</span> | The command string has become too complex. Simplify it. |
| <span>?POP</span> | <span>Attempt to move pointer off page with '*x*'</span> | A J, C or R command has been executed which attempted to move the pointer off the page. The result of executing one of these commands must leave the pointer between 0 and Z, The characters referenced by a D or nA command must also be within the buffer limits. |
| <span>?SNI</span> | <span>Semi-colon not in iteration</span> | A ; command has been executed outside of a loop. |
| <span>?SRH</span> | <span>Search failure: 'foo'</span> | A search command not preceded by a colon modifier and not within an iteration has failed to find the specified " command. After an S search fails the pointer is left at the beginning of the buffer. After an N or _ search fails the last page of the input file has been input and, in the case of N, output, and the buffer is cleared. In the case of an N search it is usually necessary to close the output file and reopen it. |
| <span>?TAG</span> | <span>Missing tag: '!foo!'</span> | The tag specified by an O command cannot be found. This tag must be in the same macro level as the O command referencing it. |
| <span>?TXT</span> | <span>Invalid text delimiter '*x*'</span> | Text delimiters must be graphic ASCII characters in the range of [33,126], or control characters in the range of [1,26]. Characters such as spaces or ESCapes may not be used for delimiters. |
| <span>?UTC</span> | <span>Unterminated command string</span> | This is a general error which is usually caused by an unterminated insert, search, or filename argument, an unterminated ^A message, an unterminated tag or comment (i.e., unterminated ! construct), or a missing ' character which closes a conditional execution command. |
| <span>?UTM</span> | <span>Unterminated macro</span> | This error is that same as the ?UTC error except that the unterminated command was executing from a Q-register (i.e., it was a macro). (Note: An entire command sequence stored in a Q-register must be complete within the Q-register.) |
| <span>?XAB</span> | <span>Execution aborted</span> | Execution of TECO was aborted. This is usually due to the typing of <CTRL/C>. |
| <span>?YCA</span> | <span>Y command aborted</span> | An attempt has been made to execute a Y or _ search command with an output file open, that would cause text in the edit buffer to be erased without outputting it to the output file. The ED command controls this check. |


