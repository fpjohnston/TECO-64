## TECO-64 - Q-Register Commands

### Q-Register Overview

TECO provides data storage registers, called Q-registers, each of
which may be used to store both a signed integer and a character
string that can be loaded, stored, and used by the commands
described below.

### Macros

A character string stored in a Q-register is called a *macro*, which
allows it to be called by other commands as a subroutine, using the
M command. Numeric arguments may be passed to and returned from macros,
and macros may be nested.

When TECO is executing a command typed at the prompt, it is at
macro level 0. When an M command is then executed, TECO is at
macro level 1, and if that macro calls itself or another macro,
the macro level is further incremented. This is relevant in the
discussion of the global and local Q-registers below.

### Global Q-Registers

There are 36 global Q-registers, of which has a one-character name,
A through Z, and 0 through 9. Lower case names may also be used and
are equivalent to the corresponding upper case names. Global
Q-registers are available at all macro levels.

### Local Q-Registers

There are additionally 36 local Q-registers, each of which has a
two-character name: .A through .Z and .0 through .9. Effectively,
there is a complete and unique set of local Q-registers available
to each and every macro level, including prompt level.

TECO automatically saves and restores a given macro level’s local
Q-registers whenever a new macro is invoked, by saving the current
local Q-registers and and creating a new set for the next macro
level. When that macro exits, its local Q-registers are destroyed
and those of the calling macro level are restored. (If a fatal error
occurs and TECO returns to prompt level, local Q-registers from all
macro levels are destroyed and prompt level’s set is restored.)

### Q-Register Push-Down List

The Q-register pushdown list is a stack that permits the numeric
and text storage areas of Q-registers to be saved (using the "[" command)
and restored (using the "]" command). The command string "[A ]B" replicates
the text string and numeric value from Q-register A into Q-register
B. Note that macros may be able to avoid the use of the push-down
list through the use of local Q-registers.

### Alternate Command Forms

The ^U commands can be modified with at signs, as shown in the table below.

| Command | Description |
| ------- | -------- |
| ^U*qtext*\` | Classic form of command, using accent grave (or possibly ESCape) as a delimiter for the file name. |
| @^U*q*/*text*/ | Equivalent to ^U*qtext* \`, except that a pair of slashes are used to delimit the file name. Other characters may be used instead of slashes, as long as they match. If the E1&4 flag bit is set, then the text string may be enclosed in paired braces (e.g., @^U*q*\{*text*\}). |

In the descriptions below, the at sign form of ^U commands will be used
for clarity.

### Q-Register Loading Commands

The table below lists the commands which permit numbers and strings to be
loaded into Q-registers.

| Command | Function |
| ------- | -------- |
| *n*U*q*   | Put *n* in the numeric storage area of Q-register q. |
| *n*:U*q*   | Equivalent to *n*U*q*, but uses a default of 0 if *n* is not provided. This requires that the E1&256 flag bit also be set. This can be used in macros to determine whether an *n* argument was specified by the caller. |
| *m*,*n*U*q* | Equivalent to *n*U*qm*. That is, this command puts the number *n* into the numeric storage area of Q-register *q* and then returns the number *m* as a value. The command "UA UB" is useful at the beginning of a macro to save the two arguments specified on the macro call. (See the *m*,*n*M*q* command below.) |
| *m*,*n*:U*q* | Equivalent to *m*,*n*U*q*, but uses defaults of 0 if *m* and *n* are not provided. This requires that the E1&256 flag bit also be set. This can be used in macros to determine whether *m* and *n* arguments were specified by the caller. |
| *n*%*q* | Add *n* to the contents of the number storage area of Q-register *q*. The updated contents of Q-register *q* are also returned as a value to be passed to the next command. If your intent is only to update the Q-register, use the *n*:%*q* command, described below, to avoid unintentionally affecting the following command. |
| *n*:%*q* | Equivalent to *n*%*q*, but discards the returned value. |
| *n*%*q*` | Same as *n*:%*q*. Note that the "^[" sequence may be substituted for a delimiter in this situation. |
| %*q* | Equivalent to 1%*q*. |
| :%*q* | Equivalent to 1:%*q*. |
| @^U*q*/*text*/ | This command inserts character string *text* into the text storage area of Q-register *q*. When entering a command string from the terminal, you must specify ^U using the caret/U format, since the <CTRL/U> character is the line erase immediate action command. |
| :@^U*q*/*text*/ | This command appends character string *text* to the text storage area of Q-register *q*. |
| *n*^U*q* | Inserts one character, whose ASCII code is *n* (module 256), into the text area of Q-register *q*. |
| *n*:^U*q* | Appends the character, whose ASCII code is *n* (module 256), into the text area of Q-register *q*. |
| @^U*q*// | Delete all text in Q-register *q*. |
| *n*X*q* | Clear Q-register *q* and copy *n* lines into it, where *n* is a signed integer. <br><br>If *n* is positive, the *n* lines following the current pointer position are copied into the text storage area of Qregister *q*. <br><br>If *n* is negative, the *n* lines preceding the pointer are copied. <br><br>If *n* is zero, the contents of the buffer from the beginning of the line on which the pointer is located up to the pointer is copied. <br><br>The pointer is not moved. The text is not deleted. |
| -X*q* | Equivalent to -1X*q*. |
| *m*,*n*X*q* | Copy the contents of the buffer from the *m*+1th character through and including the *n*th character into the text storage area of Q-register *q*. *m* and *n* must be positive, and *m* should be less than *n*. |
| .,.+*n*X*q* | Copy the *n* characters immediately following the buffer pointer into the text storage area of Q-register *q*. *n* should be greater than zero. |
| .-*n*,.X*q* | Copy the *n* characters immediately preceeding the buffer pointer into the text storage area of Q-register *q*. *n* should be greater than zero. |
| *n*:X*q* | Append *n* lines to Q-register *q*, where *n* is a signed integer with the same functions as *n* in the nX*q* command above. The pointer is not moved. <br><br>The colon construct for appending to a Q-register can be used with all forms of the X command. |
| 0,0X*q* | Delete any text string in Q-register *q*. |
| ]*q* | Pop from the Q-register push-down list into Q-register *q*. Any previous contents of Q-register *q* are destroyed. Both the numeric and text parts of the Q-register are loaded by this command. The Q-register push-down list is a last-in first-out (LIFO) storage area. (See above for a description of the push-down list.) This command does not use or affect numeric values. Numeric values are passed through this command transparently. This allows macros to restore Q-registers and still return numeric values. |
| :]*q* | Execute the ]*q* command and return a numeric value. A -1 indicates that there was another item on the Q-register push-down list to be popped. A 0 indicates that the Q-register push-down list was empty, so Q-register *q* was not modified. |
| \**q* | Save last-typed command string in Q-register *q*, as described [here](action.md). |

### Q-Register Retrieval Commands

The table below lists the commands which permit data to be retrieved from
Q-registers.

| Command | Function |
| ------- | -------- |
| Q*q* | Use the integer stored in the number storage area of Q-register *q* as the argument of the next command. |
| *n*Q*q* | Return the ASCII value of the (*n*+1)th character in Q-register *q*. The argument *n* must be between 0 and the Q-register’s size minus 1. If *n* is out of range, a value of -1 is returned. Characters within a Q-register are numbered the same way that characters in the edit buffer are numbered. The initial character is at character position 0, the next character is at character position 1, etc. Therefore, if Q-register A contains "xyz", then 0QA will return the ASCII code for "x" and 1QA will return the ASCII code for "y". |
| :Q*q* | Use the number of characters stored in the text storage area of Q-register *q* as the argument of the next command. |
| G*q* | Copy the contents of the text storage area of Q-register *q* into the edit buffer at the current position of *dot*, leaving the pointer positioned after the last character copied. |
| :G*q* | Print the contents of the text storage area of Q-register *q* on the terminal. Neither the edit buffer nor *dot* are changed by this command. |
| G\* | Copy the last filename specification into the edit buffer at the current position of *dot*, leaving the pointer positioned after the last character copied. |
| :G\* | Print the last filename specification on the terminal. Neither the edit buffer nor *dot* are changed by this command. |
| G_ | Copy the last search string buffer into the edit buffer at the current position of *dot*, leaving the pointer positioned after the last character copied. |
| :G_ | Print the search string buffer on the terminal. Neither the edit buffer nor *dot* are changed by this command. |
| G+ | Copy the output of the last ::EG command into the edit buffer at the current position of *dot*, leaving the pointer positioned after the last character copied. |
| :G+ | Print the output of the last ::EG command on the terminal. Neither the edit buffer nor *dot* are changed by this command. |
| [*q* | Copy the contents of the numeric and text storage areas of Q-register *q* into the Q-register push-down list. This command does not alter either the numeric or text storage areas of Qregister *q*. It does not use or affect numeric values. Numeric values are passed through this command transparently, allowing macros to save temporary Q-registers and still accept numeric values. (Note, however, macros written to use local Q-registers may be able to avoid saving and restoring Q-registers via the the pushdown list.) The command sequence [A ]B replicates the text string and numeric value from Q-register A into Q-register B. |

### Macro Invocation Commands

The table below lists the commands which cause macros (strings stored in
Q-registers) to be executed. Macro invocations can be nested recursively;
the limit is set by the amount of pushdown storage TECO has available.
In this table only, a distinction is made between a global Q-register name
(indicated below by "q") and a local Q-register name (indicated below by ".q").
Elsewhere in this manual, "q" indicates either a global or local Q-register name.

| Command | Function |
| ------- | -------- |
| M*q* | Execute the contents of the text storage area of global Qregister *q* as a command string. A new set of local Q-registers is created before the macro is invoked. |
| *n*M*q* | Execute the M*q* command as above, using *n* as a numeric argument for the first command contained in global Q-register *q*. A new set of local Q-registers is created. |
| *m*,*n*M*q* | Execute the M*q* command as above, using *m*,*n* as numeric arguments for the first command contained in global Q-register *q*. A new set of local Q-registers is created. |
| :M*q* | Execute the contents of the text storage area of global Qregister *q* as a command string. The current set of local Q-registers remains available to the invoked macro; no new set is created. |
| *n*:M*q* | Execute the :M*q* command as above, using *n* as a numeric argument for the first command contained in global Q-register *q*. No new set of local Q-registers is created. |
| *m*,*n*:M*q* | Execute the :M*q* command as above, using *m*,*n* as numeric arguments for the first command contained in global Q-register *q*. No new set of local Q-registers is created. |
| M.*q*, :M.*q* | Execute the contents of the text storage area of local Q-register .*q* as a command string. No new set of local Q-registers is created. |
| *n*M.*q*, *n*:M.*q* | Execute the M.*q* command as above, using *n* as a numeric argument for the first command contained in local Q-register .*q*. No new set of local Q-registers is created. |
| *m*,*n*M.*q*, *m*,*n*:M.*q* | Execute the M.*q* command as above, using *m*,*n* as numeric arguments for the first command contained in local Q-register .*q*. No new set of local Q-registers is created. |
| $$ | The \<ESC\>\<ESC\> command causes TECO to exit from the current macro level. If this command is issued from top level (not from within a macro), then the command string execution is terminated and TECO returns to prompt level. Note that the second \<ESC\> must be a true ESCape and may not be a ^[. Also, note that both ESCapes must be true TECO commands and not part of the syntax of some previous command. That is, the first \<ESC\> does not count if it is the delimiting ESCape of a string. Finally, note that these must be real ESCape characters, and not the user-designated \<DELIM\> character (which TECO understands as an ESCape only when typed at the terminal). |
| *n*$$ | This command causes TECO to exit from the current macro level, returning the number *n* as a value. This value will be used as the numeric argument to the first command following the macro call. |
| *m*,*n*$$ | This command causes TECO to exit from the current macro level, returning the pair of values *m* and *n* as arguments to the first command following the macro call. Good programming practice dictates that all ways of exiting a macro return the same number of arguments. |
