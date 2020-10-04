### EW - Edit Write or Switch Output Stream

The EW command performs two functions. The first, when a text argument follows the
command, means that the user wants to open an output file.
The second, if there is no text argument, means that the user wants to
switch to the primary output stream.

EW*foo*$
- Opens the specified file (*foo* in the example here) for output
on the currently selected output stream. Any existing file is overwritten
(this can be undone with an EK command before the file is closed). It is
an error to try to open the following:
    - A device, directory, pipe, or anything other than a regular file.
    - A file lacking the required write permissions.

@EW/*foo*/
- Equivalent to EW*foo*$

:EW*foo*$
- Executes the EW command, but also returns non-zero (-1) zero if the file was
successfully opened, and zero if it could not be opened.

:@EW/*foo*/
- Equivalent to :EW*foo*$

EW$
- Switch to primary output stream. This is only needed after an EA command
has been executed to switch to the secondary output stream.

@EW//
- Equivalent to EW$.
