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
any non-printing special characters as described [here](conventions.md).

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
| <nobr>?ARG</nobr> | <nobr>Improper arguments</nobr> | Three or more numeric arguments are given (e.g., 1,2,3D or H,cT). |
| <nobr>?ATS</nobr> | <nobr>Invalid or extraneous at-sign</nobr> | At sign not allowed for command, or more than one at sign specififed. |
| <nobr>?BAT</nobr> | <nobr>Bad tag '!foo!'</nobr> | An O command was specified with an invalid tag. Tags may only contain graphic ASCII characters (excluding commas which are reserved for computed GOTOs). Leading and trailing spaces are ignored, but embedded spaces are not allowed. |
| <nobr>?BNI</nobr> | <nobr>Right angle bracket not in iteration</nobr> | A right angle bracket was not matched by a left angle bracket (Note: a loop in a Q-register macro must be complete within the macro.) |
| <nobr>?CFG</nobr> | <nobr>Command not configured</nobr> | A command was input that has not been configured. Modify makefile to include this command, and then rebuild TECO. |
| <nobr>?CHR</nobr> | <nobr>Invalid character for command</nobr> | The numeric argument to an EE command was not an ASCII character. |
| <nobr>?CMD</nobr> | <nobr>An EG or EZ command is too long</nobr> | An EG or EZ command was longer than 4095 characters. |
| <nobr>?COL</nobr> | <nobr>Invalid or extraneous colon</nobr> | A colon preceded a command that does not allow colons, or there were too many colons specified for the command. |
| <nobr>?CPQ</nobr> | <nobr>Can't pop into Q-register</nobr> | A ] command has been executed and there is nothing saved on the Q-register push down list. |
| <nobr>?DIV</nobr> | <nobr>Division by zero</nobr> | An expression tried to divide a number by zero. |
| <nobr>?DPY</nobr> | <nobr>Display mode error</nobr> | Display mode support is either missing or disabled. |
| <nobr>?DTB</nobr> | <nobr>Delete too big</nobr> | A D command attempted to delete text outside the current page. |
| <nobr>?DUP</nobr> | <nobr>Duplicate tag '!foo!'</nobr> | An O command found a duplicate tag within the command string. |
| <nobr>?EIE</nobr> | <nobr>EI command error</nobr> | An invalid EI command has been executed. The user had too many nested EI commands, or tried to execute an old-style EI command while a new-style EI command was being executed (or vice versa). |
| <nobr>?ERR</nobr> | <nobr>(error message)</nobr> | A system call failed. The error message text explains the error. |
| <nobr>?EXT</nobr> | <nobr>Extended feature not enabled</nobr> | A command attempted to use an extended feature which is not currently enabled. |
| <nobr>?FIL</nobr> | <nobr>Invalid file 'foo'</nobr> | An attempt was made to open a directory, FIFO, socket, or similar file specification instead of a regular file. |
| <nobr>?FNF</nobr> | <nobr>File not found 'foo'</nobr> | The requested input file could not be located. If this occurred within a macro, the colon-modified command may be necessary. |
| <nobr>?IAA</nobr> | <nobr>Invalid A argument</nobr> | The argument preceding a :A command is negative or zero. |
| <nobr>?ICE</nobr> | <nobr>Invalid ^E command in search argument</nobr> | A search argument contains a ^E command that is either not defined or incomplete. |
| <nobr>?IEC</nobr> | <nobr>Invalid character '*x*' after E</nobr> | An invalid E command has been executed. |
| <nobr>?IFC</nobr> | <nobr>Invalid character '*x*' after F</nobr> | An invalid F command has been executed. |
| <nobr>?IFE</nobr> | <nobr>Ill-formed numeric expression</nobr> | The numeric expression preceding a command doesn't make sense. For example, 5+ isn't a complete expression. |
| <nobr>?IFN</nobr> | <nobr>Invalid character '*x*' in filename</nobr> | A file name contained one or more invalid characters. Check your operating system documentation to determine what characters are valid for file names. |
| <nobr>?IIA</nobr> | <nobr>Invalid insert argument</nobr> | A command of the form "nItext`" was attempted. This combination of character and text insertion is not allowed. |
| <nobr>?ILL</nobr> | <nobr>Invalid command '*x*'</nobr> | An attempt has been made to execute an invalid TECO command. |
| <nobr>?ILN</nobr> | <nobr>Invalid number</nobr> | An 8 or 9 was used in a digit string for an octal number, or a 0x or 0X prefix was not followed by a hexadecimal number. |
| <nobr>?IMA</nobr> | <nobr>Invalid m argument</nobr> | An m argument was provided to a command which does not allow it. |
| <nobr>?INA</nobr> | <nobr>Invalid n argument</nobr> | An n argument was provided to a command which does not allow it. |
| <nobr>?IPA</nobr> | <nobr>Invalid P argument</nobr> | The argument preceding a P or PW command is negative or zero. |
| <nobr>?IQC</nobr> | <nobr>Invalid quote character</nobr> | One of the valid " qualifiers did not follow the quote character. |
| <nobr>?IQN</nobr> | <nobr>Invalid Q-register name '*x*'</nobr> | An invalid Q-register name was specified in one of the Q-register commands. |
| <nobr>?IRA</nobr> | <nobr>Invalid radix argument to ^R</nobr> | The argument to a ^R radix command must be 8, 10 or 16. |
| <nobr>?ISA</nobr> | <nobr>Invalid search argument</nobr> | The argument preceding a search command is 0. This argument must not be 0. |
| <nobr>?ISS</nobr> | <nobr>Invalid search string</nobr> | One of the search string special characters (^Q, ^V, ^W, etc.) would have modified the search string delimiter (usually <ESC>). |
| <nobr>?IUC</nobr> | <nobr>Invalid character '*x*' following ^</nobr> | The character following a ^ must have an ASCII value between 64 and 95 inclusive, or between 141 and 172 inclusive. |
| <nobr>?KEY</nobr> | <nobr>Key 'foo' not found</nobr> | An invalid key was specified for an FM command. |
| <nobr>?LOC</nobr> | <nobr>Invalid location for tag '!foo!'</nobr> | An O command cannot jump to a location inside a loop, other than the one the O command may be in, or inside a conditional. |
| <nobr>?MAP</nobr> | <nobr>Missing apostrophe</nobr> | Every conditional (started with the " command) must be closed with the ' command. |
| <nobr>?MAX</nobr> | <nobr>Internal program limit reached</nobr> | Loops and conditionals are limited to a maximum of 32 levels, macros are limited to a depth of 64 levels, and file names are limited to 4095 characters. |
| <nobr>?MEM</nobr> | <nobr>Memory overflow</nobr> | Insufficient memory available to complete the current command. |
| <nobr>?MLP</nobr> | <nobr>Missing left parenthesis</nobr> | There is a right parenthesis trhat is not matched by a corresponding left parenthesis. |
| <nobr>?MQN</nobr> | <nobr>Missing Q-register name</nobr> | A command or match control construct did not include a required Q-register name. |
| <nobr>?MRA</nobr> | <nobr>Missing right angle bracket</nobr> | There is a left angle bracket that has no matching right angle bracket. A loop must be complete within a macro or command string. |
| <nobr>?MRP</nobr> | <nobr>Missing right parenthesis</nobr> | There is a left parenthesis that is not matched by a corresponding right parenthesis. |
| <nobr>?MSC</nobr> | <nobr>Missing start of conditional</nobr> | A ' command (end of conditional) was encountered. Every ' command must be matched by a preceding " (start of a conditional) command. |
| <nobr>?NAB</nobr> | <nobr>No argument before 1's complement operator</nobr> | The ^_ command must be preceded by either a specific numeric argument or a command that returns a numeric value. |
| <nobr>?NAC</nobr> | <nobr>No argument before comma</nobr> | A command has been executed in which a , is not preceded by a numeric argument. |
| <nobr>?NAE</nobr> | <nobr>No argument before equals sign</nobr> | The =, ==, or === command must be preceded by either a specific numeric argument or a command that returns a numeric value. |
| <nobr>?NAP</nobr> | <nobr>No argument before right parenthesis</nobr> | A right parenthesis has been encountered and is not properly preceded by a specific numeric argument or a command that returns a numeric value. |
| <nobr>?NAQ</nobr> | <nobr>No argument before quote</nobr> | The " commands must be preceded by a single numeric argument on which the decision to execute the following commands or skip to the matching ' is based. |
| <nobr>?NAS</nobr> | <nobr>No argument before semi-colon</nobr> | The ; command must be preceded by a single numeric argument on which the decision to execute the following commands or skip to the matching > is based. |
| <nobr>?NAT</nobr> | <nobr>Cannot have n argument and text string</nobr> | Both an n argument and a string were specified for a command which does not allow more than one. |
| <nobr>?NAU</nobr> | <nobr>No argument before U command</nobr> | The U comand must be preceded by either a specific numeric argument or a command that returns a numeric value. |
| <nobr>?NCA</nobr> | <nobr>Negative argument to comma</nobr> | A comma was preceded by a negative number. |
| <nobr>?NFI</nobr> | <nobr>No file for input</nobr> | Before issuing an input command, such as Y, it is necessary to open an input file by use of a command such as ER or EB. |
| <nobr>?NFO</nobr> | <nobr>No file for output</nobr> | Before issuing an output command, such as N or or P, it is necessary to open an output file with a command such as EW or EB. |
| <nobr>?NON</nobr> | <nobr>No n argument after m argument</nobr> | An m argument was not followed by an n argument. |
| <nobr>?NOT</nobr> | <nobr>O command has no tag</nobr> | No tag was found for an O command. |
| <nobr>?NPA</nobr> | <nobr>Negative or 0 argument to P</nobr> | A P command was preceded by a negative or 0 argument. |
| <nobr>?NYA</nobr> | <nobr>Numeric argument with Y</nobr> | The Y command must not be preceded by either a numeric argument or a command that returns a numeric value. |
| <nobr>?NYI</nobr> | <nobr>Not yet implemented</nobr> | A command was issued that is not yet implemented in this version of TECO. |
| <nobr>?OFO</nobr> | <nobr>Output file already open</nobr> | A command has been executed which tried to create an output file, but an output file currently is open. It is typically appropriate to use the EC or EK command as the situation calls for to close the output file. |
| <nobr>?PDO</nobr> | <nobr>Push-down list overflow</nobr> | The command string has become too complex. Simplify it. |
| <nobr>?POP</nobr> | <nobr>Attempt to move pointer off page with '*x*'</nobr> | A J, C or R command has been executed which attempted to move the pointer off the page. The result of executing one of these commands must leave the pointer between 0 and Z, The characters referenced by a D or nA command must also be within the buffer limits. |
| <nobr>?SNI</nobr> | <nobr>Semi-colon not in iteration</nobr> | A ; command has been executed outside of a loop. |
| <nobr>?SRH</nobr> | <nobr>Search failure: 'foo'</nobr> | A search command not preceded by a colon modifier and not within an iteration has failed to find the specified " command. After an S search fails the pointer is left at the beginning of the buffer. After an N or _ search fails the last page of the input file has been input and, in the case of N, output, and the buffer is cleared. In the case of an N search it is usually necessary to close the output file and reopen it. |
| <nobr>?TAG</nobr> | <nobr>Missing tag: '!foo!'</nobr> | The tag specified by an O command cannot be found. This tag must be in the same macro level as the O command referencing it. |
| <nobr>?TXT</nobr> | <nobr>Invalid text delimiter '*x*'</nobr> | Text delimiters must be graphic ASCII characters in the range of [33,126], or control characters in the range of [1,26]. Characters such as spaces or ESCapes may not be used for delimiters. |
| <nobr>?UTC</nobr> | <nobr>Unterminated command string</nobr> | This is a general error which is usually caused by an unterminated insert, search, or filename argument, an unterminated ^A message, an unterminated tag or comment (i.e., unterminated ! construct), or a missing ' character which closes a conditional execution command. |
| <nobr>?UTM</nobr> | <nobr>Unterminated macro</nobr> | This error is that same as the ?UTC error except that the unterminated command was executing from a Q-register (i.e., it was a macro). (Note: An entire command sequence stored in a Q-register must be complete within the Q-register.) |
| <nobr>?XAB</nobr> | <nobr>Execution aborted</nobr> | Execution of TECO was aborted. This is usually due to the typing of <CTRL/C>. |
| <nobr>?YCA</nobr> | <nobr>Y command aborted</nobr> | An attempt has been made to execute a Y or _ search command with an output file open, that would cause text in the edit buffer to be erased without outputting it to the output file. The ED command controls this check. |


