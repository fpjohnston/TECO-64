## TECO-64 - Conventions and Notations

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
includes \<ESC\>, for instance, that means that the user should type
the ESCape character at that position.

| Notation        | ASCII code(s) | Meaning |
| --------------- | ------------- | ---- |
| \<NUL\>         | 0             | NUL |
| \<BS\>          | 8             | Backspace |
| \<TAB\>         | 9             | Horizontal tab |
| \<LF\>          | 10            | Line feed |
| \<VT\>          | 11            | Vertical tab |
| \<FF\>          | 12            | Form feed |
| \<CR\>          | 13            | Carriage return |
| <nobr>\<CR\>\<LF\></nobr>    | 13+10         | Carriage return/line feed sequence |
| \<ESC\>         | 27            | ESCape |
| \<CTRL/*x*\>    | -             | Control-*x* |
| \<SPACE\>       | 20            | Space |
| ^*x*            | -             | When used to illustrate TECO output, it indicates the equivalent control character, \<CTRL/*x*\>. So, "^A" would mean \<CTRL/A\>. <br><br> When used to illustrate a command, it means that either the literal two character sequence, with a caret (^) followed by an alphabetic or special character, *or* the equivalent control character is allowed. If the example uses \<CTRL/X\>, that means that the ^*x* sequence is not allowed in that situation. |
| \`              | 96            | Accent grave. Used in example code in this manual to indicate a general delimiter, whether ESCape, accent grave, or other character. |
| \<DEL\>         | 127           | Delete |
| \<*delim*\>     | \-            | Refers to any character which is used as a delimiter for a command or command string. Originally, TECO only used \<ESC\>, but later versions allowed users to specify an alternate delimiter (often accent grave), either with the ET&8192 flag bit, or the EE flag. <br><br>If an alternate delimiter has been defined, then it will always be echoed as an accent grave, as will ESCape. If no alternate delimiter has been defined, ESCape will echo as a dollar sign ($). <br><br>Since an alternate delimiter can be user-defined, and since there can be as many as three delimiters in effect at a given time, \<*delim*\> is used throughout this manual to refer to any character that may be in use as a command delimiter. <br><br>Note that although alternate delimiters may be used when typing in TECO commands, they may not be used in macros or indirect command files. In those cases, the only delimiter allowed is ESCape. |

