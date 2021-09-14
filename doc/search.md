## TECO-64 - Search Commands

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
end of the edit buffer (or in the case of backwards searches, until the beginning of
the buffer). A bounded search, however, will only search from the current position
to the specified bound limit. If the search string is found within the bound limits,
the pointer is positioned immediately after the last character in the string. If the
string cannot be found, the pointer is left unchanged.

A special case of the bounded search occurs when the upper and lower bound
limits are the same. In such a case, the search command is called an anchored
search, and is used to compare the search argument against the character string
immediately following the edit buffer pointer.

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
| n_*text*` | This command searches for the *n*th occurrence of the specified character string, where *n* must be greater than zero. It is identical to the _ command in other respects. *n* can be negative, in which case the search proceeds backwards through each edit buffer and through the file being edited. It terminates upon the correct search string match and/or beginning of file. A -*n*:_ command returns 0 at beginning of file. |
| E_*text*` | Same as _*text*` command except that effective EY (rather than Y) commands are used. Thus, this command is never aborted and is not controlled by the Yank protection bit in the ED flag. Remember that Yank DESTROYS the current buffer; there’s no way to get it back! |
| nE_*text*` | Same as *n*_*text*` command except that effective EY (rather than Y) commands are used. |

### Bounded Search Commands

| Command | Function |
| ------- | -------- |
| *m*,*n*S*text*` | Performs the same function as the *n*S command, but *m* serves a bound limit for the search. If the search string can be found without moving the pointer more than ABS(*m*)-1 places, the search succeeds and the pointer is repositioned to immediately after the last character of the string. Otherwise, the pointer is left unchanged. The ^Q operator is useful in conjunction with this command. <br><br>Note that m,Stext$ is identical to m,1Stext$ and m,-Stext$ is identical to m,-1Stext$.|
| *m*,*n*FB*text*` | Performs the same function as the *n*S*text*` command, but *m* and *n* (inclusive) serve as bounds for the search. In order for a search to be successful, the first character to match must occur between buffer pointer positions *m* and *n*. The string that is matched is permitted to extend beyond the search limits specified, provided that it begins within bounds. If *m* \< *n*, then the search proceeds in a forwards direction. If *m* \> *n*, then the search proceeds in the reverse direction. |
| *n*FB*text*` | Performs a bounded search over the next *n* lines. If *n* is positive, the search proceeds forward over the next *n* lines; if *n* is negative the search proceeds backwards over the *n* preceding lines; if *n* is zero, the search proceeds backwards over the portion of the line preceding the pointer. |
| FB*text*` | Equivalent to 1FB*text*`. |
| -FB*text*` | Equivalent to -1FB*text*`. |
| ::S*text*` | Compare command. The ::S command is not a true search. If the characters in the buffer immediately following the current pointer position match the search string, the pointer is moved to the end of the string and the command returns a value of -1; i.e., the next command is executed with an argument of -1. If the characters in the buffer do not match the string, the pointer is not moved and the command returns a value of 0. Identical to ".,.:FB*text*`". |

### Search and Replace Commands

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

### Search String Building

TECO builds the search string by loading its search string buffer from the
supplied search command argument. To help you enter special characters or
frequently used character sequences, the argument may contain special string
building characters. The table below  lists the string building characters
and their functions.

Note that, as explained [here](conventions.md), a caret (up-arrow) may be used
to indicate that the character following it is to be treated as a control
character. Any of the commands below may be entered using the caret. This
function of the caret can be disabled by using the ED flag.

| Command | Function |
| ------- | -------- |
| \<CTRL/Q\> | Specifies that the character following the \<CTRL/Q\> is to be used literally rather than as a match control character. |
| \<CTRL/R\> | Equivalent to \<CTRL/Q\>. |
| \<CTRL/V\>\<CTRL/V\> | Convert all following alphabetic characters in this string are to be converted to lower case unless an explicit \<CTRL/W\> is given to temporarily override this state. This state continues until the end of the string or until a \<CTRL/W\> construct is encountered. |
| \<CTRL/V\>*x* | Convert *x* to lower case. |
| \<CTRL/W\>\<CTRL/W\> | Convert all following alphabetic characters in this string to upper case unless an explicit \<CTRL/V\> is encountered to temporarily override this state. This state continues until the end of the string or until a \<CTRL/V\>\<CTRL/V\> construct is encountered. |
| \<CTRL/W\>*x* | Convert *x* to upper case. |
| \<CTRL/E\>Q*q* | Specifies that the string stored in Q-register *q* is to be used in the position occupied by the ^EQ*q* in the search string. |
| \<CTRL/E\>U*q* | Specifies that the character whose ASCII code is specified by the numeric storage area of Q-register *q* is to be used in the position occupied by the ^EU*q* in the search string. |

String build characters are also permitted inside the string arguments
of the O, EB, ER, EW, and EG commands.

### Match Control Characters

TECO executes a search command by attempting to match the search command
argument character-by-character with some portion of the input file. There
are several special control characters that may be used in search command
arguments to alter the usual matching process. The following table
lists these match control characters and their functions.

| Command | Function |
| ------- | -------- |
| \<CTRL/X\> | Specifies that this position in the character string may be any character. TECO accepts any character as a match for \<CTRL/X\>. |
| \<CTRL/S\> | Specifies that any separator character is acceptable in this position. TECO accepts any character that is not a letter (upper or lower case A to Z) or a digit (0 to 9) as a match for \<CTRL/S\>. |
| \<CTRL/N\>x | TECO accepts any character as a match for the \<CTRL/N\>x combination EXCEPT the character which follows the \<CTRL/N\>. \<CTRL/N\> can be combined with other special characters. For example, the combination \<CTRL/N\>\<CTRL/E\>D means match anything except a digit in this position. |
| \<CTRL/E\>A | Specifies that any alphabetic character (upper or lower case A to Z) is acceptable in this position.|
| \<CTRL/E\>B | Equivalent to \<CTRL/S\>. |
| \<CTRL/E\>C | Specifies that any symbol constituent character is acceptable in this position. TECO accepts any letter (upper or lower case A to Z), any digit (0 to 9), a dot (.), a dollar sign ($), or an underscore (_) as a match for \<CTRL/E\>C. |
| \<CTRL/E\>D | Specifies that any digit (0 to 9) is acceptable in this position. |
| \<CTRL/E\>G*q* | Specifies that any character contained in Qregister *q* is acceptable in this position. For example, if Q-register A contains "A\*:" then TECO accepts either A, \*, or : as a match for \<CTRL/E\>GA. |
| \<CTRL/E\>L | Specifies that any line terminator (line feed, vertical tab, or form feed) is acceptable in the position occupied by \<CTRL/E\>L in the search string. |
| \<CTRL/E\>R | Specifies that any alphanumeric character (letter or digit as defined above) is acceptable in this position. |
| \<CTRL/E\>S | Specifies that any non-null string of spaces and/or tabs is acceptable in the position occupied by \<CTRL/E\>S. |
| \<CTRL/E\>V | Specifies that any lower case alphabetic character is acceptable in this position. |
| \<CTRL/E\>W | Specifies that any upper case alphabetic character is acceptable in this position. |
| \<CTRL/E\>X | Equivalent to \<CTRL/X\>. |
| <nobr>\<CTRL/E\>\<*nnn*\></nobr> | Specifies that the character whose ASCII decimal code is *nnn* is acceptable in this position. |
