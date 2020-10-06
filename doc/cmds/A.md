### A - Append Lines

A
- Appends the next page of the input file to the contents of the
text buffer, thus combining the two pages of text on a single
page with no intervening form feed character. This command
takes no argument. To perform n append commands, use the n<A>
construct. Note that nA is a completely different command.

:A
- Executes the A command, but also returns non-zero (-1) if the
append succeeded, and zero if the append failed because
end-of-file for the input file had previously been reached
(^N flag is -1 at start of this command).

*n*:A
- Appends n lines of text from the input file to the contents of
the text buffer. The value of *n* must not be negative. Non-zero (-1)
is returned if the command succeeded, and zero is returned if
end-of-file on the input file was encountered before all *n* lines
were read in. Note that the command can succeed and yet read in fewer
than *n* lines in the case that the text buffer fills up.