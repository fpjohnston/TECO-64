## TECO-64 - Arithmetic Operators and Expressions

The numeric argument of a TECO command may consist of a single integer, any
of the characters listed in (TBD), the numeric contents of any Q-register,
or an arithmetic combination of these elements. If an arithmetic expression
is supplied as a numeric argument, TECO will evaluate the expression.
All arithmetic expressions are evaluated from left to right without any
operator precedence. Parentheses may be used to override the normal order
of evaluation of an expression. If parentheses are used, all operations within
the parentheses are performed, left to right, before operations outside the
parentheses. Parentheses may be nested, in which case the innermost expression
contained by parentheses will be evaluated first. The table below lists all of
the arithmetic operators that may be used in arithmetic expressions.

### Arithmetic Operators

| Command | Example | Function |
| ------- | --------| -------- |
| \+ | \+2=2 | Unary plus. |
| \+ | 5+6=11 | Binary addition. |
| \- | \-2=-2 | Unary minus (2's complement negation). |
| \- | 8-2=6 | Binary subtraction. |
| \* | 8\*2=16 | Binary multiplication. |
| \/ | 8/3=2 | Binary division yielding quotient. |
| \& | 12&10=8 | Bitwise logical AND. |
| \# | 12#10=14 | Bitwise logical OR. |
| \^_ | 5\^_=-6 | Unary one’s complement. Note that it is used *after* an expression. |

### Extended Operators

If the E1&1 bit is set, then there are additional operators that can be used, as long as they are within parentheses (to avoid confusion with commands that may use the same characters). Note that any of the multi-character operators must be entered as a single token, with no intervening whitespace.

| Command | Example | Function |
| ------- | --------| -------- |
| ==      | (1==1)=-1   | Relational comparison for equality. |
| \<\>    | (1\<\>1)=0  | Relational comparison for inequality. |
| \<      | (1\<1)=0    | Relational comparison for less than. |
| \<=     | (1\<=1)=-1  | Relational comparison for less than or equal. |
| \>      | (1\>1)=0    | Relational comparison for greater than. |
| \>=     | (1\>=1)=0   | Relational comparison for greater than or equal. |
| //      | (7//3)=1    | Binary division yielding remainder. |
| !       | (!0)=-1     | Bitwise logical NOT. |
| \~      | (7~3)=4     | Bitwise logical XOR. |
| \<\<    | (1<<3)=8    | Arithmetic left shift. |
| \>\>    | (16>>2)=4   | Arithmetic right shift. |

### Conversion and Radix Control Commands

| Command | Function |
| ------- | -------- |
| *n*= | This command causes the value of *n* to be output at the terminal in decimal followed by a carriage return and line feed. Decimal numeric conversion is signed. TECO's radix is unaltered. |
| *n*== | This command causes the value of *n* to be output at the terminal in octal (base 8) followed by a carriage return and line feed. Octal numeric conversion is unsigned. TECO’s radix is unaltered. |
| *n*=== | This command causes the value of *n* to be output at the t erminal in hexadecimal (base 16) followed by a CR/LF. Hexadecimal output is unsigned. radix is unaltered. |
*n*:=, *n*:==, and *n*:=== | These commands are equivalent to *n*=, *n*==, and *n*===, respectively, except that no CR/LF is output. |
| *n*@=/*format*/ | Output the value of *n* using *format* as a *printf()* format string, followed by a CR/LF. Any string for *printf()* may be used, as long as it contains at most one numeric conversion specifier. Non-numeric specifiers such as %s, or multiple numeric specifiers, are not allowed. However, %% may be used to print a percent sign. Also, the output format is used regardless whether =, ==, or === is specified. |
| *n*:@=/*format*/ | Equivalent to *n*@=/*format*/, except that no CR/LF is added. |
| ^O | \<CTRL/O\> causes all subsequent numeric input to be accepted as octal numbers. Numeric conversions using the \\ or *n*\\ commands will also be octal. The digits 8 and 9 become illegal as numeric characters. The octal radix will continue to be used until the next ^D command is executed or until TECO’s radix is changed by an *n*^R command. |
| ^D | \<CTRL/D\> causes all subsequent numeric input to be accepted as decimal numbers. This is the initial setting. |
| ^R | This command returns the binary value of TECO’s current radix. |
| *n*^R | This command sets TECO’s radix to the value of *n*. *n* may only be 8, 10, or 16 (representing octal mode, decimal mode, or hexadecimal mode). If *n* is not one of these values, TECO’s radix remains unchanged and the ?IRA error message is issued. |
| \\ | A backslash character which is not preceded by a numeric argument causes TECO to evaluate the digit string (if any) beginning with the character immediately following the buffer pointer and ending at the next character that is not valid for the current radix. The value becomes the numeric argument of the next command. The first character may be a digit or \+ or \-. As the backslash command is evaluated, TECO moves the buffer pointer to a position immediately following the digit string. If there is no digit string following the pointer, the result is zero and the pointer position remains unchanged. The digits 8 and 9 will stop the evaluation if TECO’s current radix is octal. |
| *n*\\ | The backslash command preceded by an argument inserts the value of *n* into the edit buffer at the current position of the pointer, leaving the pointer positioned after the last digit of the insertion. The insertion is either signed decimal, unsigned octal, or unsigned hexadecimal. Note that \\ is a "bidirectional" command. *n*\\ inserts a string into text while \\ without a numeric argument returns a numeric result. |