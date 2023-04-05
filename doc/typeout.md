## TECO-64 - Type-Out Commands

The commands are used to type out part or all of the contents of
the buffer for examination are listed below.

These commands do not move *dot*.

| Command | Function |
| ------- | -------- |
| \^A*text*&lt;CTRL/A> | Types *text* on the terminal. While the command may begin with &lt;CTRL/A> or \^A, closing character must be a &lt;CTRL/A>. |
| @^A/*text*/ | Equivalent to \^A*text*&lt;CTRL/A>. |
| :\^A*text*&lt;CTRL/A> | Equivalent to \^A*text*&lt;CTRL/A> 13\^T 10\^T. |
| :@\^A/*text*/ | Equivalent to \^A/*text*/ 13\^T 10\^T. |
| *n*^T | Types out to the terminal the character whose ASCII value is *n*. Whatever normal type-out conversions may currently be in effect and applicable (such as translation of control characters to up-arrow format) are done. The value of *n* is used modulo 256 (except if it is -1; see below). |
| *n*:^T | Outputs to the terminal the character whose ASCII value is *n*. Output is done in "one-shot" binary mode; no type-out translations are done. The value of *n* is used modulo 256 (except if it is -1; see below). |
| -1^T | Types CR/LF. Equivalent to 13^T 10^T. |
| *m*,*n*^T | Specifies a repeat count of *m* for the character whose ASCII value is *n*. *n* may be -1. |
| *n*= | This command causes the value of *n* to be output at the terminal in decimal followed by a carriage return and line feed. Decimal numeric conversion is signed. TECO's radix is unaltered. |
| *n*== | This command causes the value of *n* to be output at the terminal in octal (base 8) followed by a carriage return and line feed. Octal numeric conversion is unsigned. TECO’s radix is unaltered. |
| *n*=== | This command causes the value of *n* to be output at the terminal in hexadecimal (base 16) followed by a CR/LF. Hexadecimal output is unsigned. radix is unaltered. |
*n*:=, *n*:==, and *n*:=== | These commands are equivalent to *n*=, *n*==, and *n*===, respectively, except that no CR/LF is output. |
| *n*@=/*format*/ | Output the value of *n* using *format* as a *printf()* format string, followed by a CR/LF. Any string for *printf()* may be used, as long as it contains at most one numeric conversion specifier. Non-numeric specifiers such as %s, or multiple numeric specifiers, are not allowed. However, %% may be used to print a percent sign. Also, the output format is used regardless whether =, ==, or === is specified. |
| *n*:@=/*format*/ | Equivalent to *n*@=/*format*/, except that no CR/LF is added. |
| T | Types out the contents of the buffer from the current position of *dot* through and including the next line terminator character. |
| *n*T | Types *n* lines, as follows: <br><br>If *n* > 0, types the *n* lines following the current position of *dot*. <br><br>If *n* &lt; 0, types the *n* lines preceding *dot*. <br><br>If *n* = 0, types the contents of the buffer from the beginning of the line on which *dot* is located up to *dot*. |
| -T | Equivalent to -1T. |
| *m*,*n*T | Types out the contents of the buffer between pointer positions *m* and *n*. |
| .,.+*n*T | Types out the *n* characters immediately following *dot*. *n* should be greater than zero. |
| .-*n*,.T | Types the *n* characters immediately preceding *dot*. *n* should be greater than zero. |
| HT | Types out the entire contents of the buffer. |
| V | Types out the current line. Equivalent to 0TT. |
| *n*V | Types out *n*-1 lines on each side of the current line. Equivalent to 1-*n*T*n*T. |
| *m*,*n*V | Types out *m*-1 lines before and *n*-1 lines after the current line. |
