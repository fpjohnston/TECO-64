### CTRL/U - Add/Delete Q-Register Text

^U*qstring*$
- This command inserts character string *string* into the text storage
area of Q-register *q*. When entering a command string from the terminal,
you must specify ^U using the caret/U format, since the <CTRL/U> character
is the line erase immediate action command.

:^U*qstring*$
- This command appends character string *string* to the text storage area
of Q-register "q".

*n*^U*q*$
- This form of the ^U*q*$ command inserts the single character whose ASCII
code is *n* into the text storage area of Q-register *q*. (*n* is taken
modulo 256.

*n*:^U*q*$
- This form of the :^U*q*$ command appends the single character whose ASCII
code is *n* to the text storage area of Q-register *q*. (n is taken modulo
256.

@^U*q*/*string*/
- Equivalent to ^U*qstring*$.

:@^U*q*/*string*/
- Equivalent to :^U*qstring*$.

*n*@^U*q*//
- Equivalent to *n*^U*q*$.

*n*:@^U*q*//
- Equivalent to *n*:^U*q*$.

^U*q*$
- Delete all text in Q-register *q*.

@^U*q*//
- Equivalent to ^U*q*$.

