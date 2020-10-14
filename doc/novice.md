## TECO-64 - Novice Commands

The table below describes a set of commands that can be used by a novice
user to edit files.
For simplicity, no attempt is made here to describe all of the modifiers
and variations that may be used with each of these, nor is any information
included regarding errors that might occur, and how to respond to them.
More detailed descriptions are presented in the Command Reference
sections that follow.


| Command | Function |
| ------- | -------- |
| . | The current position of *dot*. |
| = | Print a numeric value on the user's terminal. |
| B | The value of *dot* before the first character in the buffer. Always 0. |
| C | Move *dot* one character forward in the edit buffer. |
| D | Delete the character to the right of *dot*. |
| -D | Delete the character to the left of *dot*. |
| EC | Write out the edit buffer, and any remaining data in the input file, to the output file, then clear the edit buffer and close both files. |
| ER*filespec*\` | Open an existing file for input. |
| EW*filespec*\` | Create a new file, or overwrite an existing one. |
| EX | Same as EC, but then exits from TECO. |
| FS*text1*\`*text2*\` | Search forward for the next occurrence of *text1* and replace it with *text2*. |
| <nobr>-FS*text1*\`*text2*\`</nobr> | Same as -FS, but search backward. |
| I*text*\` | Insert *text* into the edit buffer at the current position of *dot*. |
| J | Move *dot* to the start of the buffer. |
| K | Delete the contents of the edit buffer from *dot* through the next line terminator. |
| -K | Delete the contents of the edit buffer from the start of the previous line terminator through *dot*. |
| HK | Delete the entire contents of the edit buffer. |
| L | Move *dot* to the start of the next line. |
| -L | Move *dot* to the start of the previous line. |
| P | Write out the contents of the edit buffer to the output file, and then read in the next page from the input file. |
| R | Move *dot* one character backward in the edit buffer. |
| S*text*\` | Search forward for *text* in the edit buffer, starting at *dot*. |
| -S*text*\` | Same as S, but search backward. |
| T | Type the contents of the edit buffer from *dot* through the next line delimiter. |
| -T | Type the contents of the edit buffer from the start of the previous line through *dot*. |
| HT | Type the entire contents of the edit buffer. |
| V | Type the current line that *dot* is on. |
| Y | Read the first page of data from the input file into the edit buffer. If any data exists in the edit buffer, it is overwritten. |
| Z | The value of *dot* after the last character in the buffer. Equal to the number of characters in the buffer. |

The C, D, K, L, P, R, S, T, and V commands may all be preceded by a repeat
count that is either positive or negative. For example, 5C means to move 
*dot* forward 5 characters.

The J command may be preceded by a positive number in the range of B to Z
inclusive. So BJ (or 0J) moves *dot* before the first character in the
edit buffer, and ZJ moves it after the last character.

### Examples

Example 1:
- Open new output file. [EW]
- Print size of edit buffer. [Z=]
- Insert text. [I]
- Print size of edit buffer to see number of characters added. [Z=]
- Write out the contents of the edit buffer. [EC]

```
teco
*EWhello1.txt``
*Z=``
0
*Ihello, world!
``
*Z=``
14
*EC``
```

Example 2:
- Same as before, but combine all commands in a single command string,
omitting the commands to print the value of Z. Resultant file should
be identical to the one created in Example 1.

```
teco
*EWhello2.txt` Ihello, world!
` EC ``
```

Example 3:
- Open input file created in Example 1. [ER]
- Read text into the edit buffer. [Y]
- Type out the edit buffer. [HT]
- Move *dot* down one line. [L]
- Insert "hello again, world!". [I]
- Type out the edit buffer again.. [HT]
- Jump to start of edit buffer. [J]
- Print current line. [V]
- Jump to end of buffer. [ZJ]
- Print previous line. [-V]
- Open new output file. [EW]
- Write out buffer and close files. [EC]
- Exit TECO. [EX]

```
teco
*ERhello1.txt``
*Y``
*HT``
hello, world!
*L``
*Ihello again, world!
``
*HT``
hello, world!
hello again, world!
*J``
*T``
hello, world!
*ZJ``
*-T``
hello again, world!
*EWhello3.txt``
*EC``
*EX``
```

Example 4:
- Open the file created in Example 2. [ER]
- Open new output file. [EW]
- Read text into the edit buffer. [Y]
- Type out the edit buffer. [HT]
- Search for "hello, ". [S]
- Delete previous 5 characters. [-5D]
- Insert "goodbye". [I]
- Move forward 2 characters. [skip past ", "). [2C]
- Insert "cruel ". [I]
- Type out the edit buffer again. [HT]
- Write out buffer and close files. [EC]

```
teco
*ERhello2.txt``
*EWhello4.txt``
*Y``
*HT``
hello, world!
*Shello``
*-5D``
*Igoodbye``
*2C``
*Icruel ``
*HT``
goodbye, cruel world!
*EC``
```

Example 4:
- Open new output file. [EW]
- Open input file created in Example 5. [ER]
- Type out the edit buffer. [HT]
- Insert "I don't know why you say " [I]
- Search for ", " [S]
- Kill remainder of line [K]
- Insert "I say hello." [I]
- Type out the edit buffer again. [HT]
- Write out buffer and exit TECO. [EX]

```
teco
*EWhello5.txt``
*ERhello4.txt``
*Y``
*HT``
goodbye, cruel world!
*II don't know why you say ``
*S, ``
*K``
*II say hello.
``
*HT``
I don't know why you say goodbye, I say hello.
*EC``
```

Example 5:
- Open input file created in Example 1. [ER]
- Open new output file. [EW]
- Read text into the edit buffer. [Y]
- Search for "world!" and replace with "it's me." [FS]
- Type out the edit buffer. [HT]
- Write out buffer and exit TECO. [EX]

```
teco
*ERhello1.txt``
*EWhello6.txt``
*Y``
*FSworld!`it's me``
*HT``
hello, it's me.
*EX``
```

Example 6:
- Open input file created in Example 4. [ER]
- Open new output file. [EW]
- Read text into the edit buffer. [Y]
- Type out the first line. [V]
- Delete entire buffer. [HK]
- Insert the first four lines of Jabberwocky. [I]
- Write out buffer and exit TECO. [EX]

```
teco
*ERhello5.txt``
*EWhello7.txt``
*Y``
*V``
I don't know why you say goodbye, I say hello.
*HK``
*I'Twas brillig, and the slithy toves
did gyre and gimble in the wabe.
All mimsy were the borogoves,
and the mome raths outgrabe.
``
*EX``
```

