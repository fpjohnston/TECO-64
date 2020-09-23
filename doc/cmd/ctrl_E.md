### CTRL/E - Match Control Constructs

^EA
- ^EA indicates that any alphabetic character (upper or lower case A to Z)
is acceptable in this position.

^EB
- Same as <CTRL/S>.

^EC
- ^EC indicates that any character that is legal as part of a symbol
constituent is acceptable in this position. TECO accepts any letter
(upper or lower case A to Z), any digit (0 to 9), or a dot (.) as a
match fo ^EC. Additional characters may be matched depending on the
settings of the E1 flag.

^ED
- ^ED indicates that any digit (0 to 9) is acceptable in
this position.

^EG*q*
- ^EG*q* indicates that any character contained in Q-
register *q* is acceptable in this position. For example, if
Q-register A contains "A ? :" then TECO accepts either A, ? ,
or : as a match for ^EGA.

^EL
- ^EL indicates that any line terminator (line feed,
vertical tab, or form feed) is acceptable in the position occupied
by ^EL in the search string.

^ER
- ^ER indicates that any alphanumeric character (letter
or digit as defined above) is acceptable in this position.

^ES
- ^ES indicates that any non-null string of spaces and/or
tabs is acceptable in the position occupied by ^ES.

^EV
- ^EV indicates that any lower case alphabetic character
is acceptable in this position.

^EW
- ^EW indicates that any upper case alphabetic
character is acceptable in this position.

^EX
- Equivalent to <CTRL/X>.

^E*nnn*
- Indicates that the character whose ASCII octal code is *nnn* is acceptable
in this position.
