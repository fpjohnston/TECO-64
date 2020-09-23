### @ - Text argument modifier

Many TECO commands take a text (character string) argument. The string
is placed immediately after the command and consists of a sequence of ASCII
characters terminated by a <DELIM> character (or in the case of ! and ^A
commands, by the command character). The string of ASCII characters may not
include a <DELIM>, since this would terminate the string prematurely, but may
include any other character.

Examples:

S*abc*$
- Search for the string *abc*.

^UA*HELLO*$
- Insert the text *HELLO* into Q-register A.

O*BEGIN*$
- Branch to the tag specified by the string *BEGIN*.

Some TECO commands require two text arguments. Each argument must be
followed by a <DELIM> character.

Example:

FS*abc*$*def*$
- Replace string *abc* by *def*.

You can include a <DELIM> character in a text string by using another
format of text argument. In this alternate form, the string is delimited
on both sides by any ASCII code that does not otherwise appear in the
string. You signal that this format is being used by inserting an @
character before the command.

Examples:

@ER/*hello.c*/
- Open the file *hello.c* for input. The delimiter used is */*.

@^A+*Hello, world!*+
- Type the message *Hello, world!* on the terminal. The delimiter
used is *+*.

Unlike other TECOs, delimiters must be graphic ASCII characters; control
characters are not allowed. Also, spaces are allowed between a command
and the text argument delimiter.

Example:

@S /*foo*/
- Equivalent to @S/*foo*/.

