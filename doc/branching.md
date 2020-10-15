## TECO-64 - Branching Commands

TECO provides an unconditional branch command and a set of conditional
execution commands. To branch within a command string, you must be able to
name locations inside the command string. TECO permits location tags of
the form

!*tag*!

to be placed between any two commands in a command string. The name *tag*
will be associated with this location when the command string is executed.
Tags may contain any number of ASCII characters and any character except
an exclamation mark. (When using the @ form of this command, any character
except the delimiter is legal.)

Since tags are ignored by TECO except when a branch command references
the tagged location, they may also be used as comments within complicated
command strings.

The unconditional branch command is the O command which has the form:

O*tag*\`

where *tag* is a location named elsewhere in the command string and "`" signifies
a \<DELIM\>. When an O command is executed, the next command to be executed
will be the one that follows the tag referenced by the O command. Command
execution continues normally from this point.

Use of the O command is subject to two restrictions. First, if an O command is
stored in a Q-register as part of a command string which is to be executed by
an M command, the tag referenced by the O command must reside in the same
Q-register.

Second, an O command which is inside a command loop may not branch to a
tagged location preceding the command loop. However, it is always possible to
branch out of a command loop to a location which follows the command loop and
then branch to the desired tag.

The text argument in the O command has the same format as the text
arguments in the search and E commands. String building characters such
as ^EQq can be embedded within the string. Also, the O command may be
@-sign modified. In that case, the syntax of the command would be @O/*tag*/.

Branching into a conditional poses no problems, but branching into a command
loop will causes unpredictable results.

Tags may contain any character sequence that does not include control
characters. This is also true for O command text arguments, except
that commas are not allowed, since those are required for computed GOTOs.
However, to allow for improved legibility, leading and trailing spaces
are allowed for tags in computed GOTO lists. A consequence of this is
that it is not possible to branch to a tag that has any spaces in it.
This is one way that tags can be distinguish from comments.

There are many other branching commands. Most of these are considerably
faster than the O command and should be used wherever convenient. They are
all described in the table below.


| Command | Function |
| ------- | -------- |
| @O*tag*\` | Causes TECO to branch to the first occurrence of the specified label *tag* in the current macro level. Branching to the left of the start of the current iteration is not permitted, and this command will only look for an occurrence of the specified tag following the \< of the current iteration, if you are in an iteration. In any case, branching out of an iteration is poor programming practice. Command execution resumes at the first character after the delimiter terminating the specified tag. Using this syntax, any character except \<ESC\> is permitted in the tag specification. The usual string build characters are permitted when specifying the tag. |
| @O/*tag*/ | Equivalent to O*tag*`. The usual string build characters are permitted when specifying the tag. |
| *n*O*tag0*,*tag1*, *tag2*,...\` | Causes TECO to branch to the tag specified by the *n*th tag in the accompanying list. The string argument to this command consists of a sequence of tags separated by commas. The tags may contain any characters other than comma or \<ESC>; however, good programming practice suggests that the tags should consist only of letters and digits. If *n* is out of range, or if *n* selects a null tag (signified by two adjacent commas), then command execution continues with the first command following the \<DELIM\> that delimits this command. |
| *n*O/*tag0*,*tag1*, *tag2*,.../ | Same as the preceding command except that the list of tags is bracketed by a delimiter shown here as "/". The delimiter can be any character other than comma that does not appear within the list of tags. |
| \; | Causes TECO to branch out of the current iteration, if the immediately preceding search (or search and replace) command failed. In that case, control resumes at the character following the matching \> at the end of the current iteration. On the other hand, if the preceding search succeeded, command execution continues with the character following the ;. If this command is encountered from outside of an iteration (in the current macro level), then the ?SNI error message is issued. |
| n; | Causes TECO to branch out of the current iteration if the value of *n* is greater than or equal to 0. In that case, command execution resumes at the character following the matching \> at the end of the current iteration. On the other hand, if *n* is less than 0, command execution continues with the character following the ;. If this command is encountered from outside of an iteration (in the current macro level), then the ?SNI error message is issued. |
| :; | Causes TECO to branch out of the current iteration if the immediately preceding search (or search and replace) command succeeded. In that case, control resumes at the character following the matching \> at the end of the current iteration. On the other hand, if the preceding search failed, command execution continues with the character following the ;. If this command is encountered from outside of an iteration (in the current macro level), then the ?SNI error message is issued. |
| *n*:; | Causes TECO to branch out of the current iteration if the value of *n* is less than 0. In that case, command execution resumes at the character following the matching \> at the end of the current iteration. On the other hand, if *n* is greater than or equal to 0, command execution continues with the character following the ;. If this command is encountered from outside of an iteration (in the current macro level), then the ?SNI error message is issued. |
| ’ | This "command" is actually part of the syntax of TECO conditionals. It has no affect if "executed" other than to signify termination of the current conditional level. |
| \| | Causes control to branch to the end of the conditional. Command execution resumes with the character following the ’ that ends the current conditional with the ELSE clause being skipped. |
| \> | Causes TECO to bump the current iteration count by 1 and test to see if the resulting count is equal to the maximum count permitted for the iteration (specified as an argument before the matching \<). If the iteration count has not expired, then control returns to the command following the \< at the beginning of the current iteration. If the iteration has expired, then command execution continues with the character following the \>. If this command is encountered outside of an iteration (within the current macro level), then the ?BNI error message is issued. |
| F\> | Causes TECO to branch (flow) to the end of the current iteration. TECO effectively resumes execution at the matching \>. The iteration count is tested as usual. If it has not expired, control returns back to the start of the iteration with the count having been incremented by 1. If the count was up, the iteration is exited and control continues with the first command after the \>. If this command is encountered outside of an iteration, it has the same effect as the \<DELIM\>\<DELIM\> command. |
| F\< | Causes TECO to branch (flow) to the start of the current iteration. TECO effectively resumes execution at the first command following the \< at the beginning of the current iteration. The iteration count is not affected. If this command is issued outside of an iteration, it causes TECO to branch back to the start of the current command string (in the current macro level). |
| F’ | Causes TECO to branch (flow) to the end of the current conditional. TECO effectively resumes execution at the first command following the ’ at the end of the current conditional. Numeric arguments are eaten up by this command. If this command is issued while not in a conditional, the ?MAP error (or its equivalent) is issued. |
| F\| | Causes TECO to branch (flow) to the else clause of the current conditional. TECO effectively resumes execution at the first command following the |\ at the end of the current THEN clause. If the current conditional has no ELSE clause, or if an unmatched ’ is encountered before an unmatched \|,  then control resumes at the command following the ’ . Numeric arguments are eaten up by this command. If this command is issued while not in a conditional, the ?MAP error is issued. Well-structured programs should not need to use this command. |
| $$ | The \<ESC\>\<ESC\> command causes TECO to exit from the current macro level. If this command is issued from top level (not from within a macro), then the command string execution is terminated and TECO returns to prompt level. Note that the second \<ESC\> must be a true ESCape and may not be a ^[. Also, note that both ESCapes must be true TECO commands and not part of the syntax of some previous command. That is, the first \<ESC\> does not count if it is the delimiting ESCape of a string. Finally, note that these must be real ESCape characters, and not the user-designated \<DELIM\> character (which TECO understands as an ESCape only when typed at the terminal). |
| *n*$$ | This command causes TECO to exit from the current macro level, returning the number *n* as a value. This value will be used as the numeric argument to the first command following the macro call. |
| *m*,*n*$$ | This command causes TECO to exit from the current macro level, returning the pair of values *m* and *n* as arguments to the first command following the macro call. Good programming practice dictates that all ways of exiting a macro return the same number of arguments. |
| ^C | Causes command execution to stop and control return to TECO’s prompt. No clean-up of push-down lists, flag settings, etc. is done. This command lets a macro abort TECO’s command execution. |
| ^C\<CTRL/C\> | Causes TECO to unconditionally abort and control exits from TECO. Control returns to the operating system. The second \<CTRL/C\> must be a true \<CTRL/C\> and may not be a Caret-C. |
