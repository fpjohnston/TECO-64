### TECO-64 Page Commands

The following commands permit text to be read into the text buffer from
an input file or written from the buffer onto an output file.

All of the input commands listed in this table assume that the input file
is organized into pages small enough to fit into available memory. If any
page of the input file contains more characters than will fit into
available memory, the input command will continue reading characters into
the buffer until a line feed is encountered when the buffer is about 3/4
full. Special techniques for handling pages larger than the buffer
capacity will be presented later in this chapter.

#### Append Commands

| Command | Function |
| ------- | -------- |
| A | Appends the next page of the input file to the contents of the text buffer, thus combining the two pages of text on a single page with no intervening form feed character. This command takes no argument. To perform n append commands, use the n\<A\> construct. Note that nA is a completely different command. |
| :A | Executes the A command, but also returns -1 if the append succeeded, and zero if the append failed because end-of-file for the input file had previously been reached (\^N flag is -1 at start of this command). |
| *n*:A | Appends *n* lines of text from the input file to the contents of the text buffer. The value of *n* must not be negative. -1 is returned if the command succeeded, and zero is returned if end-of-file on the input file was encountered before all *n* lines were read in. Note that the command can succeed and yet read in fewer than *n* lines in the case that the text buffer fills up. |

#### Page Commands

| Command | Function |
| ------- | -------- |
| P | Writes the contents of the buffer onto the output file, then clears the buffer and reads the next page of the input file into the buffer. A form feed is appended to the output file if the last page read in (with a P, Y, or A command) was terminated with a form feed. |
| :P | Same as the P command except that a value is returned. -1 is returned if the command succeeded. 0 is returned if the command failed because the end-of-file on the input file had been reached prior to the initiation of this command. The command string \<:P;\> takes you to end of file. |
| *n*P | Executes the P command *n* times, where *n* must be a non-zero positive integer. If backwards paging is enabled, *n* can be a negative integer (and -P means -1P). A -*n*P command will back up *n* pages. |
| *n*:P | Executes the :P command *n* times, where n must be a non-zero positive integer. If backwards paging is enabled, *n* can be a negative integer (and -:P means -1:P). This command returns 0 when another page could not be backed up because beginning of file had been reached. |
| PW | Write the contents of the buffer onto the output file and append a form feed character. The buffer is not cleared and the pointer position remains unchanged. |
| *n*PW | Executes the PW command *n* times, where *n* must be a non-zero positive integer. |
| *m*,*n*PW | Writes the contents of the buffer between pointer positions *m* and *n* onto the output file. *m* and *n* must be positive integers. A form feed is not appended to this output, nor is the buffer cleared. The pointer position remains unchanged. |
| *m*,*n*P | Equivalent to *m*,*n*PW. |
| HPW | Equivalent to the PW command except that a form feed character is not appended to the output. |
| HP | Equivalent to HPW. |

#### Yank Commands

| Command | Function |
| ------- | -------- |
| Y | Clears the text buffer and then reads the next page of the input file into the buffer. Because the Y command DESTROYS the current text buffer, its use can result in the loss of data, and is subject to "Yank protection" (see ED flag). |
| -Y | Clears the text buffer and then reads the previous page of the file being edited back into the buffer. Because the Y command DESTROYS the current text buffer, its use can result in the loss of data, and is subject to "Yank protection" (see ED flag). |
| :Y | Same as the Y command but a value is returned. -1 is returned if the Yank succeeded. 0 is returned if the Yank failed because the end-of-file had been reached on the input file prior to the initiation of this command. |
| -:Y | Same as the -Y command but a value is returned. -1 is returned if the Yank succeeded. 0 is returned if the Yank failed because the beginning of file had been reached on the file being edited prior to the initiation of this command. |
| EY | Same as the Y command, but its action is always permitted regardless of the value of the Yank Protection bit in the ED flag. Remember that Yank DESTROYS the current buffer; there’s no way to get it back! |
| :EY | Same as the :Y command, but its action is always permitted regardless of the value of the Yank protection bit in the ED flag. Remember that Yank DESTROYS the current buffer; there’s no way to get it back! |
| -EY | Same as the -Y command, but its action is always permitted. |
| -:EY | Same as the -:Y command, but its action is always permitted. |

#### Memory Commands

| Command | Function |
| ------- | -------- |
| *n*EC | *n*EC tells TECO to expand or contract until it uses *n*K bytes of memory for its text buffer. If this is not possible, then TECO’s memory usage does not change. The 0EC command tells TECO to use the least amount of memory possible. |
