## TECO-64 - Error Messages

<style>
    span {
        white-space: nowrap;
    }
</style>

If TECO is unable to execute a command, either because of a syntax,
system, or other error, it aborts execution of the command, prints an
error message, ignores all following commands, resets its state, and
prints its prompt to indicate that the user can enter new commands.

All error messages are of the form:

?XXX *message*

where XXX is an error code and *message* is a description of the error.
Depending on the setting of the EH flag, more information may also be
printed.

Some messages mention a specific character or string of characters
associated with the error. In these error messages, TECO represents
any non-printing special characters as described [here](conventions.md).

TECO can also print warning messages. These do not abort the
command being executed.

| Message | Description |
| ------- | ----------- |
| <span>\%Superseding existing file</span> | The file specified to an EW command already exists, and the old copy will be deleted if the file is closed with an EC or EX command. The EK command may be used to undo the EW command. |
| <span>\%Search failure in loop</span> | A search command has failed inside a loop. A ; (semi-colon) command immediately following the search command can be used to suppress this message. After printing the message, the iteration is terminated, i.e., TECO simulates an F> command. |

The following error messages are listed alphabetically by their three-letter
code.

| Error | Message            | Description |
| ----- | ------------------ | ----------- |
/* (INSERT: ERROR CODES) */

