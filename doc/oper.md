## TECO-64 - Arithmetic Operators and Expressions

The numeric argument of a TECO command may consist of:

- A single integer
- [One of TECO's numeric variables](variables.md)
- The numeric contents of a Q-register
- An arithmetic expression combining these elements

TECO-64 provides all of the standard arithmetic operators as classic TECO, but
additionally provides a set of extended operators, as described in the
following two sections.

### Standard Operators

| Command | Example     | Function |
| ------- | ----------- | -------- |
| \+      | \+2=2       | Unary plus. |
| \+      | 5+6=11      | Binary addition. |
| \-      | \-2=-2      | Unary minus (2&apos;s complement negation). |
| \-      | 8-2=6       | Binary subtraction. |
| \*      | 8\*2=16     | Binary multiplication. |
| \/      | 8/3=2       | Binary division yielding quotient. |
| \&      | 12&10=8     | Bitwise logical AND. |
| \#      | 12#10=14    | Bitwise logical OR. |
| \^_     | 5\^_=-6     | Unary 1&apos;s complement. Note that it is used *after* an expression. |
| ( )     | 3*(42/16)=6 | Expression grouping. |

### Extended Operators

If the E1&1 flag bit is set, then there are additional operators that can be
used, as long as they are within parentheses (to avoid confusion with commands
that may use the same characters).
Note that any of the multi-character operators must be entered as a single
token, with no intervening whitespace.

Also, TECO-64 uses a value of 0 for true and -1 for false, as shown in the examples
for relational and logical NOT operators.
This is in keeping with classic TECO, which considered a numeric value as true if
it had all bits set, and false if it had no bits set.
This is why colon-modified commands return values of either 0 or -1.

| Command  | Example      | Function |
| -------- | ------------ | -------- |
| ==       | (1==1)=-1    | Relational comparison for equality. |
| &lt;>    | (1&lt;>1)=0  | Relational comparison for inequality. |
| &lt;     | (1&lt;1)=0   | Relational comparison for less than. |
| &lt;=    | (1&lt;=1)=-1 | Relational comparison for less than or equal. |
| >        | (1>1)=0      | Relational comparison for greater than. |
| >=       | (1>=1)=0     | Relational comparison for greater than or equal. |
| //       | (7//3)=1     | Binary division yielding remainder. |
| !        | (!0)=-1      | Bitwise logical NOT. |
| \~       | (7~3)=4      | Bitwise logical XOR. |
| &lt;&lt; | (1<<3)=8     | Arithmetic left shift. |
| >\>      | (16>>2)=4    | Arithmetic right shift. |

### Operator Precedence and Associativity

In TECO-64, arithmetic expressions may evaluated in one of two ways.
If the E1&2048 flag bit is clear, expressions are evaluated from left to right
without any operator precedence, as they are in classic TECO.
However, if the E1&2048 flag bit is set, operators have the same precedence and
associativity in C, as described in the following table, which lists
operators from highest to lowest precedence.

| Operator | Precedence | Associativity | Function |
| -------- | ---------- | ------------- | -------- |
| ( )      |      1     |     Left      | Expression grouping. |
| !        |      2     |     Right     | Bitwise logical NOT. |
| \^_      |      2     |     Left      | Unary 1&apos;s complement. |
| \-       |      2     |     Right     | Unary minus (2&apos;s complement negation). |
| \+       |      2     |     Right     | Unary plus. |
| \*       |      3     |     Left      | Binary multiplication. |
| \/       |      3     |     Left      | Binary division yielding quotient. |
| //       |      3     |     Left      | Binary division yielding remainder. |
| \+       |      4     |     Left      | Binary addition. |
| \-       |      4     |     Left      | Binary subtraction. |
| &lt;&lt; |      5     |     Left      | Arithmetic left shift. |
| >\>      |      5     |     Left      | Arithmetic right shift. |
| &lt;     |      6     |     Left      | Relational comparison for less than. |
| &lt;=    |      6     |     Left      | Relational comparison for less than or equal. |
| >        |      6     |     Left      | Relational comparison for greater than. |
| >=       |      6     |     Left      | Relational comparison for greater than or equal. |
| ==       |      7     |     Left      | Relational comparison for equality. |
| &lt;>    |      7     |     Left      | Relational comparison for inequality. |
| \&       |      8     |     Left      | Bitwise logical AND. |
| \~       |      9     |     Left      | Bitwise logical XOR. |
| \#       |     10     |     Left      | Bitwise logical OR. |


Parentheses may be used to override the normal order of evaluation
of an expression.
If parentheses are used, all operations within the parentheses are performed,
before operations outside the parentheses, according to the precedence and
associativity rules in effect at that point.
Parentheses may be nested, in which case the innermost expression
contained by parentheses will be evaluated first.
