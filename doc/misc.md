## TECO-64 - Miscellaneous Commands

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

Only \<SPACE\>, \<LF\>, \<VT\>, \<FF\>, and \<CR\> can be used to format
command strings. \<TAB\> cannot be used, since that is an insertion command.

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

### Messages

The \<CTRL/A\> command may be used to print out a statement at any point
during the execution of a command string. The \<CTRL/A\> command has the
general form:

```^Atext\<CTRL/A\>```

or

```@^A/text/```

The first ^A is the actual command, which may be entered by striking the
control key and the A key simultaneously or by typing a caret (uparrow)
followed by an A character. The second \<CTRL/A\> character of the first
form shown is the command terminator, which must be entered by typing
the control key and the A key simultaneously. In the second form, the
second occurrence of the delimiting character (shown as slash in the
example) terminates the message. Upon execution, this command causes
TECO to print the specified message at the terminal.

The ^Amessage\<CTRL/A\> command is particularly useful when it precedes a
command whose numeric argument contains ^T or ^F characters. The message
may contain instructions notifying the user as to what sort of input is
required.

### Tracing

A question mark (?) entered betweeen any two commands in a command string
causes TECO to print all subsequent commands at the terminal as they are
executed. Commands will be printed as they are executed until another question
mark character is encountered or the command string terminates.

### Convenience Characters

| Command | Function |
| ------- | -------- |
| \<NUL\> | Null characters in command strings will be ignored. Numeric values are not affected. |
| \<ESC\> | An ESCape that is executed as a TECO command (as distinct from an immediate action command or an ESCape that is part of the syntax of some other TECO command) is ignored by TECO. However, any pending numeric values are discarded. This command is useful for discarding the value returned from a command (such as n%q or m,n:W) when you don’t want that value to affect execution of the following command. |
| ^[ | Same as \<ESC\>. Like any other TECO command that is a control character, ESCape may be entered in up-arrow mode. |

### Memory Commands

| Command | Function |
| ------- | -------- |
| *n*EC | *n*EC tells TECO to expand or contract until it uses *n*K bytes of memory for its edit buffer. If this is not possible, then TECO’s memory usage does not change. The 0EC command tells TECO to use the least amount of memory possible. |

### Case Commands

| Command | Function |
| ------- | -------- |
| *n*FL | Change the following *n* lines to lower case. |
| 0*n*FL | Change the preceding *n* lines to lower case. |
| *m*,*n*FL | Change all characters between buffer positions *m* and *n* to lower case. |
| *n*FU | Change the following *n* lines to upper case. |
| *n*FU | Change the preceding *n* lines to upper case. |
| *m*,*n*FU | Change all characters between buffer positions *m* and *n* to upper case. |
| ^V | Puts TECO into lower case conversion mode. In this mode, all alphabetic characters in string arguments are automatically changed to lower case. This mode can be overridden by explicit case control within the search string. This command makes all strings behave as if they began with a \<CTRL/V\>\<CTRL/V\>. |
| 0^V | Returns TECO to its original mode. No special case conversion occurs within strings except those case conversions that are explicitly specified by \<CTRL/V\> and \<CTRL/W\> string build constructs located within the string. |
| ^W | Puts TECO into upper case conversion mode. In this mode, all alphabetic characters in string arguments are automatically changed to upper case. This mode can be overridden by explicit case control within the search string. This command makes all strings behave as if they began with \<CTRL/W\>\<CTRL/W\>. |
| 0^W | Returns TECO to its original mode. No special case conversion occurs within strings except those case conversions that are explicitly specified by \<CTRL/\V> and \<CTRL/W\> string build constructs located within the string. |

### Radix Control Commands

| Command | Function |
| ------- | -------- |
| ^O | \<CTRL/O\> causes all subsequent numeric input to be accepted as octal numbers. Numeric conversions using the \\ or *n*\\ commands will also be octal. The digits 8 and 9 become invalid as numeric characters. The octal radix will continue to be used until the next ^D command is executed or until TECO’s radix is changed by an *n*^R command. |
| ^D | \<CTRL/D\> causes all subsequent numeric input to be accepted as decimal numbers. This is the initial setting. |
| *n*^R | This command sets TECO’s radix to the value of *n*. *n* may only be 8, 10, or 16 (representing octal mode, decimal mode, or hexadecimal mode). If *n* is not one of these values, TECO’s radix remains unchanged and the ?IRA error message is issued. |

