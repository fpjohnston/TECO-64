## TECO-64 – New & Enhanced Features

This document describes the features that are either new to TECO-64,
or which have been copied or enhanced from earlier TECOs.

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

- An at sign is used to modify a command that does not allow it.
- More than one at sign is used to modify a command.

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

[^A - Print text](typeout.md)
- Added colon modifier for adding CR/LF after text.

[^E - Match control constructs](search.md)
- Added ^E*nnn*. (TECO-10)

[^K - Reset display colors](immediate.md)

[^Q - Convert line numbers to character values](variables.md) (TECO-10)

[^T - Type character](typeout.md)
- Added repeat count.
- Allowed -1 for printing CR/LF.

[^U - Add/Delete Q-register text](qregister.md)
- Added ability to delete text.

[^V - Lower case text](misc.md) (TECO-10)

[^W - Upper case text](misc.md) (TECO-10)

[^X- Search mode flag](flags.md)
- Flag may be set to 1, in addition to 0 and -1.

[; - Branch out of current iteration](branching.md)
- Allowed whitespace preceding semi-colon.

[= - Print numeric value](typeout.md)
- Added ability to use *printf()* format string.

[@ - Text argument modifier](basics.md)
- Restricted delimiters to graphic ASCII characters.
- Allowed whitespace between command and delimiter.

[C - Move Pointer Forward](dot.md)
- Added colon modifier to return success/failure value. (TECO-10)

[D - Delete Characters](delete.md)
- Added colon modifier to return success/failure value. (TECO-10)

[E% - Write file from Q-register](file.md) (TECO-10)

[E1 - Extended Features Flag](flags.md)

[E2 - Command Restrictions Flag](flags.md)

[E3 - File Operations Flag](flags.md)

[E4 - Display Mode Flag](flags.md)

[EC - Set Memory Size](misc.md) (TECO-10)

[EI - Indirect File Command](file.md)
- Added colon modifier to return success/failure value.

[EJ - Get environment information](env.md)
- Changed returned values, and added new ones.

[EL - Open/close log file](file.md) (TECO-10)

[EO - TECO version number](variables.md)
- Updated version number of 200, and added minor and patch versions.

[EQ - Read file to Q-register](file.md) (TECO-10)

[EW - Open output file](file.md)
- Added colon modifier to return success/failure value.

[EY - Yank backwards](page.md)
- Added optional code to allow backwards paging (TECO-32).

[F1 - Set command window colors](display.md)

[F2 - Set text window colors](display.md)

[F3 - Set status line colors](display.md)

[FD - Search and delete](search.md) (TECO-10)

[FK - Search and delete](search.md) (TECO-10)

[FL - Lower case text](misc.md)

[FM - Map keys to macros](display.md)

[FQ - Map keys to Q-registers](display.md)

[FU - Upper case text](misc.md)

[I - Insert string](insert.md)
- Added repeat count.

[J - Jump to position](move.md)
- Added colon modifier to return success/failure value. (TECO-10)

[L - Line count](variables.md)
- Added colon modifier to return line count.

[N - Non-stop search](search.md)
- Allowed backwards searching if virtual memory code included. (TECO-32)

[P - Write page to file](page.md)
- Allowed backwards paging if virtual memory code included. (TECO-32)

[R - Move pointer backward](move.md)
- Added colon modifier to return success/failure value. (TECO-10)

[X - Load Q-register from edit buffer](qregister.md)
- Added ability to delete text from Q-register.

[Y - Yank page](page.md)
- Allowed backwards yanking if virtual memory code included. (TECO-32)

[{} - Braced text arguments](basics.md)
