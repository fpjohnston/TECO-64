## TECO-64 - GOTO Commands

TECO provides for unconditional branches, equivalent to GOTOs in other
programming languages, using the O command, which has the following form:

O*tag*\`

where *tag* is a location named elsewhere in the command string and *`*
signifies a &lt;*delim*>.
The text argument in the O command has the same format as the text
arguments in search commands, which includes the use of
string building characters such as ^EQq.
Also, the O command may be modified with an at-sign, in which case
the syntax of the command would be @O/*tag*/.

Note that leading and trailing spaces are ignored for tags specified in O
commands.
This is to improve readability.
However, a consequence of this is that it is not possible to branch
to a tag that has any spaces in it.
This is one way that tags can be distinguished from comments.

When an O command is executed, the next command to be executed will be the
tag referenced by the O command, followed by any subsequent commands.

Tags have the form:

!*tag*!

TECO performs no action on encountering a tag, and basically just treats
them as no-ops, but can be divided into the following categories:
- Labels that are the targets of O commands.
- Comments that are ignored by TECO.

Labels may contain any number of ASCII characters, but are subject to the
following restrictions:
- Labels must be unique (this is not the case for comments which are not
the targets of O commands).
- Control characters are not allowed.
- Exclamation marks are not allowed, since they delimit tags.
- Commas are not allowed, since they delimit labels in computed
GOTO commands).

The following conditions apply to the use to O commands:
- Branching out of a loop or conditional statement is allowed.
- Branching into a conditional statement is allowed.
- The tag branched to must be in the same command string,
Q-register, or indirect command file as the O command.
- The tag must occur only once (this only applies to labels referenced by
O commands; comments may be duplicated without causing an error).
- If an O command is inside a loop or nested loop, it cannot branch backward
before the start of the outermost enclosing loop.
- Branches into the middle of a new loop, whether forward or backward, are not
allowed.

| Command | Function |
| ------- | -------- |
| @O*tag*\` | Causes TECO to branch to the first occurrence of the specified label *tag* in the current macro level. Command execution resumes at the start of the tag. |
| @O/*tag*/ | Equivalent to O*tag*`. |
| *n*O*tag0*,*tag1*, *tag2*,...\` | Causes TECO to branch to the tag specified by the *n*th tag in the accompanying list. The string argument to this command consists of a sequence of tags separated by commas. If *n* is out of range, or if *n* selects a null tag (signified by two adjacent commas), then command execution continues with the first command following the &lt;*delim*> that terminates this command. |
| *n*O/*tag0*,*tag1*, *tag2*,.../ | Same as the preceding command except that the list of tags is bracketed by a delimiter shown here as "/". The delimiter can be any character other than comma that does not appear within the list of tags. |
