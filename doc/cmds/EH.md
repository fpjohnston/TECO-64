### EH - Help Level Flag

The help level flag controls the printing of error
messages and failed commands. (See also the / command.)

| Bits | Function |
| ---- | -------- |
| EH&3 | If the low two bits of EH are 1, error messages are output in abbreviated form ("?XXX"). If they are equal to 2, error messages are output in normal form ("?XXX Message"). If they are equal to 3, error messages are output in long or "War and Peace" form, that is, a paragraph of informative material is typed following the normal form of the error message. |
| EH&4 | If this bit of EH is set, the failing command is also output up to and including the failing character in the command followed by a question mark. (Just like TECO’s response to the typing of a question mark immediately after an error.) |

The initial value of the EH flag is 0 which is equivalent to a value of 2.