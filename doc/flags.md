## TECO-64 - Mode Control Flags

TECO has flags which control various aspects of its operation. You can find a
flag’s current setting by executing its command name without an argument; the
current setting of the flag is returned as a value. A flag may be set to a specific
value by executing its command name preceded by a numerical argument; the
flag is set to the value of the argument.

The following table describes the format of the commands that set and clear flags.
Note that the formats which set and/or clear bits in a flag only apply to commands
which are truly bit-encoded, such as ED or ET.
Other commands, such as ES, interpret the *m* and *n* arguments differently.
See specific commands for details.

| Command | Function |
| ------- | -------- |
| \<flag\>  | Return value of flag. |
| *n*\<flag\> | Set value of flag to *n*. |
| *m*,*n*\<flag\> | In the flag, turn off those bits specified by *m* and turn on those bits specified by *n*. |
| 0,*n*\<flag\> | Turn on the bits in the flag specified by *n*. |
| *m*,0\<flag\> | Turn off the bits in the flag specified by *m*. |
| *m*,*n*\<flag\> | Turn off the bits in the flag specified by *m*, and turn on the bits specified by *n*. |

### E1 - Extended Features Flag

The E1 flag contains bit-encoded information controls whether certain new and
extended features are enabled in TECO.
By default, all of the bits below are set, but an initialization file may be
used to customize which bits are set or cleared.

| Bit | Function |
| --- | -------- |
| E1&1 |  If set, allow extended operators within parentheses. If clear, no extended operators are allowed. | 
| E1&2 | If set, paired parentheses, braces, square brackets, or angle brackets may be used to delimit text strings. If clear, parentheses, braces, and brackets have no special meaning in text arguments. |
| E1&4 | If set, a colon modifier for a CTRL/A command will cause it to print a newline after the text message . If clear, CTRL/A commands will ignore any colon modifiers. |
| E1&8 | If set, an at-sign modifier may be used with an '=' command, which allows the use of a text argument containing an *sprintf()* format string. If clear, '=' commands will ignore any at-sign modifiers. |
| E1&16 | If set, EI commands are executed immediately, as though they were macros, rather being executed after completion of the current command string. The latter method is how EI commands were classically executed in TECO, while the former method is new to TECO-64. |
| E1&32 | If set, "!!" may be used start a comment that extends to the next line terminator (LF, VT, or FF). |
| E1&64 | If set, CR/LF will be output if needed to ensure that TECO's prompt is printed at the start of a new line. |
| E1&128 | If set, the radix of an input number will be determined as follows: if the first  two characters are 0x or 0X, the number is assumed to be decimal; if the first character is 1-9, it is assumed to be decimal; and if the first character is 0, it is assumed to be octal. |
| E1&256 | Unused. |
| E1&512 | If set, an *n*I command is equivalent to *n*I\<ESC\> or *n*@I//. If clear, any *n*I command must be terminated with either an ESCape or a delimiter. |
| E1&1024 | If set, *n*% commands may include a colon modifier that causes the return value to be discarded (obviating the need to include an ESCape in order to avoid passing that value to the next command). If clear, colon modifiers preceding *n*% commands have no special meaning. |
| E1&2048 | Unused. |
| E1&4096 | Unused. |
| E1&8192 | Unused. |
| E1&16384 | Reserved for future use. |
| E1&32768 | Reserved for future use. |

### E2 - Command Restrictions Flag

The E2 flag contains bit-encoded information that controls how strictly TECO
enforces command syntax.
For compatibility with older TECO macros, none of the bits below are set
by default, but an initialization may be used to customize which bits
are set or cleared.

| Bit | Function |
| --- | -------- |
| E2&1 | If set, issue DIV error if attempt is made to divide by zero. |
| E2&2 | If set, issue IFE error if double operators are used in expressions (for example, 2**3). |
| E2&4 | If set, issue ATS error if command does not allow an at sign modifier, or if more than one at sign modifier is seen. |
| E2&8 | If set, issue COL error if command does not allow a colon modifier, or if more than two colon modifiers are seen. |
| E2&16 | If set, issue MCA error if missing m argument before comma command. |
| E2&32 | If set, issue IMA error if command does not allow an m argument. |
| E2&64 | If set, issue INA error if command does not allow an n argument. |
| E2&128 | If set, issue MAP error if loop is not complete within a conditional. |
| E2&256 | If set, issue BNI error if conditional is not complete within a loop. |
| E2&512 | If set, issue NAT error for m,n:P or H:P or :PW. |
| E2&1024 | If set, issue ARG error if command has too many arguments. |

### E3 - File Operations Flag

The E3 flag contains bit-encoded information that controls how TECO reads input
files and writes output files.
This flag exists mostly to achieve compatibility with various older TECOs,
where the handling of file operations was operating-system dependent.

| Bit | Function |
| --- | -------- |
| E3&1 | If this bit is set, set, FF is a normal input character and not a page separator. If this bit is clear, FF is a page separator, and is not normally included in edit buffers. |
| E3&2 | Specifies whether “smart” line terminator mode is in effect. If this bit is set, the line terminator for the first line read sets the next two bits. If the first line ends with LF, then all CR/LF is translated to LF on input, and lines are output with LF only. If the first line ends with CR/LF, then all input lines will end with CR/LF, as will all output lines. If this bit is clear, then the next two bits determine what line terminators are used for input and output. |
| E3&4 | Specifies whether the line delimiter for input files is LF or CR/LF. If this bit is set, CR/LF is translated to LF on input. If this bit is clear, the delimiter is LF. The default setting is clear for Linux and MacOS, and set for Windows and VMS. |
| E3&8 | Specifies whether the line delimiter for output files is LF or CR/LF. If this bit is set, LF is translated to CR/LF on output. If this bit is clear, the delimiter is LF. The default setting is clear for Linux and MacOS, and set for Windows and VMS. |
| E3&16 | This bit affects the behavior of echoed input to log files (opened with the EL command). If the bit is set, echoed input is not written to the log file. If the bit is clear, all echoed input is written to the log file. |
| E3&32 | This bit affects the behavior of output messages to log files (opened with the EL command). If the bit is set, output is not written to the log file. If the bit is clear, all output is written to the log file. |
| E3&64 | Unused. |
 | E3&128 | If set, keep NUL characters found in input files. If the bit is clear, discard NUL characters in input files. |
 | E3&256 | This bit affects the type out of LF with CTRL/A, CTRL/T, :G*q*, T, and V commands. If set, LF is converted to CR/LF. If clear, LF is output as is. |
 
### E4 - Display Mode Flag

The E4 flag contains bit-encoded information that controls the appearance of
the TECO's display window, when the -1W has been executed.

| Bit | Function |
| --- | -------- |
| E4&1 | Controls whether the text window is above or below the command window. If this bit is set, the text window is below the command window. If this bit is clear, the text window is above the command window. |
| E4&2 | Controls whether there should be a line between the text and command windows. If this bit is set, there is a line separating the text and command windows. If this bit is clear, no line is displayed. |
| E4&4 | Controls whether status information should be included on the status line. If the bit is set, then information about the position within the file as well as the date and time are included. If the bit is clear, there is no information displayed. |

### ED - Edit Level Flag

The edit level flag is a bit-encoded word that controls TECO’s
behavior in various respects. Any combination of the individual
bits may be set as the user sees fit. The bits have the following
functions:

| Bit | Function |
| --- | -------- |
| ED&1 | Allow caret (^) in search strings. If this bit is clear, a caret (^) in a search string modifies the immediately following character to become a control character. When this bit is set, a caret in a search string is simply the literal character caret. If you are editing a file that contains many caret characters, you will want to set this bit. (For control of upper/lower case matching in search strings, see the ^X flag.) |
| ED&2 | Allow all Y and _ commands. If this bit is set, the Y and _ commands work unconditionally as described earlier in the manual. If clear, the behavior of the Y and _ commands are modified as follows: If an output file is open and text exists in the edit buffer, the Y or _ command will produce an error message and the command will be aborted leaving the edit buffer unchanged. Note that if no output file is open the Y and _ commands act normally. Furthermore, if the edit buffer is empty the Y command can be used to bring in a page of text whether or not an output file is open (HKY will always work). The _ command will succeed in bringing one page of text into an empty edit buffer but will fail to bring in successive pages if an output file is open. |
| ED&4 | Unused in TECO-64. |
| ED&8 | Unused in TECO-64. |
| ED&16 | Allow failing searches to preserve dot. If this bit is set, then whenever a search fails, the original location of the edit buffer pointer will be preserved. If this bit is clear, then failing searches (other than bounded searches) leave the edit buffer pointer at pointer position 0 after they fail. |
| ED&32 | Enable immediate ESCape-sequence commands. If this bit is set, TECO will recognize an ESCape sequence key pressed immediately after the prompting asterisk as an immediate command. See [here](action.md) for a description of immediate ESCape-sequence commands. If this bit is clear (the default case), TECO will treat an ESCape coming in immediately after the asterisk prompt as a \<DELIM\>; that is, TECO will hear a discrete \<ESC\> character: an ESCape sequence will therefore be treated not as a unified command, but as a sequence of characters. |
| ED&64 | Only move dot by one on multiple occurrence searches. If this bit is clear, TECO treats nStext$ exactly as n\<1Stext\$\>. That is, skip over the whole matched search string when proceeding to the nth search match. For example, if the edit buffer contains only A’s, the command 5SAA$ will complete with dot equal to 10. If this bit is set, TECO increments dot by one each search match. In the above example, dot would become 5. |
| ED&128 | Unused in TECO-64. |
| ED&256 | If set before a file is opened with an EB or EW command, P and PW commands cause buffer data to be immediately output to that file. If clear, file data may be internally buffered before being output, and possibly not output until the file is closed. Changing this bit has no effect on any output files that are already open. |

The initial value of ED&1 is system dependent. The initial value of the other
bits in the ED flag is 0.

### EE - Alternate Command Delimiter

| Command | Function |
| ------- | -------- |
| *n*EE | Set EE flag. When the flag is zero, and unless the 8192 ET bit is set, TECO recognizes only the ESCape as its \<DELIM\> character from the terminal. If it is desired to use another character as an ESCape surrogate (as when working from a terminal lacking an ESCape key), the ASCII value of that other character may be set in the EE flag. <br><br>Note that when an ESCape surrogate is set, that character (when typed at the terminal) is received by TECO as an ESCape — that character is no longer directly available at the keyboard. When an ESCape surrogate is active, an ESCape received by TECO echoes as accent grave; when none is active, an ESCape received by TECO echoes as a dollar sign. This feature is provided for the benefit of newer terminals which lack an ESCape key. (8192ET and 96EE both set \` as ESCape surrogate. EE, however, can be used to designate a character other than accent grave.) |

The EE flag is initially zero.

### EH - Help Level Flag

The help level flag controls the printing of error messages and failed commands.
(See also the / command.)

| Bits | Function |
| ---- | -------- |
| EH&3 | If the low two bits are 1, error messages are output in abbreviated form ("?XXX"). If they are equal to 0 or 2, error messages are output in normal form ("?XXX Message"). If they are equal to 3, error messages are output in long or "War and Peace" form, that is, a paragraph of informative material is typed following the normal form of the error message. |
| EH&4 | If set, the failing command is also output up to and including the failing character in the command followed by a question mark. (Just like TECO’s response to the typing of a question mark immediately after an error.) |
| EH&8 | Reserved for future use. |
| EH&16 | Reserved for future use. |

The initial value of the EH flag is 0 which is equivalent to a value of 2.

### ES - Search Verification Flag

The search verification flag controls whether any text is typed out
after searches.

| Command | Function |
| ------- | -------- |
| 0ES  | Nothing is typed out after searches. |
| -1ES | The current line is typed out when a successful search at top level is completed (i.e., a V command is done automatically). |
| *n*ES | If *n* is between 1 and 31, the current line is typed out with a line feed immediately following the position of the pointer to identify its position. If *n* is between 32 and 126, the current line is typed out with the ASCII character corresponding to the value of n immediately following the position of the pointer to identify its position. |
| *m*,*n*ES | Same as *n*ES, but prints *m* lines before and after the current line. <br><br>For compatibility with older TECOs, this command may also be specified in the form (*m*+1)\*256+*n*ES. |

The ES flag does not apply to searches executed inside iterations or
macros; lines found inside iterations or macros are never typed
out.

The initial value of the ES flag is 0.

### ET - Terminal Flag

The ET flag contains bit-encoded information that affect terminal input and output.
Any combination of the individual bits may be set.

| Bit | Function |
| --- | -------- |
| ET&1 | Type out in image mode. Setting this bit inhibits all of TECO’s type out conversions. All characters are output to the terminal exactly as they appear in the buffer or ^A command. For example, the changing of control characters into the "caret/character" form, and the conversion of \<ESCAPE\> to \` (accent grave) or to $ (dollar sign) are suppressed. This mode is useful for driving displays. |
| ET&2 | Process DELETEs and \<CTRL/U\>s by using "scope mode". Scope mode processing uses the cursor control features of monitors to handle character deletion by actually erasing characters from the screen. |
| ET&4 | Read lower case. TECO normally converts all lower case alphabetics to upper case on input. Setting this bit causes lower case alphabetics to be input as lower case. TECO commands and file specifiers may be typed in either upper or lower case. For the purpose of searches, however, upper and lower case may be treated as different characters. (See ^X flag). |
| ET&8 | Read without echo for ^T commands. This allows data to be read by the ^T command without having the characters echo at the terminal. Normal command input to TECO will echo. |
| ET&16 | Unused. |
| ET&32 | Read with no wait. This enables the ^T command to test if a character is available at the user terminal. If a character has been typed, ^T returns the value of the character as always. If no character has been typed, ^T immediately returns a value of -1 and execution continues without waiting for a character. |
| ET&64 | Detach TECO from the user's terminal, but continue execution of the command string. An error will be issued if this feature is not supported in the current operating environment. <br><br> While detaching with open files should work in at least some environments, it is safer and more portable to detach from a terminal before executing EB, ER, EW, or other commands that open files. However, this does not apply to an EI command, since the data from any indirect command file is read, and the file closed, before execution of the commands contained therein. |
| ET&128 | "Abort-on-error" bit. Initially set, when TECO starts up; cleared each time TECO issues its asterisk prompt. When this bit is set: 1) all informational messages are suppressed, 2) any \<CTRL/C\> causes the immediate termination of TECO, and 3) any error causes the termination of TECO after the error message is printed. |
| ET&256 | If this bit is set, all lines output to the terminal are truncated to the terminal’s width if needed. |
| ET&512 | If this bit is set, the display mode feature of TECO is present and your terminal is able to use display mode. This bit is a read-only bit; its state cannot be altered. |
| ET&1024 | Unused. |
| ET&2048 | Unused. |
| ET&4096 | This bit reflects the terminal is capable of handling eight-bit character codes. <br><br> Because the data manipulated (edited) by TECO can consist of all 256 possible byte codes, the way data characters are displayed (typed out) at the terminal varies depending upon the setting of the 4096 ET bit. <br><br> Unprintable or invalid codes in the 128 to 255 range are typed out as [ab] (where ab is the corresponding hexadecimal code). The display mode feature (controlled by the W commands) always uses the \<xy\> and [ab] notations. |
| ET&8192 | Accent grave as ESCape surrogate. If this bit is set, TECO recognizes the \` (accent grave) character as an ESCAPE surrogate. That is, an ` character typed at the terminal will be recognized as a command \<DELIM\> character, and passed to TECO as an ESCape. (This interpretation applies only to \<DELIM\>s typed at the terminal; ESCape characters must still be used in macros and indirect files.) When an ESCape surrogate is set, an ESCape received by TECO echos as accent grave; when none is set, an ESCape received by TECO echos as dollar sign. This feature is provided for the benefit of certain newer terminals which lack an ESCape key. (See also the Introduction, and the EE flag.) |
| ET&16384 | Unused. |
| ET&32768 | If this bit is set and a \<CTRL/C\> is typed, the bit is turned off, but execution of the current command string is allowed to continue. This allows a TECO macro to detect typed \<CTRL/C\>s. |

When TECO is initialized, ET&2, ET&4, ET&512, ET&4096, and ET&8192 are set,
and all other bits are clear. Furthermore, ET&128 and ET&32768 are always
cleared before each prompt.

### EU - Upper/Lower Case Flag

This flag controls whether certain characters are marked when output. It was originally intended for terminals or monitors that were unable to display both upper and lower case characters, and its use is primarily historical. By default, it is not enabled in TECO-64, but can be configured if desired. If it is not configured, any attempt to execute the command will result in a ?CFG error.

| Command | Function |
| ------- | -------- |
| -1EU | No case flagging of any type is performed on type out, and lower case characters are output as lower case characters. |
| 0EU | Lower case characters are flagged by outputting a \’ (quote) before the lower case character, and the lower case character is output in upper case; upper case characters are unchanged. |
| 1EU | Upper case characters are flagged by outputting a \’ (quote) before each character, and then the upper case character is output; lower case characters are output as their upper case equivalents. |

The initial value of the EU flag is -1.

### EV - Edit Verify Flag

Before TECO prints its prompt, it checks its prompt to determine whether any lines
should be output to the terminal, according to the table below.

| Command | Function |
| ------- | -------- |
| 0EV | Do not print any lines before the prompt. |
| *n*EV | If *n* is between 1 and 31, the current line is typed out with a line feed immediately following the position of the pointer to identify its position. If *n* is between 32 and 126, the current line is typed out with the ASCII character corresponding to the value of n immediately following the position of the pointer to identify its position. |
| *m*,*n*EV | Same as *n*EV, but prints *m* lines before and after the current line. <br><br>For compatibility with older TECOs, this command may also be specified in the form (*m*+1)\*256+*n*EV. |

The initial value of the EV flag is 0.

### ^X - Search Mode Flag

| Command | Function |
| ------- | -------- |
| 0^X | The text argument in a search command will match text in the text buffer independent of case in either the search argument or the text buffer. The lower case alphabetics match the upper case alphabetics, and "`", "{", " | ", "}", "~" match "@", "[", "\ ", "]", "^" respectively. |
| -1^X | The search will succeed only if the text argument is identical to text in the text buffer. (This means that lower case does NOT match upper case). |
| 1^X | The text argument in a search command will match text in the text buffer independent of case in either the search argument or the text buffer. |

The initial value of the ^X flag is 1. (For control of caret or
uparrow treatment in search strings, see the ED&1 flag bit.)
