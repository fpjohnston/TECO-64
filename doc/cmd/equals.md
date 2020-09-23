### = - Print Numeric Value

*n*=
- This command causes the value of *n* to be output at the
terminal in decimal followed by a CR/LF.
Decimal numeric conversion is signed. TECO’s radix is
unaltered.

*n*:=
- Equivalent to *n*=, except that no CR/LF is output.

*n*==
- This command causes the value of n to be output at the
terminal in octal (base 8) followed by a CR/LF.
Octal numeric conversion is unsigned. TECO’s radix
is unaltered.

*n*:==
- Equivalent to *n*==, except that no CR/LF is output.

*n*===
- This command causes the value of *n* to be output at the
terminal in hexadecimal (base 16) followed by a CR/LF.
Hexadecimal output is unsigned. TECO’s
radix is unaltered.

*n*:===
- Equivalent to *n*===, except that no CR/LF is output.

*n*@=/*format*/
- Output the value of *n* using *format* as a *printf()* format string,
followed by a CR/LF.

- Any string for *printf()* may be used, as long as it contains at most
one numeric conversion specifier. Non-numeric specifiers such as %s,
or multiple numeric specifiers, are not allowed. However, %% may be used
to print a percent sign.

*n*:@=/*format*/
- Equivalent to *n*@=/*format*/, except that no CR/LF is added.
