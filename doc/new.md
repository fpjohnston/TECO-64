## TECO-64 – New & Enhanced Features

### General syntax changes

Whitespace may be used to make TECO-64 commands more readable. However,
unlike previous versions of TECO, whitespace may not occur within
anything considered a token, which includes digit strings and multi-character
commands such as ER or FS. Therefore, commands such as “1 2 3 =” are no longer
valid, and must instead be written as “123=” or “123 =”. Whitespace between
tokens is always valid.

Note that whitespace may now occur between a search or replace command
and any subsequent semi-colon command. Earlier versions of TECO required
that semi-colons immediately followed such commands, and printed a
NAS error if they did not. So the following are now valid:

    < @S/foo/ ; >

    < @FN/foo/baz/ ; >

If the E2&4 flag bit is set, an error will result if the following occurs:

- An at-sign is used to modify a command that does not allow it.
- More than one at-sign is used to modify a command.

If the E2&8 flag bit is set, an error will result if the following occurs:

- A colon is used to modify a command that does not allow it.
- A double colon is used to modify a command that does not allow it.
- More than two colons are used to modify a command.

Numeric expressions consist of either a single numeric value, called *n*, or
a pair of values, called *m* and *n*, separated by a comma. If a comma is seen,
then it must be preceded by an *m* argument, and must be followed by an *n*
argument. Multiple commas are not allowed.

Colons are allowed for all commands that open files, and will result in -1
being returned if the open succeeded, and 0 if it failed.

### Commands

[^A - Print text](cmds/ctrl_A.md)
- Added colon modifier for adding CR/LF after text.

[^E - Match control constructs](cmds/ctrl_E.md)
- Added ^E*nnn*. (TECO-10)

[^K - Reset display colors](cmds/ctrl_K.md)

[^Q - Convert line numbers to character values](cmds/ctrl_Q.md) (TECO-10)

[^T - Type character](cmds/ctrl_T.md)
- Added repeat count.
- Allowed -1 for printing CR/LF.

[^U - Add/Delete Q-register text](cmds/ctrl_U.md)
- Added ability to delete text.

[^V - Lower case text](cmds/ctrl_V.md) (TECO-10)

[^W - Upper case text](cmds/ctrl_W.md) (TECO-10)

[^X- Search mode flag](cmds/ctrl_X.md)
- Flag may be set to 1, in addition to 0 and -1.

[; - Branch out of current iteration](cmds/semi.md)
- Allowed whitespace preceding semi-colon.

[= - Print numeric value](cmds/equals.md)
- Added ability to use *printf()* format string.

[@ - Text argument modifier](cmds/atsign.md)
- Restricted delimiters to graphic ASCII characters.
- Allowed whitespace between command and delimiter.

[C - Move Pointer Forward](cmds/C.md)
- Added colon modifier to return success/failure value. (TECO-10)

[D - Delete Characters](cmds/D.md)
- Added colon modifier to return success/failure value. (TECO-10)

[E% - Write file from Q-register](cmds/E_pct.md) (TECO-10)

[E1 - Extended Features Flag](cmds/E1.md)

[E2 - Command Restrictions Flag](cmds/E2.md)

[E3 - File Operations Flag](cmds/E3.md)

[E4 - Display Mode Flag](cmds/E4.md)

[EC - Set Memory Size](cmds/EC.md) (TECO-10)

[EI - Indirect File Command](cmds/EI.md)
- Added colon modifier to return success/failure value.

[EJ - Get environment information](cmds/EJ.md)
- Changed returned values, and added new ones.

[EL - Open/close log file](cmds/EL.md) (TECO-10)

[EO - TECO version number](cmds/EO.md)
- Updated version number of 200, and added minor and patch versions.

[EQ - Read file to Q-register](cmds/EQ.md) (TECO-10)

[EW - Open output file](cmds/EW.md)
- Added colon modifier to return success/failure value.

[EY - Yank backwards](cmds/EY.md)
- Added optional code to allow backwards paging (TECO-32).

[F1 - Set command window colors](cmds/F1.md)

[F2 - Set text window colors](cmds/F2.md)

[F3 - Set status line colors](cmds/F3.md)

[FD - Search and delete](cmds/FD.md) (TECO-10)

[FK - Search and delete](cmds/FK.md) (TECO-10)

[FL - Lower case text](cmds/FL.md)

[FU - Upper case text](cmds/FU.md)

[I - Insert string](cmds/I.md)
- Added repeat count.

[J - Jump to position](cmds/J.md)
- Added colon modifier to return success/failure value. (TECO-10)

[L - Line count](cmds/L.md)
- Added colon modifier to return line count.

[N - Non-stop search](cmds/N.md)
- Allowed backwards searching if virtual memory code included. (TECO-32)

[P - Write page to file](cmds/P.md)
- Allowed backwards paging if virtual memory code included. (TECO-32)

[R - Move pointer backward](cmds/R.md)
- Added colon modifier to return success/failure value. (TECO-10)

[X - Load Q-register from edit buffer](cmds/X.md)
- Added ability to delete text from Q-register.

[Y - Yank page](cmds/Y.md)
- Allowed backwards yanking if virtual memory code included. (TECO-32)

[{} - Braced text arguments](cmds/braces.md)

### Error Messages

The following error messages have been added:

- ATS - Illegal at-sign, or too many at-signs
- BOA - Numeric argument for O command is out of range
- CHR - Invalid character for command
- COL - Illegal colon, or too many colons
- DIV - Division by zero
- DPY - Display mode initialization error
- DUP - Duplicate tag
- EGC - EG command is too long
- FIL - Illegal file
- IE1 - Illegal E1 command
- IFE - Ill-formed numeric expression
- IMA - Illegal m argument
- INA - Illegal n argument
- INI - Initialization error
- IOA - Illegal O argument
- MAT - No matching files
- NAT - Illegal n argument with text string
- NOD - Display mode not enabled
- NON - No n argument after m argument
- SYS - System error
