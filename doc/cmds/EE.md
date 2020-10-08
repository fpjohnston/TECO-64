### EE - Alternate Command Delimiter

EE
- Current value of EE flag.

*n*EE
- Set EE flag. When the flag is zero, and unless the 8192 ET bit is
set, TECO recognizes only the ESCape as its \<DELIM\>
character from the terminal. If it is desired to use another
character as an ESCape surrogate (as when working from a
terminal lacking an ESCape key), the ASCII value of that
other character may be set in the EE flag. Note that when an
ESCape surrogate is set, the designated character (when typed
at the terminal) is received by TECO as an ESCape — that
character is no longer directly available at the keyboard. When
an ESCape surrogate is active, an ESCape received by TECO
echoes as accent grave; when none is active, an ESCape received
by TECO echoes as a dollar sign. This feature is provided for the
benefit of newer terminals which lack an ESCape key. (8192ET
and 96EE both set \` as ESCape surrogate. EE, however, can
be used to designate a character other than accent grave.)

The EE flag is initially zero.