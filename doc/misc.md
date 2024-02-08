## TECO-64 - Miscellaneous Commands

### Ignored and Convenience Commands

| Command | Function |
| ------- | -------- |
| &lt;NUL> | Null characters in command strings will be ignored. Numeric values are not affected. |
| &lt;ESC> | An ESCape that is executed as a TECO command (as distinct from an immediate action command or an ESCape that is part of the syntax of some other TECO command) is ignored by TECO. However, any pending numeric values are discarded. This command is useful for discarding the value returned from a command (such as n%q or m,n:W) when you don’t want that value to affect execution of the following command. |
| \^[ | Same as &lt;ESC>. Like any other TECO command that is a control character, ESCape may be entered in up-arrow mode. |

### Comments

One of the most powerful features of TECO is its ability to store very long
command strings so that a given sequence of commands may be executed
whenever needed. Long command strings may be thought of as editing programs
and, like any other type of program, they should be documented by means of
comments.

Comments, like tags, are delimited between a pair of
exclamation marks. They may be inserted between any two commands,
and may contain any characters except special characters such as
command delimiters). Thus a long TECO macro might look like:

```TECO commands ! This comment describes line 1 !```

```TECO commands ! This comment describes line 2 !```

```more commands ! yada yada yada !```

```final commands ! end of command string!```

Only &lt;SPACE>, &lt;LF>, &lt;FF>, and &lt;CR> can be used to format
command strings. &lt;TAB> cannot be used, since that is an insertion command.

It is suggested, but not required, that, in order to help distinguish
tags from comments, tags should only consist of alphanumeric characters,
and that comments should start with a space or punctuation character.
Both tags and comments may contain control characters, but this is not
recommended.

If the E1&64 flag bit is set, an alternate comment form may be used,
which starts with a double exclamation, and ends with the next line
delimiter:

```TECO commands !! This comment describes line 1```

```TECO commands !! This comment describes line 2```

A numeric value that occurs before a comment or tag will be passed
through to the next command, much like the [ and ] commands.

### Printing Messages

The &lt;CTRL/A> command may be used to print out a statement at any point
during the execution of a command string. The &lt;CTRL/A> command has the
general form:

```^Atext&lt;CTRL/A>```

or

```@^A/text/```

The first \^A is the actual command, which may be entered by striking the
control key and the A key simultaneously or by typing a caret (uparrow)
followed by an A character. The second &lt;CTRL/A> character of the first
form shown is the command terminator, which must be entered by typing
the control key and the A key simultaneously. In the second form, the
second occurrence of the delimiting character (shown as slash in the
example) terminates the message. Upon execution, this command causes
TECO to print the specified message at the terminal.

The \^Amessage&lt;CTRL/A> command is particularly useful when it precedes a
command whose numeric argument contains \^T or \^F characters. The message
may contain instructions notifying the user as to what sort of input is
required.

### Tracing Commands

A question mark (?) entered between any two commands in a command string
causes TECO to print all subsequent commands at the terminal as they are
executed. Commands will be printed as they are executed until another question
mark character is encountered or the command string terminates.

### Squishing Command Strings

| Command | Function |
| ------- | -------- |
| *n*EM*q* | EM basically traces the command string in Q-register *q*, except that it only echoes commands and does not executes them, and it allows the user to suppress tracing of certain commands. This feature can be used to delete extraneous characters and create a new macro which executes more efficiently (although at the expense of readability). The original macro is left unchanged. It is functionally similar to the original TECO *squ.tec* macro, and as with that macro, it allows some customization of its behavior through use of the *n* argument, which is bit-encoded as follows: |

| Bit | Function |
| --- | -------- |
| *n*&1 |  If set, don't trace spaces unless they are part of an argument to a command. | 
| *n*&2 | If set, don't trace blank lines (containing zero or more spaces, terminated by an LF character). |
| *n*&4 | If set, don't trace VT, FF, or CR characters. |
| *n*&8 | If set, don't trace tags (starting with "! "). |
| *n*&16 | If set, don't trace comments (starting with "!!"). |

These bits may be OR'd together. If *n* is -1, this is equivalent to setting all bits. If *n* is 0, or if *n* is not specified, then the macro will be echoed unchanged.

The *squish.tec* indirect command file may be used to squish macros, as in the following example, in which the *n* argument is set by the -A command-line option:

    teco -T macro.tec -A -1 -E squish -X >newmacro.tec

### Memory Commands

| Command | Function |
| ------- | -------- |
| *n*EC | *n*EC tells TECO to expand or contract until it uses *n*K bytes of memory for its edit buffer. If this is not possible, then TECO’s memory usage does not change. The 0EC command tells TECO to use the least amount of memory possible, and the -1EC command tells TECO to use the most amount of memory possible. |

### Case Commands

| Command   | Function |
| --------- | -------- |
| FL        | Change current line to lower case, from dot through end of line. |
| 1FL       | Same as FL. |
| 0FL       | Change current line to lower case, from beginning of line to dot. |
| 0FL1FL    | Change entire current line to lower case. |
| -1FL      | Change text to lower case in previous line. |
| -FL       | Same -1FL. |
| *n*FL     | Change the following *n* lines to lower case. |
| -*n*FL    | Change the preceding *n* lines to lower case. |
| *m*,*n*FL | Change all characters between buffer positions *m* and *n* to lower case. |
| HFL       | Change contents of entire edit buffer to lower case. |
| FU        | Change current line to upper case, from dot through end of line. |
| 1FU       | Same as FU. |
| 0FU       | Change current line to upper case, from beginning of line to dot. |
| 0FU1FU    | Change entire current line to upper case. |
| -1FU      | Change text to upper case in previous line. |
| -FU       | Same -1FU. |
| *n*FU     | Change the following *n* lines to upper case. |
| -*n*FU    | Change the preceding *n* lines to upper case. |
| *m*,*n*FU | Change all characters between buffer positions *m* and *n* to upper case. |
| HFU       | Change contents of entire edit buffer to upper case. |
| \^V        | Puts TECO into lower case conversion mode. In this mode, all alphabetic characters in string arguments are automatically changed to lower case. This mode can be overridden by explicit case control within the search string. This command makes all strings behave as if they began with a &lt;CTRL/V>&lt;CTRL/V>. |
| 0\^V       | Returns TECO to its original mode. No special case conversion occurs within strings except those case conversions that are explicitly specified by &lt;CTRL/V> and &lt;CTRL/W> string build constructs located within the string. |
| \^W        | Puts TECO into upper case conversion mode. In this mode, all alphabetic characters in string arguments are automatically changed to upper case. This mode can be overridden by explicit case control within the search string. This command makes all strings behave as if they began with &lt;CTRL/W>&lt;CTRL/W>. |
| 0\^W       | Returns TECO to its original mode. No special case conversion occurs within strings except those case conversions that are explicitly specified by &lt;CTRL/\V> and &lt;CTRL/W> string build constructs located within the string. |

### Radix Control Commands

| Command | Function |
| ------- | -------- |
| \^O | &lt;CTRL/O> causes all subsequent numeric input to be accepted as octal numbers. Numeric conversions using the \\ or *n*\\ commands will also be octal. The digits 8 and 9 become invalid as numeric characters. The octal radix will continue to be used until the next \^D command is executed or until TECO’s radix is changed by an *n*\^R command. |
| \^D | &lt;CTRL/D> causes all subsequent numeric input to be accepted as decimal numbers. This is the initial setting. |
| *n*\^R | This command sets TECO’s radix to the value of *n*. *n* may only be 8, 10, or 16 (representing octal mode, decimal mode, or hexadecimal mode). If *n* is not one of these values, TECO’s radix remains unchanged and the ?IRA error message is issued. |

### Aborting Execution

| Command | Function |
| ------- | -------- |
| \^C | Causes command execution to stop and control return to TECO’s prompt. No clean-up of push-down lists, flag settings, etc. is done. This command lets a macro abort TECO’s command execution. |
| \^C&lt;CTRL/C> | Causes TECO to unconditionally abort and control exits from TECO. Control returns to the operating system. The second &lt;CTRL/C> must be a true &lt;CTRL/C> and may not be a Caret-C. |


