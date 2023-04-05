## TECO-64 - Arithmetic Operators and Expressions

The numeric argument of a TECO command may consist of:

- A single integer
- [One of TECO's numeric variables](variables.md)
- The numeric contents of a Q-register
- An arithmetic expression combining these elements

Arithmetic expressions are evaluated from left to right without any
operator precedence.

Parentheses may be used to override the normal order of evaluation
of an expression. If parentheses are used, all operations within
the parentheses are performed, left to right, before operations outside the
parentheses. Parentheses may be nested, in which case the innermost expression
contained by parentheses will be evaluated first.

The table below lists all of the arithmetic operators that may be
used in arithmetic expressions.

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
| ( ) | 3*(42/16)=6 | Expression grouping. |

### Extended Operators

If the E1&1 flag bit is set, then there are additional operators that can be used, as long as they are within parentheses (to avoid confusion with commands that may use the same characters). Note that any of the multi-character operators must be entered as a single token, with no intervening whitespace.

| Command | Example | Function |
| ------- | --------| -------- |
| ==      | (1==1)=-1   | Relational comparison for equality. |
| &lt;>    | (1&lt;>1)=0  | Relational comparison for inequality. |
| &lt;      | (1&lt;1)=0    | Relational comparison for less than. |
| &lt;=     | (1&lt;=1)=-1  | Relational comparison for less than or equal. |
| >      | (1>1)=0    | Relational comparison for greater than. |
| >=     | (1>=1)=0   | Relational comparison for greater than or equal. |
| //      | (7//3)=1    | Binary division yielding remainder. |
| !       | (!0)=-1     | Bitwise logical NOT. |
| \~      | (7~3)=4     | Bitwise logical XOR. |
| &lt;&lt;    | (1<<3)=8    | Arithmetic left shift. |
| >\>    | (16>>2)=4   | Arithmetic right shift. |
