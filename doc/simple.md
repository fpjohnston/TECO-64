## TECO-64 - Simple Commands

### File Selection Commands

Input and output files may be specified to TECO in several ways. The following
sections present first a simple method for specifying files, and then more
sophisticated commands that permit flexible file selection.

- Note: All of the following file selection commands are shown with a general
argument of *filespec*. The actual contents of this file specification argument
are operating system dependent.

### Simplified File Selection

For most simple applications, you can use special operating system commands to
specify the name of the file you wish to edit at the same time that you start up
TECO.

To create a new file:

teco -C *filespec*

This command starts up TECO and creates the specified file for output.

To edit an existing file:

teco *filespec*

This command starts up TECO and opens the specified file for editing while
preserving the original file (as a backup file). It also automatically
brings the first page of the file into the edit buffer. These functions
simulate the EB command.

If any of the above commands do not seem to work on your operating system,
consult the appropriate appendix for information about how to install TECO and
its associated operating system commands.

### Input File Specification (ER command)

TECO will accept input text from any input device in the operating system. The
input device may be specified by the text string supplied in the ER command (and
which, like any text argument, is terminated by a \<*delim*\> character). The ER
command causes TECO to open the specified file or print an error message if the
file is not found. This command does not cause any portion of the file to be read
into the edit buffer, however. The following examples illustrate use of the ER
command.

| Command | Function |
| ------- | -------- |
| ER*filespec*\` | General form of the ER command where *filespec* is the designation of the input file. The command is terminated by a \<*delim*\ > character. |
| ERhello.c\` | Prepare to read input file hello.c from the current directory. |

TECO will only keep one input and one output file open and selected at a time.
The current input file may be changed by simply using the ER command to
specify a new file.

It is not always necessary to specify an input file. If you want to create a file
without using any previously edited text as input, you may type commands to
insert the necessary text directly into the edit buffer from the keyboard and, at
the end of each page, write the contents of the buffer onto an output file. Since
all input is supplied from the keyboard, no input file is necessary.

### Output File Specification (EW command)

TECO will write output text onto any output device in the operating system.
The output file may be specified by means of the text string supplied in an EW
command. If the output device is a file-structured device (for example, a disk),
the file name and any extension must be supplied. If a file name is specified
but no device is explicitly specified, the system’s default device is assumed. The
following examples illustrate use of the EW command.

| Command | Function |
| ------- | -------- |
| EW*filespec*\` | General form of the EW command where *filespec* is the designation of the input file. The command is terminated by a \<*delim*\> character. |
| EWhello.c\` | Prepare to write output file hello.c to the default directory. |
| ERinput.c\` EWoutput.c`` | Open an input file input.c and open  an output file named output.c. The double <*delim*> (echoed as ``) terminates the command string and causes the string to be executed. Note that the \<*delim*\> which terminates the EW command may be one of the two \<*delim*\>s which terminates the command string.

You do not need to specify an output file if you only want to examine an input
file, without making permanent changes or corrections. In this case, the contents
of the input file may be read into the edit buffer page by page and examined at
the terminal. Since all output is printed on the user terminal, no output file is
needed.

### Closing Files (EX command)

When you are finished editing a file, use the EX command to close out the file
and exit from TECO. The current contents of the edit buffer and any portion of
the input file that has not been read yet are copied to the output file before TECO
exits. The EX command takes no arguments.

| Command | Function |
| ------- | -------- |
| EX\`\` |  Write the edit buffer to the current output file, move the remainder of the current input file to the current output file, close the output file, then return to the operating system. |
| ERinput.c\` EWoutput.c\` EX\`\` | Open an input file input.c and open an output file named output.c, then copy all the text in the input file to the output file, close the output file, and exit from TECO. |

### Input and Output Commands

The following commands permit pages of text to be read into the TECO text
buffer from an input file or written from the buffer onto an output file. Once a
page of text has been written onto the output file, it cannot be recalled into the
edit buffer unless the output file is closed and reopened as an input file.

| Command | Function |
| ------- | -------- |
| Y | Clear the edit buffer, then read the next page of the input file into the buffer. Since the Y command causes the contents of the edit buffer to be lost, it is not permitted if an output file is open and there is text in the buffer. |
| P | Write the contents of the edit buffer onto the next page of the output file, then clear the buffer and read the next page of the input file into the buffer. |
| *n*P | Execute the P command *n* times. If *n* is not specified, a value of 1 is assumed. |

After each Y, P, or nP command, TECO positions the pointer before the first
character in the buffer.

### Pointer Positioning Commands

The buffer pointer provides the means of specifying the location within a block of
text at which insertions, deletions or corrections are to be made. The following
commands permit the buffer pointer to be moved to a position between any two
adjacent characters in the buffer.

| Command | Function |
| ------- | -------- |
| J | Move the pointer to the beginning of the buffer. |
| L | Move the pointer forward to a position between the next line feed and the first character of the next line. That is, advance the pointer to the beginning of the next line. |
| *n*L | Execute the L command *n* times, where *n* is a signed integer. A positive value of *n* moves the pointer to the beginning of the *n*th line following the current pointer position. A negative value moves the pointer backward *n* lines and positions it at the beginnning of the *n*th line preceding the current position. If *n* is zero, the pointer is moved to the beginning of the line on which it is currently positioned. |
| C | Advance the pointer forward across one character. |
| *n*C | Execute the C command *n* times, where *n* is a signed integer. A positive value of *n* moves the pointer forward across *n* characters. A negative value of *n* moves the pointer backward across *n* characters. If *n* is zero, the pointer position is not changed. Remember that there are two characters, \<CR\> and \<LF\>, at the end of each line in the buffer. |

These commands may be used to move the buffer pointer across any number of
lines or characters in either direction; however, they will not move the pointer
across a page boundary. If a C command attempts to move the pointer backward
beyond the beginning of the buffer or forward past the end of the buffer, an error
message is printed and the command is ignored.

If an L command attempts to exceed the page boundaries in this manner, the
pointer is positioned at the boundary which would have been exceeded. Thus, in
a page of 2000 lines, the command "–4000L" would position the pointer before the
first character in the buffer. The command "4000L" would position the pointer
after the last character in the buffer. No error message is printed in either case.

### Type-Out Commands

The following commands permit sections of the text in the buffer to be printed
out on your terminal for examination. These commands do not move the buffer
pointer.

| Command | Function |
| ------- | -------- |
| T | Type the contents of the edit buffer from the current position of the pointer through and including the next line feed character. 
| *nT | Type *n* lines, where *n* is a signed integer. A positive value of *n* causes the *n* lines following the pointer to be typed. A negative value of *n* causes the *n* lines preceding the pointer to be typed. If *n* is zero, the contents of the buffer from the beginning of the line on which the pointer is located up to the pointer is typed. This is useful for verifying the location of the buffer pointer. |
| HT | Type the entire contents of the edit buffer. |
| V | Type the current line. Equivalent to the sequence "0TT". |

### Text Modification Commands

You can insert or delete text from the buffer using the following commands:

| Command | Function |
| ------- | -------- |
| I*text*` | Where *text* is a string of ASCII characters terminated by a \<*delim*\> character. The specified text is inserted into the buffer at the current position of the pointer. The pointer is positioned immediately after the last character of the insertion.
| K | Delete the contents of the edit buffer from the current position of the pointer up to and including the next line feed character. |
| *n*K | Execute the K command *n* times, where *n* is a signed integer. A positive value of *n* causes the *n* lines following the pointer to be deleted. A negative value of *n* causes the *n* lines preceding the pointer to be deleted. If *n* is zero, the contents of the buffer from the beginning of the line on which the pointer is located up to the pointer is deleted. |
| HK | Delete the entire contents of the edit buffer. |
| D | Delete the character following the buffer pointer. |
| *n*D | Execute the D command *n* times, where *n* is a signed integer. A positive value of *n* causes the *n* characters following the pointer to be deleted. A negative value of *n* causes the *n* characters preceding the pointer to be deleted. If *n* is zero, the command is ignored. |

Like the L and C commands, the K and D commands may not execute across
page boundaries. If a K command attempts to delete text up to and across the
beginning or end of the buffer, text will be deleted only up to the buffer boundary
and the pointer will be positioned at the boundary. No error message is printed.
A D command attempting to delete text past the end or beginning of the text
buffer will produce an error message and the command will be ignored.

### Search Commands

The following commands may be used to search the input file for a specified
string of characters.

Command Function

| Command | Function |
| ------- | -------- |
| S*text*` | Where *text* is a string of ASCII characters terminated by a \<*delim*\> character. This command searches the edit buffer for the next occurrence of the specified character string following the current pointer position. If the string is found, the pointer is positioned after the last character on the string. If it is not found, the pointer is positioned immediately before the first character in the buffer and an error message is printed. |
| N*text*` | Performs the same function as the S command except that the search is continued across page boundaries, if necessary, until the character string is found or the end of the input file is reached. If the end of the input file is reached, an error message is printed. If virtual paging support has been enabled, you can "back up" into the file and continue editing, although doing so uses TECO features beyond those introduced here. Using only basic features, you must close the output file and reopen it as an input file before you can edit the file further. |

Both the S command and the N command begin searching for the specified
character string at the current position of the pointer. Therefore, neither
command will locate any occurrence of the character string which precedes the
current pointer position, nor will it locate any character string which is split
across a page boundary.

Both commands execute the search by attempting to match the command
argument, character for character, with some portion of the buffer contents. If
an N command reaches the end of the buffer without finding a match for its
argument, it writes the contents of the buffer onto the output file, clears the
buffer, reads the next page of the input file into the buffer, and continues the
search.
