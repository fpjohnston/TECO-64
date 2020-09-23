### N - Non-Stop Searching

N*text*$
- Performs the same function as the S command except that
the search is continued across page boundaries, if necessary,
until the character string is found or the end of the input file
is reached. This is accomplished by executing an effective P
command after each page is searched. If the end of the input
file is reached, an error message is printed and it is necessary
to close the output file and re-open it as an input file before
any further editing may be done on that file. The N command
will not locate a character string which spans a page boundary.

*n*N*text*$
- This command searches for the *n*th occurrance of the specified
character string, where *n* must be greater than zero. It is
identical to the N command in other respects.

-N*text*$
- Performs the same function as the -S command except that
the search is continued (backwards) across page boundaries, if
necessary, until the character string is found or the beginning
of the file being edited is reached.

-*n*N*text*$
- This command searches (backwards) for the *n*th occurrence
of the specified character string. It is identical to the -N
command in other respects.
