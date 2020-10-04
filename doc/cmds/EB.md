### EB - Edit Backup

EB*foo*$
- This command is recommended for most editing jobs. It opens the specified
file (*foo* in the example here) for input on the currently selected input
stream and for output on the currently selected output stream.
The EB command also keeps the unmodified file (the latest copy of the input
file) available to the user; details of this process are system dependent.

    - On Linux, the name of the backup file is formed by appending a tilde
(\~) to the original file name. So, using the example command above, the
backup file would be *foo*\~.

It is an error to try to open any of the following:
    - A non-existent input file (but see description below of the colon
modifier).
    - A device, directory, pipe, or anything other than a regular file.
    - A file lacking the required read or write permissions.

@EB/*foo*/
- Equivalent to EB*foo*$.

:EB*foo*$
- Executes the EB command, but also returns non-zero (-1) if the file was
successfully opened, and zero if the specified file could
not be found. Any error condition other than a simple failure to find the
file will generate an error message and terminate command execution.

:@EB/*foo*/
- Equivalent to :EB*foo*$.
