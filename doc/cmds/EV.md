### EV - Edit Verify Flag

The  edit verify flag is decoded just like the ES flag. Just before
TECO prints its prompt, the EV flag is checked. If it is
non-zero the lines to be viewed are printed on the terminal.

EV
- Current value of the EV flag.

*n*EV
- Set the EV flag. *n* may be zero or non-zero; if non-zero, EV is set to -1.

The initial value of the EV flag is 0.