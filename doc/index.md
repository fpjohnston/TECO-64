### Index of TECO Commands and Special Characters

| Character(s)   | Description |
| -------------- | ----------- |
| NUL            | Discarded on input; ignored in command |
| ^A             | [Output message to terminal](type-out.md) |
| ^B             | [Current date](values.md) |
| ^C             | [Stop execution](cmds/ctrl_C.md) |
| ^D             | [Set radix to decimal](cmds/ctrl_D.md) |
| ^E             | [Form feed flag](values.md) |
| ^E<*n*>        | (Match char) Match ASCII code *n* |
| ^EA            | (Match char) Match alphabetics |
| ^EB            | (Match char) Match separator character |
| ^EC            | (Match char) Match Symbol Constituent |
| ^ED            | (Match char) Match numerics |
| ^EG*q*         | (Match char) Match contents of Q-register *q* |
| ^EL            | (Match char) Match line terminators |
| ^EM*x*         | (Match char) Match any number of *x* |
| ^EQ*q*         | (String char) Use contents of Q-register *q* |
| ^ER            | (Match char) Match alphanumerics |
| ^ES            | (Match char) Match non-null space/tab |
| ^EU*q*         | (String char) Use ASCII code in Q-register *q* |
| ^EV            | (Match char) Match lower case alphabetic |
| ^EW            | (Match char) Match upper case alphabetic |
| ^EX            | (Match char) Match any character |
| ^E\[ \]        | (Match char) Match one of list |
| ^F             | Not a TECO command |
| ^G             | [Kill command string](cmds/ctrl_G.md) |
| ^G\<space\>    | [Retype current command line](cmds/ctrl_G.md) |
| ^G\*           | [Retype current command input](cmds/ctrl_G.md) |
| ^H             | [Current time of day](values.md) |
| BS             | [Back up and type one line](cmds/ctrl_H.md) |
| TAB            | [Insert tab and text](insert.md) |
| LF             | [Advance and type one line](cmds/LF.md) |
| LF             | [Line terminator](cmds/LF.md) |
| VT             | [Line terminator](cmds/VT.md) |
| FF             | [Page terminator](cmds/FF.md) |
| CR             | [End input line](cmds/CR.md) |
| ^N             | [End of file flag](values.md) |
| ^N*x*          | [(Match char.) Match all but *x*](cmds/ctrl_N.md) |
| ^O             | [Set radix to octal](cmds/ctrl_O.md) |
| ^P             | Not a TECO command |
| ^Q             | [Convert line argument to character argument](cmds/ctrl_Q.md) |
| ^Q*x*          | [Use *x* literally in search string](cmds/ctrl_Q.md) |
| ^R             | [Value of current radix](values.md) |
| *n*^R          | [Set radix to *n*](cmds/ctrl_R.md) |
| ^R*x*          | [Use *x* literally in search string](cmds/ctrl_Q.md) |
| ^S             | [-(length) of last referenced string](values.md) |
| ^S             | [(Match char.) Match separator character](cmds/ctrl_S.md) |
| ^T             | [ASCII value of next character typed](values.md) |
| ^T             | [Read and decode next keystroke typed](cmds/ctrl_T.md) |
| *n*^T          | [Type ASCII character of value *n*](type-out.md) |
| *n*:^T         | [Output binary byte of value n](type-out.md) |
| ^U             | [Kill command line](cmds/ctrl_U.md) |
| ^U*q*          | [Put string into Q-register *q*](cmds/ctrl_U.md) |
| :^U*q*         | [Append string to Q-register *q*](cmds/ctrl_U.md) |
| n^U*q*         | [Put ASCII char. *n* to Q-register *q*](cmds/ctrl_U.md) |
| :n^U*q*        | [Append ASCII char. *n* to Q-register *q*](cmds/ctrl_U.md) |
| ^V             | [Enable lower case conversion](cmds/ctrl_V.md) |
| ^V*x*          | [(String char.) Force *x* to lower case](cmds/ctrl_V.md) |
| ^W             | [Enable upper case conversion](cmds/ctrl_W.md) |
| ^W*x*          | [(String char.) Force *x* to upper case](cmds/ctrl_W.md) |
| ^X             | [Search mode flag](cmds/ctrl_X.md) |
| ^X             | [(Match char) Match any character](cmds/ctrl_X.md) |
| ^Y             | [Equivalent to ".+^S,."](values.md) |
| ^Z             | [Size of text in all Q-registers](values.md) |
| ESC            | [String and command terminator](cmds/ESC.md) |
| ^[             | [String and command terminator](cmds/ESC.md) |
| ^\\            | Not a TECO command |
| ^]             | Not a TECO command |
| ^^*x*          | [ASCII value of *x*](values.md) |
| ^_             | [One's complement (logical NOT)](oper.md) |
| SPACE          | Ignored in commands |
| !tag!          | [Define label](branch.md) |
| !              | [Logical NOT](oper.md) |
| \#             | [Logical OR](oper.md) |
| \&             | [Logical AND](oper.md) |
| \*             | [Multiplication](oper.md) |
| \+             | [Addition](oper.md) |
| \-             | [Subtraction or negation](oper.md) |
| .              | [Current pointer position](values.md) |
| /              | [Division yielding quotient](oper.md) |
| //             | [Division yielding remainder](oper.md) |
| 0-9            | Digit |
| :              | Modify next command |
| ;              | Exit iteration on search failure |
| *n*;           | Exit iteration if *n* is positive |
| :;             | Exit iteration on search success |
| *n*:;          | Exit iteration if *n* is negative |
| *n*\<          | Iterate n times |
| \<             | [Relational comparison for less than](oper.md) |
| \<=            | [Relational comparison for less than or equal](oper.md) |
| \<\<           | [Arithmetic left shift](oper.md) |
| \<\>           | [Relational comparison for inequality](oper.md) |
| =              | [Type in decimal](oper.md) |
| ==             | [Type in octal](oper.md) |
| ==             | [Relational comparison for equality](oper.md) |
| ===            | [Type in hexadecimal](oper.md) |
| :=             | [Type in decimal, no CR/LF](oper.md) |
| :==            | [Type in octal, no CR/LF](oper.md) |
| :===           | [Type in hexadecimal, no CR/LF](oper.md) |
| \>             | [Relational comparison for greater than](oper.md) |
| \<=            | [Relational comparisson for greater than or equal](oper.md) |
| \>\>           | [Arithmetic right shift](oper.md) |
| ?              | Toggle trace mode |
| ?              | Type out command string in error |
| @              | Modify next text argument |
| *n*A           | [ASCII value of *n*th character in buffer](values.md) |
| B              | [Beginning of buffer](values.md) |
| *n*C           | [Advance *n* characters](move.md) |
| *n*D           | [Delete *n* characters](delete.md) |
| *m*,*n*D       | [Delete between positions *m* and *n*](delete.md) |
| E%             | [Write file from Q-register](cmds/E_pct.md) |
| E1             | [Extended features flag](cmds/E1.md) |
| E2             | [Command restrictions flag](cmds/E2.md) |
| E3             | [File operations flag](cmds/E3.md) |
| E4             | [Display mode flag](cmds/E4.md) |
| EA             | [Switch to secondary output stream](cmds/EA.md) |
| EB             | [Edit backup](cmds/EB.md) |
| EC             | [Close input and output files](cmds/EC.md) |
| ED             | [TBD](cmds/ED.md) |
| EE             | [TBD](cmds/EE.md) |
| EF             | [Close output file](cmds/EF.md) |
| EG             | [Exit and Go](cmds/EG.md) |
| EH             | [TBD](cmds/EH.md) |
| EI             | [Execute indirect file](cmds/EI.md) |
| EJ             | [Get environment information](cmds/EJ.md) |
| EK             | [Kill output file](cmds/EK.md) |
| EL             | [Open/close log file](cmds/EL.md) |
| EN             | [Find next file](cmds/EN.md) |
| EO             | [TECO version number](values.md) |
| EP             | [Switch to secondary input stream](cmds/EP.md) |
| EQ             | [Read file to Q-register](cmds/EQ.md) |
| ER             | [Edit read](cmds/ER.md) |
|                | [Switch to primary input stream](cmds/ER.md) |
| ES             | [TBD](cmds/ES.md) |
| ET             | [TBD](cmds/ET.md) |
| EU             | [TBD](cmds/EU.md) |
| EV             | [TBD](cmds/EV.md) |
| EW             | [Edit write](cmds/EW.md) |
|                | [Switch to primary output stream](cmds/EW.md) |
| EX             | [Exit TECO](cmds/EX.md) |
| EY             | [TBD](cmds/EY.md) |
| E_             | [TBD](cmds/E_ubar.md) |
| F'             | Flow to end of conditional |
| F\<            | Flow to start of iteration |
| F\>            | Flow to end of iteration |
| F\|            | Flow to ELSE part of conditional |
| *m*,*n*FB      | Search between positions *m* and *n* |
| *n*FB          | Search, bounded by *n* lines |
| *m*,*n*FC      | Search and replace between *m* and *n* |
| *n*FC          | Search and replace over *n* lines |
| *n*FD          | [Search and delete string](delete.md) |
| FK             | [Search and delete intervening text](delete.md) |
| *n*FN          | Global string replace |
| FR             | [Replace last string](delete.md) |
| *n*FS          | Local string replace |
| *n*F_          | Destructive search and replace |
| G*q*           | Copy string from Q-register *q* into buffer |
| G\*            | Copy last filespec string into buffer |
| G_             | Copy last search string into buffer |
| :G*q*          | Type Q-register *q* |
| :G*            | Type last filespec string |
| :G_            | Type last search string |
| H              | [Equivalent to "B,Z"](values.md) |
| I              | [Insert text](insert.md) |
| *n*I           | [Insert character](insert.md) |
| *n*J           | [Move pointer](move.md) |
| *n*K           | [Kill *n* lines](delete.md) |
| *m*,*n*K       | [Delete between *m* and *n*](delete.md) |
| *n*L           | [Advance *n* lines](move.md) |
| *n*:L          | [Count of buffer lines](values.md) |
| M*q*           | [Returned value from macro](values.md) |
| M*q*           | Execute string in Q-register *q* |
| *n*N           | Global search |
| O              | Go to label |
| *n*O           | Computed *goto* |
| *n*P           | Advance or back up *n* pages |
| *m*,*n*P       | Write out chars from positions *m* to *n* |
| *n*PW          | Write buffer *n* times |
| *m*,*n*PW      | Write out chars from positions *m* to *n* |
| Q*q*           | [Number in Q-register *q*](values.md) |
| :Q*q*          | [Size of text in Q-register *q*](values.md) |
| *n*R           | [Back up *n* characters](move.md) |
| *n*S           | Local search |
| *m*,*n*S       | Search for *n*th occurrence within *m* characters |
| ::S            | Compare string 5.7 |
| *n*T           | [Type *n* lines](type-out.md) |
| *m*,*n*T       | [Type from positions *m* to *n*](type-out.md) |
| *n*U*q*        | Put number *n* into Q-register *q* |
| *n*V           | [Type *n* current lines](type-out.md) |
| *m*,*n*V       | [Type lines before and after current line](type-out.md) |
| W              | Display mode |
| *n*:W          | Return display mode characteristics |
| *m*,*n*:W      | Set display mode characteristics |
| *n*X*q*        | Put *n* lines into Q-register *q* |
| *m*,*n*X*q*    | Put characters *m* to *n* into Q-register *q* |
| *n*:X*q*       | Append *n* lines to Q-register *q* |
| *m*,*n*:X*q*   | Append characters *m* to *n* into Q-register q |
| Y              | Read into buffer |
| Z              | [End of buffer value](values.md) |
| [*q*           | Push Q-register *q* onto stack |
| \\             | [Value of digit string in buffer](values.md) |
| *n*\\          | Convert *n* to digits in buffer |
| ]*q*           | Pop from stack into Q-register *q* |
| *n*_           | Global search without output |
| \`             | Alternative command delimiter |
| a-z            | Equivalent to upper case A-Z commands |
| {              | Alternative text argument delimiter |
| \|             | Start ELSE part of conditional |
| }              | Alternative text argument delimiter |
| \~             | [Logical XOR](oper.md) |
| DEL            | Delete last character typed |

