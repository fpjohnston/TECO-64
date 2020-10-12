## TECO-64 - Notations and Conventions

### Character Set

TECO uses the 8-bit ASCII character set. It currently has no ability
to use multi-character encodings such as Unicode.

### Special Characters

| Notation        | ASCII code | Meaning |
| --------------- | ---------- | ---- |
| \<NUL\>         | 0          | NUL |
| \<BS\>          | 8          | Backspace |
| \<TAB\>         | 9          | Horizontal tab |
| \<LF\>          | 10         | Line feed |
| \<VT\>          | 11         | Vertical tab |
| \<FF\>          | 12         | Form feed |
| \<CR\>          | 13         | Carriage return |
| \<ESC\>         | 27         | ESCape |
| \<CTRL/*x*\>    | (various)  | Control-*x* |
| ^*x*            | (various)  | Either the two-character sequence ^ and *x*, or \<CTRL/*x*\>. When ^*x* is used in the manual, it means that either that sequence or CTRL/*x* is allowed at that position. When CTRL/*x* is used, that means that only the control character is allowed. |
| \<SPACE\>       | 20         | Space |
| \`              | 96         | Accent grave. Used in example code in this manual to indicate a general delimiter, whether ESCape, accent grave, or other character. |
| \<DEL\>         | 127        | Delete |
| \<*delim*\>     | \-         | Refers to any character which is used as a delimiter for a command or command string. This was originally \<ESC\>, but later versions of TECO allowed users to specify an alternate delimiter (often accent grave), either with the ET&8192 flag bit, or the EE flag. <br><br>If an alternate delimiter has been defined, then it will always be echoed as an accent grave, as will ESCape. If no alternate delimiter has been defined, ESCape will echo as a dollar sign ($). <br><br>Since an alternate delimiter can be user-defined, and since there can be as many as three delimiters in effect at a given time, \<*delim*\> is used throughout this manual to refer to any character that may be in use as a command delimiter. Also, accent grave is always used to indicate a delimiter (whether ESCape, accent grave, or other character) in any example commands. <br><br>Note that although alternate delimiters may be used when typing in TECO commands, they may not be used in macros or indirect command files. In those cases, the only delimiter allowed is ESCape. |

### Flag Bits

Individual bits in flag commands such as ED or ET are referenced as follows:

[command] & [bit mask]

An example might be:

ET&8192

This refers to bit 13 (starting from 0) in the ET flag.

### Command Case

TECO does not distinguish between upper and lower case commands. For
example, the commands MQ, mQ, Mq, and mq are all equivalent.
