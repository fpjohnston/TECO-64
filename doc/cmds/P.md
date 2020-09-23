### P - Write Page to File

P
- Writes the contents of the buffer onto the output file, then
clears the buffer and reads the next page of the input file into
the buffer. A form feed is appended to the output file if the last
page read in (with a P, Y, or A command) was terminated with
a form feed.

:P
- Same as the P command except that a value is returned. -1 is
returned if the command succeeded. 0 is returned if
the command failed because the end-of-file on the input file
had been reached prior to the initiation of this command. The
command string <:P;> takes you to end of file.

*n*P
- Executes the P command *n* times, where *n* must be a non-zero
positive integer. If backwards paging is enabled, *n* can be a
negative integer (and -P means -1P). A -*n*P command will back up
*n* pages.

*n*:P
- Executes the :P command *n* times, where n must be a non-zero
positive integer. If backwards paging is enabled, *n* can be a
negative integer (and -:P means -1:P). This command returns 0
when another page could not be backed up because beginning of file
had been reached.

PW
- Write the contents of the buffer onto the output file and append
a form feed character. The buffer is not cleared and the pointer
position remains unchanged.

*n*PW
- Executes the PW command *n* times, where *n* must be a non-zero
positive integer.

*m*,*n*PW
- Writes the contents of the buffer between pointer positions *m*
and *n* onto the output file. *m* and *n* must be positive integers.
A form feed is not appended to this output, nor is the buffer
cleared. The pointer position remains unchanged.

*m*,*n*P
- Equivalent to *m*,*n*PW.

HPW
- Equivalent to the PW command except that a form feed
character is not appended to the output.

HP
- Equivalent to HPW.
