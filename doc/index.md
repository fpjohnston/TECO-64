## TECO-64 - Index of Commands and Special Characters

| Character(s)   | Description |
| -------------- | ----------- |
| NUL            | [Discarded on input; ignored in command](misc.md) |
| \^A            | [Output message to terminal](typeout.md) |
| \^B            | [Current date](variables.md) |
| \^C            | [Stop execution](action.md) |
| \^D            | [Set radix to decimal](misc.md) |
| \^E            | [Form feed flag](variables.md) |
| \^E<*n*>       | [(Match char) Match ASCII code *n*](search.md) |
| \^EA           | [(Match char) Match alphabetics](search.md) |
| \^EB           | [(Match char) Match separator character](search.md) |
| \^EC           | [(Match char) Match symbol constituent](search.md) |
| \^ED           | [(Match char) Match digit character](search.md) |
| \^EG*q*        | [(Match char) Match contents of Q-register *q*](search.md) |
| \^EL           | [(Match char) Match line terminators](search.md) |
| \^EQ*q*        | [(String char) Use contents of Q-register *q*](search.md) |
| \^ER           | [(Match char) Match alphanumerics](search.md) |
| \^ES           | [(Match char) Match non-null space/tab](search.md) |
| \^EU*q*        | [(String char) Use ASCII code in Q-register *q*](search.md) |
| \^EV           | [(Match char) Match lower case alphabetic](search.md) |
| \^EW           | [(Match char) Match upper case alphabetic](search.md) |
| \^EX           | [(Match char) Match any character](search.md) |
| \^F            | (Reserved for future use) |
| \^G\^G         | [Delete entire command string](action.md) |
| \^G&lt;SPACE>  | [Retype current command line](action.md) |
| \^G\*          | [Retype current command input](action.md) |
| \^H            | [Current time of day](variables.md) |
| BS             | [Back up and type one line](action.md) |
| TAB            | [Insert tab and text](insert.md) |
| LF             | [Advance and type one line](action.md) |
| LF             | [Line terminator](conventions.md) |
| &lt;CTRL/K>    | [Reset display colors](action.md) |
| VT             | [Line terminator](conventions.md) |
| FF             | [Page terminator](conventions.md) |
| CR             | [End input line](conventions.md) |
| \^N            | [End of file flag](variables.md) |
| \^N*x*         | [(Match char.) Match all but *x*](search.md) |
| \^O            | [Set radix to octal](misc.md) |
| \^P            | [Current page number](variables.md) |
| \^Q            | [Convert line argument to character argument](variables.md) |
| \^Q*x*         | [Use *x* literally in search string](search.md) |
| \^R            | [Value of current radix](variables.md) |
| *n*\^R         | [Set radix to *n*](misc.md) |
| \^R*x*         | [Use *x* literally in search string](search.md) |
| \^S            | [-(length) of last referenced string](variables.md) |
| \^S            | [(Match char.) Match separator character](search.md) |
| \^T            | [ASCII value of next input character](variables.md) |
| *n*\^T         | [Type ASCII character of value *n*](typeout.md) |
| *n*:\^T        | [Output binary byte of value n](typeout.md) |
| \^U            | [Kill command line](qregister.md) |
| \^U*q*         | [Put string into Q-register *q*](qregister.md) |
| :\^U*q*        | [Append string to Q-register *q*](qregister.md) |
| n\^U*q*        | [Put ASCII char. *n* to Q-register *q*](qregister.md) |
| :n\^U*q*       | [Append ASCII char. *n* to Q-register *q*](qregister.md) |
| \^V            | [Enable lower case conversion](misc.md) |
| \^V*x*         | [(String char.) Force *x* to lower case](search.md) |
| &lt;CTRL/W>    | [Repaint display](action.md) |
| \^W            | [Enable upper case conversion](misc.md) |
| \^W*x*         | [(String char.) Force *x* to upper case](search.md) |
| \^X            | [Search mode flag](flags.md) |
| \^X            | [(Match char) Match any character](search.md) |
| \^Y            | [Equivalent to ".+\^S,."](variables.md) |
| \^Z            | [Size of text in all Q-registers](variables.md) |
| ESC            | [String and command terminator](action.md) |
| \^\[           | [String and command terminator](action.md) |
| \^\\           | Causes TECO to immediately abort. Use with caution. No recovery is possible. |
| \^]            | [Repeats last command](action.md) |
| \^\^*x*        | [ASCII value of *x*](variables.md) |
| \^_            | [One's complement (logical NOT)](oper.md) |
| SPACE          | [Ignored in commands](conventions.md) |
| !tag!          | [Define label](goto.md) |
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
| $              | Not a TECO command. |
| *n*%*q*        | [Add *n* to Q-register *q* and return result](qregister.md) |
| *n*:%*q*       | [Add *n* to Q-register *q*, but do not return result](qregister.md) |
| \&             | [Logical AND](oper.md) |
| '              | [End conditional](ifthen.md) |
| (              | [Expression grouping](oper.md) |
| )              | [Expression grouping](oper.md) |
| \*             | [Multiplication](oper.md) |
| \**q*          | [Save last command in Q-register *q*](qregister.md) |
| \+             | [Addition](oper.md) |
| ,              | [Argument separator](basics.md) |
| \-             | [Subtraction or negation](oper.md) |
| .              | [Current pointer position](variables.md) |
| /              | [Division yielding quotient](oper.md) |
| /              | [Type detailed explanation of error](action.md) |
| //             | [Division yielding remainder](oper.md) |
| 0-9            | [Digit](conventions.md) |
| :              | [Modify next command](basics.md) |
| ;              | [Exit iteration on search failure](loops.md) |
| *n*;           | [Exit iteration if *n* is positive](loops.md) |
| :;             | [Exit iteration on search success](loops.md) |
| *n*:;          | [Exit iteration if *n* is negative](loops.md) |
| *n*&lt;        | [Iterate n times](loops.md) |
| &lt;           | [Relational comparison for less than](oper.md) |
| &lt;=          | [Relational comparison for less than or equal](oper.md) |
| &lt;&lt;       | [Arithmetic left shift](oper.md) |
| &lt;>          | [Relational comparison for inequality](oper.md) |
| =              | [Type in decimal](typeout.md) |
| ==             | [Type in octal](typeout.md) |
| ==             | [Relational comparison for equality](oper.md) |
| ===            | [Type in hexadecimal](typeout.md) |
| :=             | [Type in decimal, no CR/LF](typeout.md) |
| :==            | [Type in octal, no CR/LF](typeout.md) |
| :===           | [Type in hexadecimal, no CR/LF](typeout.md) |
| >              | [End loop](loops.md) |
| >              | [Relational comparison for greater than](oper.md) |
| &lt;=          | [Relational comparison for greater than or equal](oper.md) |
| >>             | [Arithmetic right shift](oper.md) |
| ?              | [Toggle trace mode](misc.md) |
| ?              | [Type out command string in error](action.md) |
| @              | [Modify next text argument](basics.md) |
| A              | [Append to buffer](page.md) |
| *n*A           | [ASCII value of *n*th character in buffer](variables.md) |
| *n*:A          | [Append *n* lines to buffer](page.md) |
| B              | [Beginning of buffer](variables.md) |
| *n*C           | [Advance *n* characters](dot.md) |
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
| *n*EC          | [Set memory size](misc.md) |
| ED             | [Edit level flag](flags.md) |
| EE             | [Alternate command delimiter](flags.md) |
| EF             | [Close output file](file.md) |
| EG             | [Exit and Go](file.md) |
| :EG            | [Read environment variables](env.md) |
| EH             | [Edit help flag](flags.md) |
| EI             | [Execute indirect command file](file.md) |
| EJ             | [Get environment information](env.md) |
| EK             | [Kill output file](file.md) |
| EL             | [Open/close log file](file.md) |
| EM*q*          | [Type macro in Q-register *q*](misc.md) |
| EN             | [Find next file](file.md) |
| EO             | [TECO version number](variables.md) |
| EP             | [Switch to secondary input stream](file.md) |
| EQ             | [Read file to Q-register](file.md) |
| ER             | [Edit read](file.md) |
| ER             | [Switch to primary input stream](file.md) |
| ES             | [Edit search flag](flags.md) |
| ET             | [Terminal flag](flags.md) |
| EU             | [Upper/lower case flag](flags.md) |
| EV             | [Edit verify flag](flags.md) |
| EW             | [Edit write](file.md) |
| EW             | [Switch to primary output stream](file.md) |
| EX             | [Exit TECO](file.md) |
| EY             | [Yank w/o yank protection](page.md) |
| EZ             | [Execute system command](env.md) |
| E_             | [Search w/o yank protection](search.md) |
| F'             | [Flow to end of conditional](ifthen.md) |
| F0             | [Edit buffer position at start of window](variables.md) |
| F1             | [Set command window colors](display.md) |
| F2             | [Set edit window colors](display.md) |
| F3             | [Set status window colors](display.md) |
| F4             | [Set separator line colors](display.md) |
| F&lt;          | [Flow to start of iteration](loops.md) |
| F>             | [Flow to end of iteration](loops.md) |
| *m*,*n*FB      | [Search between positions *m* and *n*](search.md) |
| *n*FB          | [Search, bounded by *n* lines](search.md) |
| *m*,*n*FC      | [Search and replace between *m* and *n*](search.md) |
| *n*FC          | [Search and replace over *n* lines](search.md) |
| *n*FD          | [Search and delete string](delete.md) |
| FF             | (Reserved for future use) |
| FH             | [Equivalent to "F0,FZ"](variables.md) |
| FK             | [Search and delete intervening text](delete.md) |
| FL             | [Convert to lower case](misc.md) |
| FM             | [Map key to command string](keymap.md) |
| *n*FN          | [Global string replace](search.md) |
| FQ*q*          | [Map key to Q-register *q*](keymap.md) |
| FR\`           | [Delete string from last insert or search](delete.md) |
| FR*text*\`     | [Replace string from last insert or search](insert.md) |
| *n*FS          | [Local string replace](search.md) |
| FU             | [Convert to upper case](misc.md) |
| FZ             | [Edit buffer position at end of window](variables.md) |
| *n*F_          | [Destructive search and replace](search.md) |
| F\|            | [Flow to ELSE part of conditional](ifthen.md) |
| G*q*           | [Copy string from Q-register *q* into buffer](qregister.md) |
| G\*            | [Copy last filespec string into buffer](qregister.md) |
| G_             | [Copy last search string into buffer](qregister.md) |
| :G*q*          | [Type Q-register *q*](qregister.md) |
| :G*            | [Type last filespec string](qregister.md) |
| :G_            | [Type last search string](qregister.md) |
| H              | [Equivalent to "B,Z"](variables.md) |
| I              | [Insert text](insert.md) |
| *n*I           | [Insert character](insert.md) |
| *n*J           | [Move pointer](dot.md) |
| *n*K           | [Kill *n* lines](delete.md) |
| *m*,*n*K       | [Delete between *m* and *n*](delete.md) |
| *n*L           | [Advance *n* lines](dot.md) |
| *n*:L          | [Count of buffer lines](variables.md) |
| M*q*           | [Returned value from macro](variables.md) |
| M*q*           | [Execute string in Q-register *q*](qregister.md) |
| *n*N           | [Global search](search.md) |
| O              | [Go to label](goto.md) |
| *n*O           | [Computed *goto*](goto.md) |
| *n*P           | [Advance or back up *n* pages](page.md) |
| *m*,*n*P       | [Write out chars from positions *m* to *n*](page.md) |
| *n*PW          | [Write buffer *n* times](page.md) |
| *m*,*n*PW      | [Write out chars from positions *m* to *n*](page.md) |
| Q*q*           | [Number in Q-register *q*](variables.md) |
| :Q*q*          | [Size of text in Q-register *q*](variables.md) |
| *n*R           | [Back up *n* characters](dot.md) |
| *n*S           | [Local search](search.md) |
| *m*,*n*S       | [Search for *n*th occurrence within *m* characters](search.md) |
| ::S            | [Compare string](search.md) |
| *n*T           | [Type *n* lines](typeout.md) |
| *m*,*n*T       | [Type from positions *m* to *n*](typeout.md) |
| *n*U*q*        | [Put number *n* into Q-register *q*](qregister.md) |
| *n*V           | [Type *n* current lines](typeout.md) |
| *m*,*n*V       | [Type lines before and after current line](typeout.md) |
| W              | [Enable or disable display mode](display.md) |
| *n*:W          | [Read display mode characteristics](display.md) |
| *m*,*n*:W      | [Set display mode characteristics](display.md) |
| *n*X*q*        | [Put *n* lines into Q-register *q*](qregister.md) |
| *m*,*n*X*q*    | [Put characters *m* to *n* into Q-register *q*](qregister.md) |
| *n*:X*q*       | [Append *n* lines to Q-register *q*](qregister.md) |
| *m*,*n*:X*q*   | [Append characters *m* to *n* into Q-register *q*](qregister.md) |
| Y              | [Read into buffer](page.md) |
| Z              | [End of buffer value](variables.md) |
| \[*q*          | [Push Q-register *q* onto stack](qregister.md) |
| \\             | [Value of digit string in buffer](variables.md) |
| *n*\\          | [Insert digit string in buffer](insert.md) |
| ]*q*           | [Pop from stack into Q-register *q*](qregister.md) |
| *n*_           | [Global search without output](search.md) |
| \`             | Not a TECO command |
| a-z            | Equivalent to upper case A-Z commands |
| {              | [Alternative text argument delimiter](basics.md) |
| \|             | [Start ELSE part of conditional](ifthen.md) |
| }              | [Alternative text argument delimiter](basics.md) |
| \~             | [Logical XOR](oper.md) |
| DEL            | [Delete last character typed](action.md) |
