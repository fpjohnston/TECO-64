### X - Load Q-Register From Text Buffer

*n*X*q*
- Clear Q-register *q* and move *n* lines into it, where *n* is a signed
integer. If *n* is positive, the *n* lines following the current
pointer position are copied into the text storage area of Q-register *q*.
If *n* is negative, the *n* lines preceding the pointer
are copied. If *n* is zero, the contents of the buffer from the
beginning of the line on which the pointer is located up to the
pointer is copied. The pointer is not moved. The text is not
deleted.

X*q*
- Equivalent to 1X*q*.

-X*q*
- Equivalent to -1X*q*.

*m*,*n*X*q*
- Copy the contents of the buffer from the *m*+1th character
through and including the *n*th character into the text storage
area of Q-register *q*. *m* and *n* must be positive, and *m* should
be less than *n*.

.,.+*n*X*q*
- Copy the *n* characters immediately following the buffer pointer
into the text storage area of Q-register *q*. *n* should be greater
than zero.

.-*n*,.X*q*
- Copy the *n* characters immediately preceeding the buffer
pointer into the text storage area of Q-register *q*. *n* should be
greater than zero.

*n*:X*q*
- Append *n* lines to Q-register *q*, where *n* is a signed integer
with the same functions as *n* in the *n*X*q* command above. The
pointer is not moved.

- The colon construct for appending to a Q-register can be used
with all forms of the X command.

0,0X*q*
- Delete any text string in Q-register *q*.
