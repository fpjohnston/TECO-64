### Glossary of Terms

**Accent grave**

The ` character (ASCII 96). Echoed for TECO’s text and command string delimiter when an
ESCape surrogate is in effect. Actually set as the ESCape surrogate by the 8192ET
flag bit, or EE flag.

**Abort-on-error bit**

The 128ET flag bit. If this bit is set, then TECO will abort execution on
encountering an error (after printing the error message) and control returns to
the operating system. This bit is initially set when TECO starts up, but is reset
whenever TECO issues its prompt.

**Anchored search**

A search (S) or search and replace (FS) command that is preceded by a double
colon (::). This indicates that the search must match the characters
immediately following the current pointer position. If these characters do
not match, no further searching is performed and the text buffer pointer
does not move.

**Argument pair**

A command of the form m,n where m and n are numbers (or TECO commands
that return numbers). When used before a command that normally acts on lines,
the argument pair causes the command to work on characters. The characters
specified by this argument pair is the set of characters between pointer
positions m and n. The number m should normally be less than or equal to n.

**@-sign modified command**

A command that is preceded by an @-sign modifier.

**@-sign modifier**

An at-sign (@) that proceeds a command. It indicates to TECO that the string
argument to the command is to delimited on both sides by a user-supplied
delimiter rather than to be delimited only at the end by a TECO-designated
delimiter (normally \<ESCAPE\>).

**Automatic type-out**

The feature of TECO that causes lines of text to be automatically typed out.
The ES flag may be used to control the automatic type out of lines after search
commands, and the EV flag may be used to cause automatic type out of lines
after command execution.

**Auto-trace mode**

A mode that TECO goes into when the 4EH flag bit is set.
In this mode, TECO will automatically print out the erroneous command string
after an error occurs.

**Backup protection**

The process of preserving the user’s original file (as a backup) when editing that
file with the EB (Edit with Backup) command.

**Backwards searches**

A search that proceeds in the backwards direction. If the string being looked for
does not occur at the current pointer position, the pointer position is moved back
one character and the search is tried again. This continues until the string is
found or until the boundary of the search has been reached or until the beginning
of the current text buffer has been reached. Backward searches are initiated by
using a negative argument to a search command or by using an argument pair
m,n with m greater than n to an FB or FC command.

**Bounded searches**

A search command that requires searching only a portion of the text buffer. Of
particular importance is the case where you only want to search the current line
for a given string. Bounded searches are accomplished using the FB command.

**Case flagging**

A mode of TECO wherein, on type out, it will flag alphabetic characters (in either
upper or lower case) by preceding them with a single quote. Lower case flagging
is particularly useful on terminals that do not display lower case. Case flagging
is controlled by EU, the case flag. Setting EU to 0 sets lower case flagging mode;
setting EU to 1 sets upper case flagging mode, and setting EU to -1 removes all
case flagging.

**Character-oriented editor**

An editor that allows modification of single characters, so that if just one
character of a line is wrong, the entire line does not have to be retyped. TECO is
a character-oriented editor (although it has a number of facilities for dealing with
text lines, too).

**Colon-modified command**

A command that is preceded by a colon (:) modifier.

**Colon-modifier**

A colon preceding a TECO command, used to indicate that the action of the
command is to change. Frequently indicates that the command is to return a
value (-1 if the command succeeded, 0 if the command failed).

**Command line**

The current line of the command string that is being typed into TECO.

**Command string scope editing**

The feature of TECO that is enabled when editing is performed on a display
terminal. In this mode, typing the immediate action \<DELETE\> command causes
the character deleted from the command string to physically disappear from the
screen. Other commands, such as \<CTRL/U\> also behave differently, taking best
advantage of the properties of a video terminal.

**Command string**

The string of TECO commands that is currently being typed into TECO, or is
currently being executed by TECO.

**Comment**

An informative message used within a TECO program, to make the code more
readable. The comment explains the meaning and purpose of the associated
TECO commands. The comment is enclosed in exclamation marks.

**Conditional**

A TECO language construct used to specify code to be conditionally executed
depending upon the results of some test. The most general form of the TECO
conditional is n"X \<then-clause\> | \<else-clause\> ’ which tests the number n
using condition X. The commands specified by the
\<then-clause\> are executed if the condition succeeds, otherwise the commands
specified by the \<else-clause\> are executed.

**Control-character**

An ASCII character whose octal code is in the range 0-31. Usually denoted in
this manual by \<CTRL/X\> where X is the character whose ASCII code is 100
(octal) greater than the ASCII code for the control character being represented.
TECO displays such a character as ^X (Caret-X) except for \<TAB\>, \<LF\>, \<VT\>,
\<FF\>, and \<CR\> which have their normal display, and \<ESC\> which is displayed
as $ (dollar sign) or, if a user-designated ESCAPE surrogate is in effect, as `
(accent grave). Anytime a single control-character, \<CTRL/X\> is a valid TECO
command, the two-character sequence, ^X, may be used instead.

**\<CTRL/C\> trapping**

A mode of operation wherein a TECO macro will regain control (rather than
the TECO interpreter) when \<CTRL/C\> or \<CTRL/C\>\<CTRL/C\> is typed on
the user’s terminal to abort execution of the current command string.
\<CTRL/C\> trapping is enabled by setting the high-order bit of the ET flag.

**Current character**

The character immediately following the current text buffer pointer position.

**Cursor**

A visible pattern on a display terminal (usually blinking) that would specify
to a true scope editor (like VTEDIT) the current location of the text buffer
pointer, or which would specify the location where subsequently typed
characters would be displayed.

**\<DELIM\>**

Terminology in this manual for an ESCape typed at the terminal and passed
to TECO. On newer terminals, there may be no ESCape key. Some versions of
TECO provide for an "ESCape surrogate", a user-designated key which is to cause
transmission of an ESCape to TECO. The term \<DELIM\> is used to indicate
an ESCape transmitted to TECO whether it was generated by the designated
"surrogate" key or by a "real" ESCape key. An ESCape types out as ` when a
surrogate is active, but as $ when no surrogate is active.

**Destructive search**

A form of global search in which pages of text that are passed over (because they
did not contain the string being searched for) are discarded rather than written
to the output file. Destructive searches are initiated in TECO via use of the
_ command.

**Display**

The portion of the text buffer that is currently being displayed on the user’s
terminal or auxiliary display device.

**Dot**

A mnemonic for "the current text buffer pointer position". Stems from the fact
that the . (dot, or period) TECO command returns this number as its value.

**E command**

One of several two-character TECO commands that start with the letter ’E’.

**Echo mode**

A normal mode of operation in which TECO will automatically echo (display) each
character that is typed in response to a ^T command. Opposite of no-echo mode.
This mode is controlled by the 8ET flag bit. (0 means no-echo mode.)

**ED flag**

The edit level flag.

**Edit level flag**

A bit-encoded flag, referenced by the ED command, that describes how TECO
should behave with reference to certain features. See Section 5.16 for more
details.

**Edit verify flag**

A flag that describes how TECO should act after processing of a command string.
This flag can be set so that TECO will display the line just edited on the terminal
after each command. See Section 5.16 for more details.

**EH flag **

The help level flag.

**Either-case search mode**

A standard mode of operation in which alphabetic
characters specified within a search string are permitted to match a
corresponding character of either upper or lower case.

**Else-clause**

The part of a conditional command that is executed if the condition is not
satisfied. In TECO, these are the commands that occur between the | and the ’
characters within the conditional construct.

**End-of-file flag**

A read-only flag, referenced by the ^N command that specifies whether or not
end-of-file has been seen on the currently selected input stream. A value of 0
means that end-of-file has not been seen; a value of -1 means that end-of-file has
been reached. This flag is initially 0 and is reset to 0 each time a new file is
selected for input.

**EO level**

The current version level of TECO-64.

**ES flag**

The search verification flag.

**\<ESCAPE\>**

ASCII character code 27. It is a general-purpose delimiter
used by TECO. Traditionally known as an \<ALTMODE\>. When no ESCape
surrogate is active, an ESCape types out as $ (dollar sign). See also \<DELIM\>.

**ESCape surrogate**

A character (designated via the 8192ET flag bit, or via the EE flag) which causes
an ESCape to be transmitted to TECO. When an ESCape surrogate is active, an
ESCape types out as ` (accent grave). See also \<DELIM\>.

**ET flag**

The terminal characteristics flag.

**EU flag**

The upper/lower case flag.

**EV flag**

The edit verify flag.

**Exact-case search mode**

A mode of operation in which alphabetic characters within a search string must
match the exact case (upper case or lower case) of characters specified.

**Exit Protection**

A protective feature of TECO that prevents a user from exiting TECO if a
potential los of data is imminent. The EX and EG commands are aborted (with
the ?NFO error message) if there is text in the text buffer, but no output file is
open.

**F command**

One of several two-character TECO commands that start with the letter ’F’.

**Flag**

A data register used by TECO to control various modes of operation. The
contents of the flag are set by specifying the new value before the flag name; and
the contents are returned by using the flag name without a numeric argument.
The TECO flags are: ^X, ^E, ^N, ED, EE, EH, EO, ES, ET, EU, and EV.

**Flow command**

A TECO command that is used to flow (branch) to a particular flow control
character. The flow commands are F\<, F\>, F’, and F | .

**Form feed flag**

A read-only flag, referenced by the ^E command that specifies whether the
previous append from the input file terminated because of encountering a form
feed character in the input file, or because the text buffer became nearly full. The
value of this flag is automatically set to -1 by TECO if input is terminated by the
presence of a form feed character in the input file.

**Global Q-register**

A Q-register available throughout all macro levels (including "outside", or
at prompt level). Global Q-registers are named A-Z and 0-9. See also Local
Q-Registers.

**Global search**

A type of search that continues through the entire input file until the specified
characters are found. Successive pages of the file are read into the text buffer
and then written out into the output file (if the string is not located). Global
searches in TECO are initiated via the N command.

**Hard-copy editing mode**

A mode of operation that TECO uses when the user’s terminal is not a display.
In this mode, when a character is deleted using the \<DELETE\> key, the
deleted character is re-typed on the terminal as a visible indication that this
character was deleted. Opposite of scope editing mode. This mode can be
entered, even on a scope terminal, by turning off the 2ET flag.

**Help level flag**

A bit-encoded flag, referenced by the EH command, that controls properties of
TECO having to do with error messages and user assistance.

**Immediate command**

A special command to TECO that takes immediate effect, requiring no \<DELIM\>s
to begin its execution.

**Immediate action editing characters**

Commands such as \<DELETE\> and \<CTRL/U\>, which perform editing of the
TECO command string currently being typed in.

**Immediate inspection commands**

Single-character commands which can be typed as the very first character
after TECO’s prompting asterisk, and which cause the current location counter
to be moved by one line and the new current line to be typed out. Examples are
\<LF\> and \<BS\>. See Section 1.6.1.

**Immediate action commands**

Commands which may be typed right after TECO’s prompting asterisk,
such as the \*q to save the previously-typed command in a Q-register or the ^W to
re-display a scrolling buffer window. See Section 4.2.

**Immediate ESCape sequence commands**

These are invoked by keys which generate escape sequences, such as function or
cursor or keypad keys. Such commands are usually used to
perform editing of the TECO command string currently being entered into TECO.
For example, the commands \<DELETE\> and \<CTRL/U\> are immediate action
commands.

**Iteration**

A language construct that permits a sequence of operations to be re-executed
indefinitely or a given number of times. In TECO, this is accomplished by
enclosing the commands in angle brackets.

**Line**

A portion of text delimited by \<LF\>, \<VT\>, \<FF\>, or the beginning or end of the
text buffer. The final delimiter is considered to be part of the line.

**Line-oriented editor**

An editor that primarily uses line numbers to direct editing, and most of whose
commands are line-oriented. TECO is a character-oriented editor, but also has
many facilities that work with lines.

**Line-wrap mode**

A standard mode of TECO’s window support wherein lines that are too long to
fit on a single physical line of the user’s terminal are automatically
continued on the next line. Opposite of truncate mode.

**Literal type-out mode**

A mode that TECO can be put into by setting the 1ET flag bit, or (for a
single character on a one-shot basis) by using the :^T command. In this mode,
any characters typed by a TECO program via use of one of the commands T,
V, ^A, or :G, will be displayed on the user’s terminal without any modification.
When not in this mode, TECO will convert characters that normally do not print
to a form that can be displayed on the user’s terminal (e.g. \<CTRL/X\> displays as
^X and \<ESCape\> displays as $ or, if a user-designated ESCape surrogate is in
effect, as `). Literal type out mode is useful when trying to do real-time displays
on a display terminal. Normal (up-arrow) mode is particularly useful to let you see
what characters are really in your file.

**Local Q-register**

A Q-register available to only a single macro level (including "outside", or at
prompt level). The local Q-registers for a particular macro level are automatically
saved and restored by TECO around execution of a lower-level macro. Local
Q-registers are named .A-.Z and .0-.9. See also Global Q-Registers.

**Log file**

An audit trail that TECO-10 can keep showing all the commands that were typed
to TECO and/or all the type out made by TECO. This is useful for reviewing what
went wrong with a ’bad’ edit. The log file is initiated with the EL command (see
Appendix C).

**Macro**

A sequence of TECO commands intended to be loaded into a Q-register and
executed as a TECO program.

**Macro level**

Two commands within the same TECO macro are said to be at the same macro
level. When one TECO macro calls another, the calling macro is said to be at the
higher macro level, and the called macro at the lower macro level.

**Match control construct**

A command, consisting of certain special characters, used within a search string
to indicate to TECO that special groups of characters are permitted to match at
this point.

**Memory expansion**

TECO’s act of acquiring additional storage from the operating system when
the currently allocated storage is insufficient to handle the current TECO
command. Typically, TECO will attempt to acquire this additional memory before
it completely runs out of memory, so as to allow a ’buffer zone’ for the user. This
allows him to complete a few more commands even in the case where TECO
is unable to get more memory. The informative message "[nK Bytes]" or its
equivalent is printed on the terminal informing the user that memory usage has
expanded.

**Mung**

A recursive acronym for "Mung Until No Good"; an act applied by novice TECO
users to their source files.

**No-echo mode**

A mode of operation in which TECO will not automatically echo (display) the
character typed by the user in response to the ^T command. This mode is entered
by setting the 8ET flag bit. Opposite of echo mode.

**Page**

A portion of text delimited by form feeds. The form feeds are not considered to
be part of the page. Sometimes the term ’page’ is used to refer to all the text
currently in the text buffer.

**Pipeline editor**

An editor which only makes sequential edits to a file. The file to be edited is
read into the text buffer one piece at a time. Each piece is edited and then
written out. Once a piece has been written out, further editing to that piece is
not possible unless the output file is re-opened in a later edit as a new file to be
edited. TECOs are pipeline editors.

**Pointer preservation mode**

A mode of operation in which the text buffer pointer will not change after a
failing search. This mode is controlled by the 16ED flag bit.

**Primary input stream**

A term used by TECO to refer to the main input file that TECO is using.

**Primary output stream**

A term used by TECO to refer to the main output file that TECO is using.

**Prompt level**

A TECO command is said to be executed from prompt level if it was typed in
directly in response to TECO’s prompt, as opposed to being executed from a
macro.

**Q-register**

One of 36 global or 36 local user-accessible registers provided by TECO. Each
Q-register can hold both a number and a string of text. Of particular importance
is the ability to store TECO command strings in Q-registers to be used as
"macros".

**Q-register push down list**

A last-in first-out stack available to users for saving and restoring the contents of
Q-registers.

**Read-with-no-wait mode**

A mode of operation in which the ^T command will not hang until a key is typed
on the user’s terminal. In this mode, if no key has been struck, TECO returns a
-1 as the value of the ^T command. This mode is entered by setting the 32ET flag
bit.

**Scrolling**

A form of TECO operation available on display terminals capable of supporting
split-screen operation. The bottom n lines are used for TECO’s asterisk prompt
and the usual typed command lines; the remaining lines above are used for an
automatically-updated window into the text buffer. A valuable training aid: one
can experiment with TECO commands and immediately observe their effects.
Controlled by the n,7:W command.

**SEEALL mode**

A mode of window operation in which all characters have a distinctive visible
display, including characters such as \<TAB\>, \<CR\>, and \<LF\>, which normally
do not print. Controlled by the 3:W flag.

**Search verification flag**

A flag, referenced by the ES command, that controls the action of TECO
subsequent to the execution of a command string containg a search command.
Proper setting of this flag will enable the user to verify that the search located
the correct string, by having the line containing the string found displayed on the
terminal.

**Search mode flag**

A flag, referenced by the ^X command, that controls how TECO treats
alphabetical characters within search strings. This flag is used to put TECO into
either exact-case mode, or either-case mode. If the ^X flag is set to 0, then either
case matches in searches. If the ^X flag is set to 1, then exact case matches are
required.

**Secondary input stream**

A term used by TECO-11 to refer to an auxiliary input "channel" that was formed
by use of the EP command.

**Secondary output stream**

A term used by TECO-11 to refer to an auxiliary output "channel" that was
formed by use of the EA command. See Section 5.1.4 for details.

**Scope editing mode**

A mode of TECO in which command line scope editing (q.v.) algorithms are
used. This mode is enabled by setting the 2ET flag bit. It is usually
automatically enabled by TECO if the operating system can detect that the user
has invoked TECO from a scope terminal. Opposite of hard-copy editing mode.

**Split Q-registers**

The feature of TECO that permits storing of both a number and a string within a
Q-register. Each Q-register can be considered to consist of two compartments.

**String build construct**

A command, consisting of special characters, used within a text argument to
build up the contents of the text argument from other components.

**Tag**

A label specified within exclamation marks to mark a point within a TECO
program. Control can be transferred to this point by the use of a GOTO (Otag`)
command.

**TECO**

Text Editor and COrrector program.

**TECO’s prompt**

Refers to the asterisk (\*) that TECO prints to indicate that it is ready to accept
commands.

**Terminal characteristics flag**

A bit-encoded flag, referenced via the ET command, that contains information
about the user’s terminal terminal and specifies in what manner TECO should
support it.

**Text buffer**

The main buffer used by TECO to hold the text to be edited.

**Text buffer pointer**

A pointer that marks the position within the text buffer where TECO is currently
’at’. This pointer always points to positions between characters; it never points at
a character. The current character is considred to be the character immediately
to the right of the current position of the text buffer pointer.

**Then-clause**

The set of commands within a conditional that are executed if the condition is
satisfied. In TECO, these commands immediately follow the "X at the start of the
conditional. They are terminated by a | or ’ character.

**Tracing**

The act of watching the command-by-command execution of a TECO program.
This is accomplished by putting TECO into trace mode, via use of the ? command.

**Trace mode**

A mode of TECO wherein each command that is executed by TECO is also
displayed on the user’s terminal as it is being executed. This mode is useful for
debugging complicated TECO programs. TECO is toggled in and out of this mode
via use of the ? command.

**Truncate mode**

A mode of TECO’s window support wherein lines that are too long to fit on a
single physical line of the user’s terminal are truncated when the right margin of
the scope is encountered. This mode is entered by setting the 256ET flag bit.
Opposite of line-wrap mode.

**Type-out-time command**

A special command that makes sense only while TECO is typing out text on the
terminal. These commands are \<CTRL/S\>, \<CTRL/Q\>, and \<CTRL/O\> and affect
the type out.

**Up-arrow mode**

A standard mode of operation wherein upon type out, TECO will display control
characters by typing the visible two-character sequence ^X to represent the
control character \<CTRL/X\>. On many older terminals, the caret character (^),
whose octal ASCII code is 136, prints as an up-arrow. Some control characters
are not printed in up-arrow mode, notably \<TAB\>, \<LF\>, \<VT\>, \<FF\>, and \<CR\>.

**Upper/lower case flag**

A flag, referenced by the EU command, that specifies whether or not case flagging
is to occur. If set to -1, no case flagging occurs. If set to 0, lower case characters
are flagged on type out. If set to +1, upper case characters are flagged on type
out.

**War and Peace mode**

A mode of operation in which TECO outputs a large informative paragraph of
information automatically upon encountering an error in a user’s program. This
paragraph of information describes the error in painstaking detail and tells the
user how to recover. This mode is entered by setting the help level to 3. This
mode is particularly useful to novices and particularly obnoxious to experts.

**Yank protection**

A feature of TECO wherein any Y, _, or F_ command that will potentially destroy
valid data is aborted. This mode is normally enabled and will cause any of the
aforementioned commands to abort with the ?YCA error message if there is text
in the text buffer and an output file is open. This feature can be disabled by
turning off the 2ED flag bit.

