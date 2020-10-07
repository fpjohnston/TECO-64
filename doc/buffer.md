### Buffer Pointer Manipulation Commands

All of the buffer pointer manipulation commands are listed below.
These commands may be used to move *dot* to a position between any two
characters in the buffer, but they will not move *dot* across a buffer
boundary. If any R or C command attempts to move *dot* backward beyond
the beginning of the buffer or forward past the end of the buffer, the command is
ignored and an error message is printed. If any L command attempts to exceed
the buffer boundaries, *dot* is positioned at the boundary which would
have been exceeded and no error message is printed.

#### Character commands

[C - Move pointer forward](cmds/C.md)

#### Jump commands

[J - Jump to position](cmds/J.md)

#### Line commands

[L - Move line](cmds/L.md)

#### Reverse commands

[R - Move pointer backward](cmds/R.md)

