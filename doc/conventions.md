### TECO-64 - Conventions and Structures

#### TECO character set

TECO accepts the full 8-bit ASCII character set. For more information on
TECO’s handling of 8-bit characters (including on terminals capable of
displaying only 7-bit data), see the description of the 4096 ET bit.

When TECO is maintaining a window into the edit buffer, a special facility (called
"SEEALL" or "View All" mode) is available to explicitly show certain characters
which normally do not print, such as \<CR\>, \<FF\>, and \<TAB\>. This mode is
controlled by the 3:W flag, which is described in Section 5.17.

TECO command strings may be entered using upper case characters (as shown
throughout this manual) or lower case characters. For example, the commands
MQ, mQ, Mq, and mq are executed identically. A file containing upper and
lower case text can be edited in the same way as a file containing upper case
only, although this may be clumsy to do from an upper case only terminal.
TECO can be set to convert lower case alphabetics to upper case as they are
typed in; commands to enable or disable lower case type-in will be presented in
Section 5.16.

Control characters are generally echoed by TECO as a caret or up-arrow followed
by the character. Some control characters, such as \<CTRL/L\> (form feed) and
\<CTRL/G\> (bell) echo as the function they perform. In many cases, you can type
a control character as a caret (up-arrow) followed by a character, and it will be
treated as if it had been entered using the control key.

There are exceptions to the interchangeable use of the CONTROL key and the
caret. When a control character is used as the delimiter of a text string (as
explained in Section 3.2.2 below), it must be entered in its \<CTRL/x\> form.
This form must also be used if the control character is the second character of a
two-character command, or is being entered as an immmediate action command.
Since certain control characters have special meaning in text arguments, some
of them (such as \<CTRL/N\> and \<CTRL/X\>), must be entered into a text string
using the CONTROL key and preceded by \<CTRL/Q\>, ^Q, \<CTRL/R\>, or ^R.

#### TECO Command Format

Each TECO commands consists of one, two, or three characters.

TECO commands may be strung together (concatenated) into a command string.
If a command in the string returns numeric value(s), it or they will be passed to
the following command.

- Note: Most commands which return a value are expressly designed for that
purpose. A few commands (such as n%q and m,n:W, q.v.) can be used
to cause an action, and also return a value. Good programming practice
suggests following such a command with \<DELIM\> or caret-[ if the
returned value is not intended to affect the following command.

A TECO command may be preceded by one or two numeric arguments. Some
TECO commands may be followed by a text argument. Many TECO commands
can be modified by "@" (see Section 3.2.2) and/or by ":" (see Section 3.2.3).

When TECO commands are concatenated into a command string, no delimiter is
necessary between commands except for one necessary to delimit a text argument
or one as suggested in the note above.

TECO commands are accumulated into a command string as they are typed. The
command string is executed when it is terminated by typing two consecutive
\<DELIM\> characters. TECO then executes the commands in the order in which
they appear in the command string, until the string is exhausted or an error
occurrs. (Some TECO commands cause branching or looping, so altering the order
of execution.)

TECO’s \<DELIM\> character is (internally) the ESCape (octal 33). Because
newer terminals use the ESCape character as part of control sequences (and
some terminals no longer even possess an ESCape key), provision is made
for you to designate a key on your terminal to cause an ESCape to be sent to
TECO—that is, to act as an ESCape surrogate. (See the EE flag and the 8192 ET
bit, Section 5.16.) Because the key you press to delimit text strings and TECO
commands may be the ESCape key or may be the surrogate, this manual uses the
indirect term \<DELIM\> to mean whichever one you are using. When an ESCape
surrogate is active, the character you see is \`. (When no surrogate is active, you
see $.) This manual uses the \` character.

##### Numeric Arguments

Most TECO commands may be preceded by a numeric argument. Some numeric
arguments must be positive; others can be negative or zero. The maximum size of
any numeric argument is restricted to a negative value of -2\^63 to a positive 
value of 2^64-1.

Exceeding these ranges of values can cause unpredictable results. So can using a
negative argument with a command that takes only an unsigned argument.
Numeric arguments can be used in the following ways:

- Character commands such as J, C, R, and D take a single numeric argument
which represents the number of characters that the command is to act on.
- Such commands as P, PW, and \< perform an action that can be repeated. The
numeric argument is the repetition count.
- Some commands, such as ED, ET, ^E, ^X, ES, EU, and EV, control the
setting of variables called flags. When a numeric argument is specified, the
value of that argument becomes the new value of the associated flag. When
no numeric argument is specified, these command return the value of the
associated flag.
- Line commands such as T, K, X, FB, and FC operate on lines. They take zero,
one, or two numeric arguments. If one argument (n) is specified, it represents
the number of lines over which the command is to have effect, beginning
at the current buffer pointer position. A positive (non-zero) n affects a text
running from the current pointer position to the nth following line delimiter.
A negative n affects a text running from the pointer back to the beginning
of the line containing the nth previous line delimiter. When n is zero, the
affected text runs from the beginning of the current line to the current pointer
position. When n is omitted, n = 1 is assumed.

When a line command contains two numeric arguments (m,n), these represent
the pointer positions between which text is affected. Unless the description of
the command says the order of these two arguments is important, they may be
entered in either order.

When a command that normally takes an argument is specified with no
argument, TECO executes the command in the most common or most useful
way, as shown in the following table:

| Command | Default Argument | Default Action |
| ------- | ---------------- | -------------- |
| C | 1 | Advance 1 character |
| R | 1 | Back over 1 character |
| L | 1 | Advance 1 line |
| J | 0 | Jump to start of buffer |
| V | 1 | View 1 line |
| D | 1 | Delete 1 character |
| K | 1 | Kill 1 line |
| S, N, etc. | 1 | Search for first occurrence |
| % | 1 | Increment Q-register by 1 |
| X | 1 | Extract one line |

These default arguments reduce the number of keystrokes needed for common
TECO actions.

##### Text Arguments

Many TECO commands take a text (character string) argument. The string
is placed immediately after the command and consists of a sequence of ASCII
characters terminated by a \<DELIM\> character (or in the case of ! and ^A
commands, by the command character). The string of ASCII characters may not
include a \<DELIM\>, since this would terminate the string prematurely, but may
include any other character. (Some characters may be difficult to enter from a
terminal because they are TECO immediate action commands or because they
have been filtered out by the operating system).

Examples of text arguments:

Sabc\` Search for the string "abc"

^UAHELLO\` Insert the text "HELLO" into Q-register A

OBEGIN\` Branch to the tag specified by the string "BEGIN"

Some TECO commands require two text arguments. Each argument must be
followed by a \<DELIM\> character, as follows:

FSabc\`def\` Replace string "abc" by "def"

##### At-Sign Modifiers

You can include a \<DELIM\> character in a text string by using
another format of text argument. In this alternate form, the string
is delimited on both sides by any ASCII code that does not otherwise
appear in the string. You signal that this format is being used by
inserting an @ character before the command, as follows:

@ER5TEST.FOR5 Open the file "TEST.FOR" for input. The delimiter used is
"5"

@^A+Hello out there!+ Type the message "Hello out there!" on the terminal.
The delimiter is "+"

Unpredictable results will occur if another TECO command intervenes between
an @ sign and the command that it is supposed to affect. Note that a control
character used as a delimiter must be entered as \<CTRL/x\> (that is, in its
single-keystroke form).

##### Colon Modifiers

The colon (:) command modifies the action of the next command. In some cases, it
will cause the next command to return a value indicating whether it has failed or
succeeded. A zero (0) indicates that the command has failed, while a -1 indicates
that it has succeeded. The colon modifier is used this way with such commands
as :ER, :EB, :EN, :S, :N, :FS, :FN, :FB, and :FC. If the next sequential command
requires a positive argument, the -1 is interpreted as the largest possible positive
number. In other cases, such as :Gq and :=, the colon modifier changes the
meaning of the command. Unpredictable results may occur if you place a colon
directly before a TECO command that does not normally accept a colon modifier.

If both the : and the @ (string delimiter) are used with the same command, they
may be placed in either order.

#### Data Structures

A good way to begin the study of a programming language is to forget the
commands, for the moment, and concentrate instead on the data structures. This
section follows that approach, describing both the values on which TECO operates
and the buffers and registers in which these values are stored.

TECO manipulates two types of data, namely,

- The character string: a sequence of zero or more ASCII characters, and
- The integer: a numeric value that may be signed or unsigned.

The text that TECO edits is, of course, a character string. Less obviously, the
command string by which the user controls TECO is also a character string. The
counters and indices for character string manipulation, and the repetition counts
for loops are integers.

Character strings and integers have distinct internal representation and this is
reflected in the design of the TECO commands. Commands designed for character
strings do not work on integers and vice versa.

The data structures described in this section are frequently applied to character
strings. Structure is never "built into" the data, but rather is attributed to the
data by particular commands and conventions. Thus "lines" of characters are
recognized by line manipulation commands, but not by character manipulation
commands, which view an end-of-line character as just another ASCII code.

The following are definitions for the line and the page in TECO:

- Any character string can be divided into TECO lines by considering the line
to be ended by either:
  - a line feed (ASCII 10)
  - a form feed (ASCII 12)
  - a vertical tab (ASCII 11) or
  - the end of the given character string
- Any character string can be divided into TECO pages by considering the page
to be ended by either
  - a form feed (ASCII 12) or
  - the end of the given character string

These data structures are used to achieve two quite separate results: the
formatting of type out and the logical division of data.

##### Text Buffer

The main storage of TECO is the edit buffer. The edit buffer stores a single
character string that TECO edits. A edit buffer pointer is used to address text
in the buffer; it is moved about by many TECO commands. The edit buffer
pointer never points to characters in the buffer; it is always pointing at pointer
positions (between characters). The available pointer positions in the edit buffer
are sequentially numbered beginning with 0. Pointer position 0 is the position
at the start of the buffer, just to the left of the first character. Pointer position 1
is the next position, just to the right of the first character, etc. As an example,
suppose the edit buffer contains the string FOOBAR. Then seven edit buffer
pointer positions are determined as shown by the arrows in the following figure:

 F O O B A R

0 1 2 3 4 5 6

Note that there are 6 characters in the buffer and that the highest numbered
pointer position is 6. The pointer position number is equal to the number of
characters preceding that position.

Useful definitions of "current" objects are made with respect to the edit buffer
pointer as follows:

- The current character of the edit buffer is the character just to the right of
the pointer. If the pointer is at the end of the edit buffer, there is no character
to the right of the buffer pointer, and the current character does not exist.
- The current line of the edit buffer is the TECO line that contains the current
character. In the special case that the pointer is at the end of the buffer,
the current line is everything back to (but not including) the last end-of-line
character.
- The current page of the edit buffer is the TECO page that contains the
current character. In the special case that the pointer is at the end of the
buffer, the current page is everything back to (but not including) the last form
feed character (or the beginning of the buffer).

When the edit buffer pointer is at the end of the edit buffer and the last character
of the buffer is an end-of-line character, then the current line is an empty string
according to the definition just given. In this case, TECO performs the required
operation on this empty string. For example, if the pointer is at the end of the
buffer and TECO is commanded to kill (delete) the next 3 lines, then the empty
string is killed 3 times. Similarly, if the pointer is at the end of the buffer and
TECO is commanded to advance the pointer 5 lines, the pointer will remain at
the end of the buffer. No error message will be given. In a like fashion, if the
pointer is at the end of the first line of the buffer, and TECO is instructed to
extract the previous 4 lines (into a Q-register), then only the first line is extracted
since it is presumed to be preceded by 3 empty lines.

##### Q-Registers

TECO provides data storage registers, called Q-registers, which may be used to
store single integers and/or ASCII character strings. Each Q-register is divided
into two storage areas. In its numeric storage area, each Q-register can store
one signed integer. In its text storage area, each Q-register can store an ASCII
character string (which can be any text, including the important case of a TECO
command string).
There are global Q-registers and local Q-registers.

In every TECO, there are 36 global Q-registers, each of which has a one-character
name: A through Z and 0 through 9. The global Q-registers are available to all
macro levels, including "outside" macros altogether (that is, at prompt level).

There are additionally 36 local Q-registers, each of which has a two
character name: .A through .Z and .0 through .9. Effectively, there is a complete
and unique set of local Q-registers available to each and every macro level,
including prompt level. TECO automatically saves and restores a given macro
level’s local Q-registers around execution of a nested (lower level) macro. When a
macro is invoked via an Mq command (where the command is not colon-modified,
and q is a global Q-register), the current set of local Q-registers is saved and a
new set of local Q-registers created. When the macro exits, its local Q-registers
are destroyed and those of the calling macro level are restored. (If a fatal error
occurs and TECO goes back to prompt level, local Q-registers from all macro
levels are destroyed and prompt level’s set is restored.)

(A new set of local Q-registers is not created when the command is colon-modified,
as in :MA, or when the invoked macro itself resides in a local Q-register, as in
M.A—see Table 5–21.) A local Q-register name can be used in any command that
takes a Q-register name.

Various TECO commands allow the storing and retrieving of numeric values from
the numeric storage areas of the Q-registers. Other TECO commands allow the
storage and retrieval of strings from the text storage areas of the Q-registers.

##### Q-register Push-down List

The Q-register pushdown list is a stack that permits the numeric and text storage
areas of Q-registers to be saved (the [ command) and restored (the ] command).
The command sequence [A ]B replicates the text string and numeric value from
Q-register A into Q-register B. Note that macros can be written to
use local Q-registers, avoiding the need to save and restore Q-registers via the
pushdown list.

##### Numeric Values and Flags

TECO has many special numeric values and flags which are accessible through
TECO commands. Some of these values, such as the edit buffer pointer, reflect
the state of TECO. Others control TECO’s behavior in various ways.
