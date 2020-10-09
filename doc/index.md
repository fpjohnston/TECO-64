### TECO-64 - Index of Commands and Special Characters

| Character(s)   | Description |
| -------------- | ----------- |
| NUL            | Discarded on input; ignored in command |
| ^A             | [Output message to terminal](type-out.md) |
| ^B             | [Current date](values.md) |
| ^C             | [Stop execution](cmds/ctrl_C.md) |
| ^D             | [Set radix to decimal](cmds/ctrl_D.md) |
| ^E             | [Form feed flag](values.md) |
| ^E<*n*>        | [(Match char) Match ASCII code *n*](search.md) |
| ^EA            | [(Match char) Match alphabetics](search.md) |
| ^EB            | [(Match char) Match separator character](search.md) |
| ^EC            | [(Match char) Match Symbol Constituent](search.md) |
| ^ED            | [(Match char) Match numerics](search.md) |
| ^EG*q*         | [(Match char) Match contents of Q-register *q*](search.md) |
| ^EL            | [(Match char) Match line terminators](search.md) |
| ^EM*x*         | [(Match char) Match any number of *x*](search.md) |
| ^EQ*q*         | [(String char) Use contents of Q-register *q*](search.md) |
| ^ER            | [(Match char) Match alphanumerics](search.md) |
| ^ES            | [(Match char) Match non-null space/tab](search.md) |
| ^EU*q*         | [(String char) Use ASCII code in Q-register *q*](search.md) |
| ^EV            | [(Match char) Match lower case alphabetic](search.md) |
| ^EW            | [(Match char) Match upper case alphabetic](search.md) |
| ^EX            | [(Match char) Match any character](search.md) |
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
| ^U             | [Kill command line](qregister.md) |
| ^U*q*          | [Put string into Q-register *q*](qregister.md) |
| :^U*q*         | [Append string to Q-register *q*](qregister.md) |
| n^U*q*         | [Put ASCII char. *n* to Q-register *q*](qregister.md) |
| :n^U*q*        | [Append ASCII char. *n* to Q-register *q*](qregister.md) |
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
| "              | [Start conditional](ifthen.md)
| *n*"<          | [Test for less than zero](ifthen.md) |
| *n*"=          | [Test for equal to zero](ifthen.md) |
| *n*">          | [Test for greater than zero](ifthen.md) |
| *n*"A          | [Test for alphabetic](ifthen.md) |
| *n*"C          | [Test for symbol constituent](ifthen.md) |
| *n*"D          | [Test for numeric](ifthen.md) |
| *n*"E          | [Test for equal to zero](ifthen.md) |
| *n*"F          | [Test for false](ifthen.md) |
| *n*"G          | [Test for greater than zero](ifthen.md) |
| *n*"L          | [Test for less than zero](ifthen.md) |
| *n*"N          | [Test for not equal to zero](ifthen.md) |
| *n*"R          | [Test for alphanumeric](ifthen.md) |
| *n*"S          | [Test for successful](ifthen.md) |
| *n*"T          | [Test for true](ifthen.md) |
| *n*"U          | [Test for unsuccessful](ifthen.md) |
| *n*"V          | [Test for lower case](ifthen.md) |
| *n*"W          | [Test for upper case](ifthen.md) |
| \#             | [Logical OR](oper.md) |
| $              | Separate TECO commands |
| *n*%*q*        | [Add *n* to Q-register *q* and return result](qregister.md) |
| \&             | [Logical AND](oper.md) |
| '              | [End conditional](ifthen.md) |
| (              | [Expression grouping](oper.md) |
| )              | [Expression grouping](oper.md) |
| \*             | [Multiplication](oper.md) |
| \**q*          | [Save last command in Q-register *q*](qregister.md) |
| \+             | [Addition](oper.md) |
| ,              | Argument separator |
| \-             | [Subtraction or negation](oper.md) |
| .              | [Current pointer position](values.md) |
| /              | [Division yielding quotient](oper.md) |
| /              | Type detailed explanation of error |
| //             | [Division yielding remainder](oper.md) |
| 0-9            | Digit |
| :              | Modify next command |
| ;              | [Exit iteration on search failure](branching.md) |
| *n*;           | [Exit iteration if *n* is positive](branching.md) |
| :;             | [Exit iteration on search success](branching.md) |
| *n*:;          | [Exit iteration if *n* is negative](branching.md) |
| *n*\<          | [Iterate n times](branching.md) |
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
| \>             | [End loop](branches.md) |
| \>             | [Relational comparison for greater than](oper.md) |
| \<=            | [Relational comparisson for greater than or equal](oper.md) |
| \>\>           | [Arithmetic right shift](oper.md) |
| ?              | Toggle trace mode |
| ?              | Type out command string in error |
| @              | Modify next text argument |
| A              | [Append to buffer](page.md) |
| *n*A           | [ASCII value of *n*th character in buffer](values.md) |
| *n*:A          | [Append *n* lines to buffer](page.md) |
| B              | [Beginning of buffer](values.md) |
| *n*C           | [Advance *n* characters](move.md) |
| *n*D           | [Delete *n* characters](delete.md) |
| *m*,*n*D       | [Delete between positions *m* and *n*](delete.md) |
| E%             | [Write file from Q-register](file.md) |
| E1             | [Extended features flag](flags.md) |
| E2             | [Command restrictions flag](flags.md) |
| E3             | [File operations flag](flags.md) |
| E4             | [Display mode flag](flags.md) |
| EA             | [Switch to secondary output stream](file.md) |
| EB             | [Edit backup](file.md) |
| EC             | [Close input and output files](file.md) |
| ED             | [Edit level flag](flags.md) |
| EE             | [Alternate command delimiter](flags.md) |
| EF             | [Close output file](file.md) |
| EG             | [Exit and Go](file.md) |
| :EG            | [Read environment variables](env.md) |
| ::EG           | [Execute system command](env.md) |
| EH             | [Edit help flag](flags.md) |
| EI             | [Execute indirect file](file.md) |
| EJ             | [Get environment information](env.md) |
| EK             | [Kill output file](file.md) |
| EL             | [Open/close log file](file.md) |
| EN             | [Find next file](file.md) |
| EO             | [TECO version number](values.md) |
| EP             | [Switch to secondary input stream](file.md) |
| EQ             | [Read file to Q-register](file.md) |
| ER             | [Edit read](file.md) |
|                | [Switch to primary input stream](file.md) |
| ES             | [Edit search flag](flags.md) |
| ET             | [Terminal flag](flags.md) |
| EU             | [Upper/lower case flag](flags.md) |
| EV             | [Edit verify flag](flags.md) |
| EW             | [Edit write](file.md) |
|                | [Switch to primary output stream](file.md) |
| EX             | [Exit TECO](file.md) |
| EY             | [Yank w/o yank protection](page.md) |
| E_             | [Search w/o yank protection](search.md) |
| F'             | [Flow to end of conditional](branching.md) |
| F\<            | [Flow to start of iteration](branching.md) |
| F\>            | [Flow to end of iteration](branching.md) |
| F\|            | [Flow to ELSE part of conditional](branching.md) |
| *m*,*n*FB      | [Search between positions *m* and *n*](search.md) |
| *n*FB          | [Search, bounded by *n* lines](search.md) |
| *m*,*n*FC      | [Search and replace between *m* and *n*](search.md) |
| *n*FC          | [Search and replace over *n* lines](search.md) |
| *n*FD          | [Search and delete string](delete.md) |
| FK             | [Search and delete intervening text](delete.md) |
| *n*FN          | [Global string replace](search.md) |
| FR             | [Replace last string](delete.md) |
| *n*FS          | [Local string replace](search.md) |
| *n*F_          | [Destructive search and replace](search.md) |
| G*q*           | [Copy string from Q-register *q* into buffer](qregister.md) |
| G\*            | [Copy last filespec string into buffer](qregister.md) |
| G_             | [Copy last search string into buffer](qregister.md) |
| :G*q*          | [Type Q-register *q*](qregister.md) |
| :G*            | [Type last filespec string](qregister.md) |
| :G_            | [Type last search string](qregister.md) |
| H              | [Equivalent to "B,Z"](values.md) |
| I              | [Insert text](insert.md) |
| *n*I           | [Insert character](insert.md) |
| *n*J           | [Move pointer](move.md) |
| *n*K           | [Kill *n* lines](delete.md) |
| *m*,*n*K       | [Delete between *m* and *n*](delete.md) |
| *n*L           | [Advance *n* lines](move.md) |
| *n*:L          | [Count of buffer lines](values.md) |
| M*q*           | [Returned value from macro](values.md) |
| M*q*           | [Execute string in Q-register *q*](qregister.md) |
| *n*N           | [Global search](search.md) |
| O              | [Go to label](branching.md) |
| *n*O           | [Computed *goto*](branching.md) |
| *n*P           | [Advance or back up *n* pages](page.md) |
| *m*,*n*P       | [Write out chars from positions *m* to *n*](page.md) |
| *n*PW          | [Write buffer *n* times](page.md) |
| *m*,*n*PW      | [Write out chars from positions *m* to *n*](page.md) |
| Q*q*           | [Number in Q-register *q*](values.md) |
| :Q*q*          | [Size of text in Q-register *q*](values.md) |
| *n*R           | [Back up *n* characters](move.md) |
| *n*S           | [Local search](search.md) |
| *m*,*n*S       | [Search for *n*th occurrence within *m* characters](search.md) |
| ::S            | [Compare string](search.md) |
| *n*T           | [Type *n* lines](type-out.md) |
| *m*,*n*T       | [Type from positions *m* to *n*](type-out.md) |
| *n*U*q*        | [Put number *n* into Q-register *q*](qregister.md) |
| *n*V           | [Type *n* current lines](type-out.md) |
| *m*,*n*V       | [Type lines before and after current line](type-out.md) |
| W              | Display mode |
| *n*:W          | Return display mode characteristics |
| *m*,*n*:W      | Set display mode characteristics |
| *n*X*q*        | Put *n* lines into Q-register *q*](qregister.md) |
| *m*,*n*X*q*    | [Put characters *m* to *n* into Q-register *q*](qregister.md) |
| *n*:X*q*       | [Append *n* lines to Q-register *q*](qregister.md) |
| *m*,*n*:X*q*   | [Append characters *m* to *n* into Q-register *q*](qregister.md) |
| Y              | [Read into buffer](page.md) |
| Z              | [End of buffer value](values.md) |
| [*q*           | [Push Q-register *q* onto stack](qregister.md) |
| \\             | [Value of digit string in buffer](values.md) |
| *n*\\          | Convert *n* to digits in buffer |
| ]*q*           | [Pop from stack into Q-register *q*](qregister.md) |
| *n*_           | [Global search without output](search.md) |
| \`             | Alternative command delimiter |
| a-z            | Equivalent to upper case A-Z commands |
| {              | Alternative text argument delimiter |
| \|             | [Start ELSE part of conditional](ifthen.md) |
| }              | Alternative text argument delimiter |
| \~             | [Logical XOR](oper.md) |
| DEL            | Delete last character typed |
