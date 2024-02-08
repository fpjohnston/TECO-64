## TECO-64 - Document Conventions

<style>
    span {
        white-space: nowrap;
    }
</style>

Before proceeding with a description of TECO syntax and commands,
the following is presented to enumerate the special conventions and
notations used throughout this manual.

### Command Case

TECO does not distinguish between upper and lower case commands. For
example, the commands MQ, mQ, Mq, and mq are all equivalent.

### Flag Bits

Bits in flag commands such as ED or ET are referenced individually
as follows:

[command] & [bit mask]

Example: ET&8192, which refers to bit 13 (starting from 0) in the ET flag.

### Special Characters

Some of the characters and character sequences used to illustrate input
commands and output messages require special notation to make it clear
what is intended.
These are listed in the table below. So, if an example TECO command
includes &lt;ESC>, for instance, that means that the user should type
the ESCape character at that position.

| Notation        | ASCII code(s) | Meaning |
| --------------- | ------------- | ---- |
| &lt;NUL>         | 0             | NUL |
| &lt;BS>          | 8             | Backspace |
| &lt;TAB>         | 9             | Horizontal tab |
| &lt;LF>          | 10            | Line feed |
| &lt;VT>          | 11            | Vertical tab |
| &lt;FF>          | 12            | Form feed |
| &lt;CR>          | 13            | Carriage return |
| <span>&lt;CR>&lt;LF></span>    | 13+10         | Carriage return/line feed sequence |
| &lt;ESC>         | 27            | ESCape |
| &lt;CTRL/*x*>    | -             | Control-*x* |
| &lt;SPACE>       | 20            | Space |
| \^*x*            | -             | When used to illustrate TECO output, it indicates the equivalent control character, &lt;CTRL/*x*>. So, "\^A" would mean &lt;CTRL/A>. <br><br> When used to illustrate a command, it means that either the literal two character sequence, with a caret (\^) followed by an alphabetic or special character, *or* the equivalent control character is allowed. If the example uses &lt;CTRL/X>, that means that the \^*x* sequence is not allowed in that situation. |
| \`              | 96            | Accent grave. Not a TECO command, but used in example code in this manual to indicate a general delimiter, whether ESCape, accent grave, or other character. |
| &lt;DEL>         | 127           | Delete |
| &lt;*delim*>     | \-            | Refers to any character which is used as a delimiter for a command or command string. Originally, TECO only used &lt;ESC>, but later versions allowed users to specify an alternate delimiter (often accent grave), either with the ET&8192 flag bit, or the EE flag. <br><br>If an alternate delimiter has been defined, then it will always be echoed as an accent grave, as will ESCape. If no alternate delimiter has been defined, ESCape will echo as a dollar sign ($). <br><br>Since an alternate delimiter can be user-defined, and since there can be as many as three delimiters in effect at a given time, &lt;*delim*> is used throughout this manual to refer to any character that may be in use as a command delimiter. <br><br>Note that although alternate delimiters may be used when typing in TECO commands, they may not be used in macros or indirect command files. In those cases, the only delimiter allowed is ESCape. |

### Digit Strings

Numbers may be specified as a string of digits in either decimal or octal.
The \^D, \^O, and \^R commands are used to set the current radix. If the radix
is octal, the digits 8 and 9 are invalid and will result in a ILN error.

If the radix is hexadecimal, only digits 0 through 9 may be used; there is
no way to specify hex digits A through F, as those could be interpreted as
TECO commands.

### Whitespace

&lt;LF>, &lt;VT>, &lt;FF>, &lt;CR>, and &lt;SPACE> are ignored in command strings,
except when they appear as part of a text argument. Numeric values are not
affected. These characters may be inserted between any two TECO commands,
or between fields in a TECO command, to lend clarity to a long command string
without affecting its execution.

Whitespace does not include &lt;TAB>, since that is a TECO command.

Also, whitespace may not occur between characters of a multi-character
command (e.g., the local search and replace command must always be
specified as "FS" and not "F S"), nor between digits of a digit string.
This is a change from earlier TECOs.


