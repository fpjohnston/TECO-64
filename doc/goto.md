## TECO-64 - GOTO Commands

TECO provides an unconditional branch command. To branch within a command
string, you must be able to name locations inside the command string. TECO
permits location tags of the form

!*tag*!

to be placed between any two commands in a command string. The name *tag*
will be associated with this location when the command string is executed.
Tags may contain any number of ASCII characters and any character except
an exclamation mark. (When using the @ form of this command, any character
except the delimiter is legal.)

Since tags are ignored by TECO except when a branch command references
the tagged location, they may also be used as comments within complicated
command strings.

The unconditional branch command is the O command, which has the form:

O*tag*\`

where *tag* is a location named elsewhere in the command string and "`" signifies
a &lt;*delim*>. When an O command is executed, the next command to be executed
will be the one that follows the tag referenced by the O command. Command
execution continues normally from this point.

Use of the O command is subject to several restrictions. First, the tag referenced
by the command must be in the same command string, Q-register, or indirect
command file as the command itself. Second, the tag must occur only once.
This only applies to tags referenced by O commands; tags that are simply
comments may be duplicated without causing an error. Finally, although an O
command may be used to exit a loop or conditional, it cannot jump inside another
loop, nor can it jump inside a conditional.

The text argument in the O command has the same format as the text
arguments in the search and E commands. String building characters such
as ^EQq can be embedded within the string. Also, the O command may be
@-sign modified. In that case, the syntax of the command would be @O/*tag*/.

Branching into a conditional poses no problems, but branching into a command
loop will cause unpredictable results.

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
| @O*tag*\` | Causes TECO to branch to the first occurrence of the specified label *tag* in the current macro level. Branching to the left of the start of the current iteration is not permitted, and this command will only look for an occurrence of the specified tag following the &lt; of the current iteration, if you are in an iteration. In any case, branching out of an iteration is poor programming practice. Command execution resumes at the first character after the delimiter terminating the specified tag. Using this syntax, any character except &lt;ESC> is permitted in the tag specification. The usual string build characters are permitted when specifying the tag. |
| @O/*tag*/ | Equivalent to O*tag*`. The usual string build characters are permitted when specifying the tag. |
| *n*O*tag0*,*tag1*, *tag2*,...\` | Causes TECO to branch to the tag specified by the *n*th tag in the accompanying list. The string argument to this command consists of a sequence of tags separated by commas. The tags may contain any characters other than comma or &lt;ESC>; however, good programming practice suggests that the tags should consist only of letters and digits. If *n* is out of range, or if *n* selects a null tag (signified by two adjacent commas), then command execution continues with the first command following the &lt;*delim*> that delimits this command. |
| *n*O/*tag0*,*tag1*, *tag2*,.../ | Same as the preceding command except that the list of tags is bracketed by a delimiter shown here as "/". The delimiter can be any character other than comma that does not appear within the list of tags. |
