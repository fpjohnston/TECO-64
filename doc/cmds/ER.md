### ER - Edit Read or Switch Input Stream

The ER command performs two functions. The first, when a text argument follows the
command, means that the user wants to open an input file.
The second, if there is no text argument, means that the user wants to
switch to the primary input stream.

ER*foo*$
- Opens the specified file (*foo* in the example here) for input
on the currently selected input stream. It is an error to try to open any of
the following:
    - A non-existent file (but see description below of the colon modifier).
    - A device, directory, pipe, or anything other than a regular file.
    - A file lacking the required read permissions.

@ER/*foo*/
- Equivalent to ER*foo*$

:ER*foo*$
- Executes the ER command, but also returns non-zero (-1) if the file was
successfully opened, and zero if the specified file could
not be found. Any error condition other than a simple failure to find the
file will generate an error message and terminate command execution.

:@ER/*foo*/
- Equivalent to :ER*foo*$

ER$
- Switch to primary input stream. This is only needed after an EP command
has been executed to switch to the secondary input stream.

@ER//
- Equivalent to ER$.
