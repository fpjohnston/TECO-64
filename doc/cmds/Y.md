### Y - Yank Page

Y
- Clears the text buffer and then reads the next page of the input
file into the buffer. Because the Y command DESTROYS the
current text buffer, its use can result in the loss of data, and is
subject to "Yank protection" (see ED flag).

-Y
- Clears the text buffer and then reads the previous page of the
file being edited back into the buffer. Because the Y command
DESTROYS the current text buffer, its use can result in the
loss of data, and is subject to "Yank protection" (see ED flag).

:Y
- Same as the Y command but a value is returned. -1 is returned
if the Yank succeeded. 0 is returned if the Yank failed because
the end-of-file had been reached on the input file prior to the
initiation of this command. [Not in TECO-8]

-:Y
- Same as the -Y command but a value is returned. -1 is
returned if the Yank succeeded. 0 is returned if the Yank
failed because the beginning of file had been reached on the file
being edited prior to the initiation of this command.
