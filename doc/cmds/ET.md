### ET - Terminal Flag

The ET flag is a bit-encoded word controlling TECO’s
treatment of the terminal. Any combination of the individual
bits may be set. The bits provide the following functions, when
set:

| Bit | Function |
| --- | -------- |
| ET&1 | Type out in image mode. Setting this bit inhibits all of TECO’s type out conversions. All characters are output to the terminal exactly as they appear in the buffer or ^A command. For example, the changing of control characters into the "caret/character" form, and the conversion of \<ESCAPE\> to \` (accent grave) or to $ (dollar sign) are suppressed. This mode is useful for driving displays. |
| ET&2 | Process DELETEs and \<CTRL/U\>s by using "scope mode". Scope mode processing uses the cursor control features of monitors to handle character deletion by actually erasing characters from the screen. |
| ET&4 | Read lower case. TECO normally converts all lower case alphabetics to upper case on input. Setting this bit causes lower case alphabetics to be input as lower case. TECO commands and file specifiers may be typed in either upper or lower case. For the purpose of searches, however, upper and lower case may be treated as different characters. (See ^X flag). |
| ET&8 | Read without echo for ^T commands. This allows data to be read by the ^T command without having the characters echo at the terminal. Normal command input to TECO will echo. |
| ET&16 | Unused by TECO-64. |
| ET&32 | Read with no wait. This enables the ^T command to test if a character is available at the user terminal. If a character has been typed, ^T returns the value of the character as always. If no character has been typed, ^T immediately returns a value of -1 and execution continues without waiting for a character. |
| ET&64 | Unused by TECO-64. |
| ET&128 | "Abort-on-error" bit. Initially set, when TECO starts up; cleared each time TECO issues its asterisk prompt. When this bit is set: 1) all informational messages are supressed, 2) any \<CTRL/C\> causes the immediate termination of TECO, and 3) any error causes the termination of TECO after the error message is printed. |
| ET&256 | If this bit is set, all lines output to the terminal are truncated to the terminal’s width if needed. |
| ET&512 | If this bit is set, the display mode feature of TECO is present and your terminal is able to use display mode. This bit is a read-only bit; its state cannot be altered. |
| ET&1024 | Unused by TECO-64. |
| ET&2048 | Unused by TECO-64. |
| ET&4096 | This bit reflects the terminal is capable of handling eight-bit character codes. <br><br> Because the data manipulated (edited) by TECO can consist of all 256 possible byte codes, the way data characters are displayed (typed out) at the terminal varies depending upon the setting of the 4096 ET bit. <br><br> Unprintable or illegal codes in the 128 to 255 range are typed out as [ab] (where ab is the corresponding hexadecimal code). The display mode feature (controlled by the W commands) always uses the \<xy\> and [ab] notations. |
| ET&8192 | Accent grave as ESCape surrogate. If this bit is set, TECO recognizes the \` (accent grave) character as an ESCAPE surrogate. That is, an ` character typed at the terminal will be recognized as a command \<DELIM\> character, and passed to TECO as an ESCape. (This interpretation applies only to \<DELIM\>s typed at the terminal; ESCape characters must still be used in macros and indirect files.) When an ESCape surrogate is set, an ESCape received by TECO echos as accent grave; when none is set, an ESCape received by TECO echos as dollar sign. This feature is provided for the benefit of certain newer terminals which lack an ESCape key. (See also the Introduction, and the EE flag.) |
| ET&32768 | If this bit is set and a \<CTRL/C\> is typed, the bit is turned off, but execution of the current command string is allowed to continue. This allows a TECO macro to detect typed \<CTRL/C\>s. |

The initial setting of ET is as follows:

| Bit | Function |
| --- | -------- |
| ET&1 | Clear |
| ET&2 | Set |
| ET&4 | Set |
| ET&8 | Clear |
| ET&32 | Clear |
| ET&128 | Clear, and reset before each prompt |
| ET&256 | Clear |
| ET&512 | Set |
| ET&4096 | Set |
| ET&8192 | Set |
| ET&32768 | Clear, and reset before each prompt |

