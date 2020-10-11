### TECO-64 - Basics of TECO

When invoked, TECO prints a command prompt -- usually an asterisk (\*),
unless the TECO_PROMPT environment variable is defined -- indicating
that is ready to accept one or more commands. Execution of these commands
occurs once the user enters two successive \<DELIM\> characters.

#### Command delimiters

**\<DELIM\>** refers to any character which is used as a command delimiter
in an input command string. Historically, this was the **ESC**ape
character [ASCII 27], but later versions of TECO allowed users to specify
an alternate delimiter (often **accent grave** [ASCII 96]), either with the
ET&8192 flag bit, or the EE flag.

If an alternate delimiter has been defined, then it will always be echoed
as an accent grave. And any input ESCape will be echoed as accent grave if
an alternate delimiter has been defined, and as a dollar sign ($) if not.

Since an alternate delimiter can be user-defined, and since there can
be as many as three delimiters in effect at a given time, \<DELIM\> is
used throughout this documentation to refer to any character that may
be in use as a command delimiter. Also, whenever an example command is
shown, accent grave (\`) is used to refer to any command delimiter that
may be in effect, whether accent grave, ESCape, or another character.

Note that although alternate delimiters may be used when typing in TECO
commands, they may not be used in macros or indirect command files. In
those cases, the only delimiter is allowed is ESCape.

#### Command Format

The general format for a TECO command is as follows:

[[*m*,]*n*] [:[:]] [@] command [*q*] [*text1*[[\<DELIM\>]*text2*]] [\<DELIM\>]

The fields in brackets are either optional, or are dependent on the
command being executed. These are for illustration only; none of TECO's
commands use all of these fields.

| Field | Description | Example |
| ----- | ----------- | ------- |
| *m*   | A numeric argument or expression, which may include one or more TECO variables. Separated from the following *n* argument with a comma. | 10,20K |
| *n*   | A numeric argument or expression, which may include one or more TECO variables. | 42C |
| : | Modifies the behavior of the following command, often in order to return a value indicating success or failure of the command rather than aborting execution and issuing an error. | \:ERinput.c\` |
| :: | Also modifies the behavior of the following command, but differently than :. Often used for "anchored" searches. | \:\:Sbaz\` |
| @ | Specifies an alternate form for delimiting any text arguments that follow the command. | \@^A/hello/ |
| command | The TECO command to be executed. It will consist of one, two, or three characters. | V |
| *q* | The name of the Q-register that the command will use. | 1XA |
| *text1* | The first (or only) text argument for the command. | Ifoobaz\` |
| *text2* | The second text argument for the command. | FSfoo\`baz\` |
| \<DELIM\> | The command delimiter. Typically only required when the at-sign modifier is not used for commands that take text arguments. Also, not required between text arguments if an at-sign modifier is used. | EWoutput.c\` |

Note that if a command allows both colon and at-sign modifiers, they
may be specified in either order.

A TECO command consists of one or two characters which cause a specific
operation to be performed. Some TECO commands may be preceded or followed
by arguments. Arguments may be either numeric or textual. A numeric
argument is simply an integer value which can be used to indicate, for
example, the number of times a command should be executed. A text argument
is a string of ASCII characters which might be, for example, words of text
or a file specification.

If a command requires a numeric argument, the numeric argument always
precedes the command. If a command requires a text argument, the text
argument always follows the command. Each text argument is terminated by
a special character (usually a \<DELIM\> character, which TECO hears as an
ESCape - see Introduction). This indicates to TECO that the next character
typed will be the first character of a new command.

TECO accumulates commands as they are typed in a command string, and
executes commands upon receipt of two consecutive \<DELIM\> characters.
\<DELIM\> may be any character you select (if your TECO and operating system
support user-selectable ESCape surrogates - see sections on ET and EE flags).

When you type the character you have designated as \<DELIM\>, TECO receives
an ESCape character, and an \` (accent grave) is echoed. (If you are not using
an ESCape surrogate — that is, you are actually pressing an ESCape key —
a dollar sign is echoed.) The accent grave character is used in examples
throughout this manual to represent typed \<DELIM\>s. Note that the carriage
 returncharacter has no special significance to TECO; only the
\<DELIM\>\<DELIM\> forces execution of the command string.

TECO executes command strings from left to right until either all commands
have been executed or a command error is recognized. It then prints an
asterisk to signal that additional commands may be entered.

If TECO encounters an erroneous command, it prints an error message and
ignores the erroneous command as well as all commands which follow it. All
error messages are of the form:

?XXX Message

where XXX is an error code and the message is a description of the error.
Some error messages mention the specific character or string of characters
in error. In these error messages, TECO represents the non-printing special
characters as follows:

| ASCII character       | Form Displayed |
| --------------------- | -------------- |
| 9 (horizontal tab)    | \<TAB\>        |
| 10 (line feed)        | \<LF\>         |
| 11 (vertical tab)     | \<VT\>         |
| 12 (form feed)        | \<FF\>         |
| 13 (carriage return)  | \<CR\>         |
| 27 (ESCape)           | \<ESC\>        |
| (other control chrs.) | \<^x\>         |

Every error message is followed by an asterisk at the left margin,
indicating that TECO is ready to accept additional commands. If you
type a single question mark character after a TECO-generated error
message, TECO will print the erroneous command string up to and
including the character which caused the error message. This helps
you to find errors in long command strings and to determine how much
of a command string was executed before the error was encountered.

You can correct typing errors by hitting the DELETE key, which may be
labeled DEL or RUBOUT or \<x on your keyboard. Each depression of the
DELETE key deletes one character and echoes it on your terminal,
beginning with the last character typed. If you have a display terminal,
TECO will actually erase the deleted character from the screen. You can
delete an entire command string this way, if necessary. To delete an
entire line of commands, enter the character \<CTRL/U\>, typed by
holding down the CTRL key while depressing the "U" key.
When you are done editing, use the EX command to exit TECO.

#### Data Structure Fundamentals

TECO considers any string of ASCII codes to be **text**. Text is broken
down into units of **characters**, **lines**, and **pages**. A character
is one ASCII code. A line of text is a string of ASCII codes including
one line terminator (usually a line feed) as the last character on the
line. A page of text is a string of ASCII codes including one form feed
character as the last character on the page.

TECO maintains a **text buffer** in which text is stored. The buffer usually
contains one page of text, but the terminating form feed character never
appears in the buffer. TECO also maintains a text buffer **pointer**, called
*dot*. *dot* is a movable position indicator which is never located directly
on a character, but is always **between** characters: between two characters
in the buffer, before the first character in the buffer, or after the
last character in the buffer.

Line feed and form feed characters are inserted automatically by TECO. A line
feed is automatically appended to every carriage return typed to TECO and a
form feed is appended to the contents of the buffer by certain output commands.
Additional line feed and form feed characters may be entered into the buffer
as text. If a form feed character is entered into the buffer, it will cause
a page break upon output; text following the form feed will begin a new page.

Finally, TECO maintains an *input file* and an *output file*, both of which
are selected by the user through file specification commands. The input file
may be on any device from which text may be accepted. The output file may be
on any device on which edited text may be written.

TECO functions as a **pipeline** editor. Text is read from the input file
into the text buffer, and is written from the buffer onto the output file.
If virtual memory paging support is enabled, it is possible to "back up"
as well as page forward in the file being edited. In other implementations,
once text has been written to the output file, it cannot be accessed again
without closing the output file and reopening it as an input file.

#### File Selection Commands

Input and output files may be specified to TECO in several ways. The following
sections present first a simple method for specifying files, and then more
sophisticated commands that permit flexible file selection.

- Note: All of the following file selection commands are shown with a general
argument of *filespec*. The actual contents of this file specification argument
are operating system dependent.

##### Simplified File Selection

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
brings the first page of the file into the text buffer. These functions
simulate the EB command.

If any of the above commands do not seem to work on your operating system,
consult the appropriate appendix for information about how to install TECO and
its associated operating system commands.

##### Input File Specification (ER command)

TECO will accept input text from any input device in the operating system. The
input device may be specified by the text string supplied in the ER command (and
which, like any text argument, is terminated by a \<DELIM\> character). The ER
command causes TECO to open the specified file or print an error message if the
file is not found. This command does not cause any portion of the file to be read
into the text buffer, however. The following examples illustrate use of the ER
command.

| Command | Function |
| ------- | -------- |
| ER*filespec*\` | General form of the ER command where *filespec* is the designation of the input file. The command is terminated by a \<DELIM\ > character. |
| ERhello.c\` | Prepare to read input file hello.c from the current directory. |

TECO will only keep one input and one output file open and selected at a time.
The current input file may be changed by simply using the ER command to
specify a new file.

It is not always necessary to specify an input file. If you want to create a file
without using any previously edited text as input, you may type commands to
insert the necessary text directly into the text buffer from the keyboard and, at
the end of each page, write the contents of the buffer onto an output file. Since
all input is supplied from the keyboard, no input file is necessary.

##### Output File Specification (EW command)

TECO will write output text onto any output device in the operating system.
The output file may be specified by means of the text string supplied in an EW
command. If the output device is a file-structured device (for example, a disk),
the file name and any extension must be supplied. If a file name is specified
but no device is explicitly specified, the system’s default device is assumed. The
following examples illustrate use of the EW command.

| Command | Function |
| ------- | -------- |
| EW*filespec*\` | General form of the EW command where *filespec* is the designation of the input file. The command is terminated by a \<DELIM\> character. |
| EWhello.c\` | Prepare to write output file hello.c to the default directory. |
| ERinput.c\` EWoutput.c`` | Open an input file input.c and open  an output file named output.c. The double <DELIM> (echoed as ``) terminates the command string and causes the string to be executed. Note that the \<DELIM\> which terminates the EW command may be one of the two \<DELIM\>s which terminates the command string.

You do not need to specify an output file if you only want to examine an input
file, without making permanent changes or corrections. In this case, the contents
of the input file may be read into the text buffer page by page and examined at
the terminal. Since all output is printed on the user terminal, no output file is
needed.

##### Closing Files (EX command)

When you are finished editing a file, use the EX command to close out the file
and exit from TECO. The current contents of the text buffer and any portion of
the input file that has not been read yet are copied to the output file before TECO
exits. The EX command takes no arguments.

| Command | Function |
| ------- | -------- |
| EX\`\` |  Write the text buffer to the current output file, move the remainder of the current input file to the current output file, close the output file, then return to the operating system. |
| ERinput.c\` EWoutput.c\` EX\`\` | Open an input file input.c and open an output file named output.c, then copy all the text in the input file to the output file, close the output file, and exit from TECO. |

#### Input and Output Commands

The following commands permit pages of text to be read into the TECO text
buffer from an input file or written from the buffer onto an output file. Once a
page of text has been written onto the output file, it cannot be recalled into the
text buffer unless the output file is closed and reopened as an input file.

| Command | Function |
| ------- | -------- |
| Y | Clear the text buffer, then read the next page of the input file into the buffer. Since the Y command causes the contents of the text buffer to be lost, it is not permitted if an output file is open and there is text in the buffer. |
| P | Write the contents of the text buffer onto the next page of the output file, then clear the buffer and read the next page of the input file into the buffer. |
| *n*P | Execute the P command *n* times. If *n* is not specified, a value of 1 is assumed. |

After each Y, P, or nP command, TECO positions the pointer before the first
character in the buffer.

#### Pointer Positioning Commands

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

#### Type-Out Commands

The following commands permit sections of the text in the buffer to be printed
out on your terminal for examination. These commands do not move the buffer
pointer.

| Command | Function |
| ------- | -------- |
| T | Type the contents of the text buffer from the current position of the pointer through and including the next line feed character. 
| *nT | Type *n* lines, where *n* is a signed integer. A positive value of *n* causes the *n* lines following the pointer to be typed. A negative value of *n* causes the *n* lines preceding the pointer to be typed. If *n* is zero, the contents of the buffer from the beginning of the line on which the pointer is located up to the pointer is typed. This is useful for verifying the location of the buffer pointer. |
| HT | Type the entire contents of the text buffer. |
| V | Type the current line. Equivalent to the sequence "0TT". |

#### Immediate Inspection Commands

In addition, there are available as a convenience abbreviations of two frequently
used type out commands. Each one consists of a single character, and must be
the very first character typed after TECO prints its prompting asterisk. Each
of these commands takes effect immediately; there is no need to follow any of
these commands by any <DELIM> character. For this reason, these commands
are known as "immediate" commands.

| Command | Function |
| ------- | -------- |
| \<LF\> | Immediately execute an LT command. This command is issued by typing the line feed key as the first keystroke after TECO’s prompt. It causes TECO to move the pointer ahead one line and then type out the new line. On terminals without a line feed key, typing \<CTRL/J\> has the same effect. (See also the next command.) |
| \<DELIM\> | Immediately execute an LT command. (Action identical to that of the command above.) This command is available when an ESCape surrogate is active, and causes TECO to move the pointer ahead one line and then type out the new line when a \<DELIM\> is the first thing typed after TECO’s prompting asterisk. |
| \<BS\> | Immediately execute a –LT command. This command is issued by typing the backspace key as the first keystroke after TECO’s prompt. It causes TECO to move the pointer back one line and then type the line just moved over on the terminal. On terminals without a backspace key, typing \<CTRL/H\> has the same effect. |

These commands are useful for "walking through" a file, examining and/or
modifying lines one at a time.

#### Text Modification Commands

You can insert or delete text from the buffer using the following commands:

| Command | Function |
| ------- | -------- |
| I*text*` | Where *text* is a string of ASCII characters terminated by a \<DELIM\> character. The specified text is inserted into the buffer at the current position of the pointer. The pointer is positioned immediately after the last character of the insertion.
| K | Delete the contents of the text buffer from the current position of the pointer up to and including the next line feed character. |
| *n*K | Execute the K command *n* times, where *n* is a signed integer. A positive value of *n* causes the *n* lines following the pointer to be deleted. A negative value of *n* causes the *n* lines preceding the pointer to be deleted. If *n* is zero, the contents of the buffer from the beginning of the line on which the pointer is located up to the pointer is deleted. |
| HK | Delete the entire contents of the text buffer. |
| D | Delete the character following the buffer pointer. |
| *n*D | Execute the D command *n* times, where *n* is a signed integer. A positive value of *n* causes the *n* characters following the pointer to be deleted. A negative value of *n* causes the *n* characters preceding the pointer to be deleted. If *n* is zero, the command is ignored. |

Like the L and C commands, the K and D commands may not execute across
page boundaries. If a K command attempts to delete text up to and across the
beginning or end of the buffer, text will be deleted only up to the buffer boundary
and the pointer will be positioned at the boundary. No error message is printed.
A D command attempting to delete text past the end or beginning of the text
buffer will produce an error message and the command will be ignored.

#### Search Commands

The following commands may be used to search the input file for a specified
string of characters.

Command Function

| Command | Function |
| ------- | -------- |
| S*text*` | Where *text* is a string of ASCII characters terminated by a \<DELIM\> character. This command searches the text buffer for the next occurrence of the specified character string following the current pointer position. If the string is found, the pointer is positioned after the last character on the string. If it is not found, the pointer is positioned immediately before the first character in the buffer and an error message is printed. |
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

#### Interlude

The rest of this manual is a description of TECO in all its glory. TECO is a
complex editor and has many features because it has been under development for
a long time. Do not try to understand everything the first time through.
If you find that what you are reading seems hopelessly obscure, or makes no
sense whatsoever, skip to the next section and come back to it some time later.
It will be a while before you need all of TECO’s features.

This manual is meant to be a reference manual and not a tutorial.
