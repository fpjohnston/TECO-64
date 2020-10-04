### EI - Indirect File Commands

EI*mung*$
- Opens *mung* as an indirect command file, so that any further
TECO requests for terminal input will come from this file. At
end-of-file, or upon TECO’s receipt of any error message, the
indirect command file will be closed and terminal input will be
switched back to the terminal. Note that this command only
presets where input will come from; it does not "splice" the
file’s data into the current command string (unless the E1&64
flag bit is set).

- While end-of-file closes the indirect command file, it does
not automatically start execution of commands. Execution
will begin only upon TECO’s receipt of two adjacent <ESC>s.
For instance, assume that FOO.TEC contains valid TECO
commands, that it presets a return to your terminal with the
customary EI$, but that it does not contain an <ESC><ESC>
pair. If you type EI*foo*$$, TECO will read in the contents
of *foo.tec* (building a command string) and attempt
to continue building the command string by reading from
your terminal when end of file on *foo.tec* is encountered.
Things will appear "very quiet" until it occurs to you to type
<DELIM><DELIM> and so begin execution of the (composite)
command string.

- All commands encountered in the indirect file will have their
normal TECO meaning (as opposed to any immediate action
meaning). For example, a <CTRL/U> encountered in an
indirect file will not erase the command line in which it occurs.
Instead, it will be treated as the TECO ^Uqtext$ command.
The only exception to this rule is the <ESC><ESC> command,
which directs TECO to execute the preceding command string
and then return to the indirect file at the point following the
<ESC><ESC>. (We can say <ESC> explicitly here, because
<DELIM> can be other than ESCape only in commands typed
at the terminal.)

@EI/*mung*/
- Equivalent to EI*mung*$

:EI*mung*$
- Equivalent to EI*mung*$, but returns -1 if the file could be opened,
and 0 if it could not be opened.

:@EI/*mung*/
- Equivalent to :EI*mung*$

EI$
- If an indirect command file is active, this command will close it
and resume terminal input from the terminal. Any portion of
the file after a double <DELIM> which has not yet been read
is discarded. This command has no effect if no indirect file is
already open.