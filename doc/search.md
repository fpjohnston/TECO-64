### Search Commands

In many cases, the easiest way to position the buffer pointer is by means of a
character string search. Search commands cause TECO to scan through text
until a specified string of characters is found, and then position the buffer pointer
at the end of the string. A character string search begins at the current position
of the pointer. It proceeds within the current buffer in a forward or a reverse
direction or through the file in a forward direction. Specifying a negative numeric
argument to the search command causes the search to proceed backwards from
the pointer.

Your last explicitly specified search string is always remembered by TECO.
If a search command is specified with a null search string argument, the last
explicitly defined search string will be used. This saves having to retype a
complex or lengthy search string on successive search commands.
Normally searches are "unbounded" - they search from the current position to the
end of the text buffer (or in the case of backwards searches, until the beginning of
the buffer). A bounded search, however, will only search from the current position
to the specified bound limit. If the search string is found within the bound limits,
the pointer is positioned immediately after the last character in the string. If the
string cannot be found, the pointer is left unchanged.

A special case of the bounded search occurs when the upper and lower bound
limits are the same. In such a case, the search command is called an anchored
search, and is used to compare the search argument against the character string
immediately following the text buffer pointer.

| Command | Function |
| ------- | -------- |
| S*text*` |  Where *text* is a string of characters terminated by a \<DELIM\>. This command searches the *text* buffer for the next occurrence of the specified character string following the current position of the buffer pointer. If the string is found, the pointer is positioned after the last character in the string. If it is not found, the pointer is positioned immediately before the first character in the buffer (i.e., a 0J is executed) and an error message is printed. |
| nS*text*` | This command searches for the *n*th occurrence of the specified character string, where *n* is greater than zero. It is identical to the S command in other respects. |
| -*n*S*text*` | Identical to "*n*S*text*`" except that the search proceeds in the reverse direction. If the string is not found, the pointer is positioned immediately before the first character in the buffer and an error message is printed. If the pointer is positioned at the beginning of or within an occurrence of the desired string, that occurrence is considered to be the first one found. Upon successful completion, the pointer is positioned after the last character in the string found. |
| -S*text*` | Equivalent to -1S*text*`. |
| N*text*` | Performs the same function as the S command except that the search is continued across page boundaries, if necessary, until the character string is found or the end of the input file is reached. This is accomplished by executing an effective P command after each page is searched. If the end of the input file is reached, an error message is printed and it is necessary to close the output file and re-open it as an input file before any further editing may be done on that file. The N command will not locate a character string which spans a page boundary. |
| *n*N*text*` | This command searches for the *n*th occurrence of the specified character string, where *n* must be greater than zero. It is identical to the N command in other respects. |
| -N*text*` | Performs the same function as the -S command except that the search is continued (backwards) across page boundaries, if necessary, until the character string is found or the beginning of the file being edited is reached. |
| -*n*N*text*` | This command searches (backwards) for the *n*th occurrence of the specified character string. It is identical to the -N command in other respects. |
| _*text*` | The underscore command is identical to the N command except that the search is continued across page boundaries by executing effective Y commands instead of P commands, so that no output is generated. Since an underscore search can result in the loss of data, it is aborted under the same circumstances as the Y command (see the ED flag). |
| n_*text*` | This command searches for the *n*th occurrence of the specified character string, where *n* must be greater than zero. It is identical to the _ command in other respects. *n* can be negative, in which case the search proceeds backwards through each text buffer and through the file being edited. It terminates upon the correct search string match and/or beginning of file. A -*n*:_ command returns 0 at beginning of file. |
| E_*text*` | Same as _*text*` command except that effective EY (rather than Y) commands are used. Thus, this command is never aborted and is not controlled by the Yank protection bit in the ED flag. Remember that Yank DESTROYS the current buffer; there’s no way to get it back! |
| nE_*text*` | Same as *n*_*text*` command except that effective EY (rather than Y) commands are used. |

#### Bounded Search Commands

| Command | Function |
| ------- | -------- |
| *m*,*n*S*text*` | System specific command. |
| *m*,*n*FB*text*` | Performs the same function as the *n*S*text*` command, but *m* and *n* (inclusive) serve as bounds for the search. In order for a search to be successful, the first character to match must occur between buffer pointer positions *m* and *n*. The string that is matched is permitted to extend beyond the search limits specified, provided that it begins within bounds. If *m* \< *n*, then the search proceeds in a forwards direction. If *m* \> *n*, then the search proceeds in the reverse direction. |
| *n*FB*text*` | Performs a bounded search over the next *n* lines. If *n* is positive, the search proceeds forward over the next *n* lines; if *n* is negative the search proceeds backwards over the *n* preceding lines; if *n* is zero, the search proceeds backwards over the portion of the line preceding the pointer. |
| FB*text*` | Equivalent to 1FB*text*`. |
| -FB*text*` | Equivalent to -1FB*text*`. |
| ::S*text*` | Compare command. The ::S command is not a true search. If the characters in the buffer immediately following the current pointer position match the search string, the pointer is moved to the end of the string and the command returns a value of -1; i.e., the next command is executed with an argument of -1. If the characters in the buffer do not match the string, the pointer is not moved and the command returns a value of 0. Identical to ".,.:FB*text*`". |

#### Search and Replace Commands

The search and replace commands listed below perform equivalent functions to
the search commands listed next to them, but then delete *text1* and replace it
with *text2*.

| Search & Replace      | Search Command |
| --------------------- | -------------- |
| FS*text1*\`*text2*`    | S*text1*`      |
| *n*FS*text1*\`*text2*` | *n*S*text1*`   |
| FN*text1*\`*text2*`    | N*text1*`      |
| *n*FN*text1*\`*text2*` | *n*N*text1*`   |
| F_*text1*\`*text2*`    | _*text1*`      |