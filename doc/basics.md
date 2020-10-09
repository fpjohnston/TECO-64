### Basics of TECO

TECO may be called from command level by typing the appropriate command,
followed by a carriage return. TECO will respond by printing an asterisk
at the left margin to indicate that it is ready to accept user commands.

A TECO command consists of one or two characters which cause a specific
operation to be performed. Some TECO commands may be preceded or followed
by arguments. Arguments may be either numeric or textual. A numeric
argument is simply an integer value which can be used to indicate, for example,
the number of times a command should be executed. A text argument is a
string of ASCII characters which might be, for example, words of text or a file
specification.

If a command requires a numeric argument, the numeric argument always
precedes the command. If a command requires a text argument, the text
argument always follows the command. Each text argument is terminated by
a special character (usually a \<DELIM\> character, which TECO hears as an
ESCape - see Introduction). This indicates to TECO that the next character
typed will be the first character of a new command.

TECO accumulates commands as they are typed in a command string, and
executes commands upon receipt of two consecutive \<DELIM\> characters.
\<DELIM\> may be any character you select (if your TECO and operating system
support user-selectable ESCape surrogates - see sections on ET and EE flags).

When you type the character you have designated as \<DELIM\>, TECO receives
an ESCape character, and an ` (accent grave) is echoed. (If you are not using
an ESCape surrogate — that is, you are actually pressing an ESCape key — a dollar
sign is echoed.) The accent grave character is used in examples throughout
this manual to represent typed \<DELIM\>s. Note that the carriage return