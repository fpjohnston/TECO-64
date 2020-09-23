### CTRL/A - Print text

^A*text*<CTRL/A>
- Types *text* on the terminal. While the command may begin with <CTRL/A> or ^A,
closing character must be a <CTRL/A>. A numeric argument must not be specified
with this command.

@^A/*text*/
- Equivalent to the ^A command except that the text to be printed may be
bracketed with any character. This avoids the need for the closing <CTRL/A>.

:^A*text*<CTRL/A>
- Equivalent to ^A*text*<CTRL/A> 13^T 10^T.

:@^A/*text*/
- Equivalent to ^A/*text*/ 13^T 10^T.
