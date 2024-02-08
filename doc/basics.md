## TECO-64 - Command Basics

<style>
    span {
        white-space: nowrap;
    }
</style>

A general editing session with TECO might look like this:

- A user starts TECO from the command line.
- TECO prints a command prompt (usually an asterisk (\*), unless
the TECO_PROMPT environment variable is defined), indicating
that it is ready to accept one or more commands.
- The user then enters a single command, or a sequence of
multiple commands called a command string.
- TECO executes these commands and when done, prints its prompt again.
- Rinse and repeat, until a command is entered that causes TECO
to exit.

Such steps could be used in order to read an input file,
search for some desired text, make changes to it, and write out
the results to an output file.

### Edit Buffer

Editing sessions make use of an edit buffer that is used by TECO
to read in text from files, insert and delete text, search for or
replace text, and write out text to files. This buffer is initially
64K bytes, but is capable of growing to 1MB. The current contents
of an edit buffer is referred to as a *page*. Each page is usually
delimited by a &lt;FF>, and is divided into *lines*, which are
delimited by a line terminator such  as &lt;LF>, &lt;VT>, or &lt;FF>.
Lines in turn are comprised of single 8-bit ASCII *characters*.
TECO provides commands to manipulate pages, lines, and characters.

TECO has classically functioned as a *pipeline editor*, in which
text once written to a file could not be further modified without
ending the current editing session, and starting a new one to read
the file again. However, TECO-64 provides optional virtual memory
support (as did TECO-32) that makes it possible to page backward
through a file.

When editing text, TECO makes use of a buffer pointer called *dot*
that specifies the current position. This is used to determine how
commands that move the pointer or affect the text are executed.
*dot* is a movable position indicator which is never located directly
on a character, but is always either between characters, before the
first character in the buffer, or after the last character in the
buffer.

### Command Format

The general format for TECO commands is as follows:

[[ *m* ,] *n* ] [ : [ : ]] [ @ ] command [ *q* ] [ *text1* [ &lt;*delim*> ][ *text2* [ &lt;*delim*> ]]]

The fields in brackets are either optional, or are dependent on the
command being executed. These are for illustration only; none of TECO's
commands use all of these fields.

| Field | Description | Examples |
| ----- | ----------- | -------- |
| *m*   | First numeric argument. | 10,20K |
| , | Separator between *m* and *n* arguments. | 0,8192ET |
| *n*   | Second (or only) numeric argument. | 42R |
| : | Colon modifier. Affects the behavior of the following command, often in order to return a value indicating success or failure of the command rather than aborting execution and issuing an error. | \:ERinput.c\` |
| :: | Double colon modifier. Affects the behavior of the following command, but differently than :. Often used for "anchored" searches. | \:\:Sbaz\` |
| @ | At signed modifier. Specifies an alternative delimiter will be used for text arguments that follow the command, using a matched pair of characters. | \@\^A/hello/ @FC/baz/foo/ |
| command | The TECO command to be executed. | V, ET, === |
| *q* | The name of the Q-register that the command will use. | 1XA |
| *text1* | The first text argument for the command. See description below. | Ifoobaz\` |
| *text2* | The second text argument for the command. See description below. | FSfoo\`baz\` |
| &lt;*delim*> | The command delimiter. Only required for commands that allow text arguments when there is no at sign modifier. | EWoutput.c\` |

Note that if a command allows both the colon (including double colon)
and at sign modifiers, they may be specified in either order.

### Numeric Arguments

Numeric arguments are used by many commands to specify such things as
a repeat count, a position within the edit buffer, or bits to set or
clear in flag variables. Some commands limit their numeric arguments
to positive values, while others also allow zero or negative arguments.

Most commands that accept *n* arguments allow for a default if *n* is
not explicitly stated, as shown in the examples below.

| Command | Default Value | Default Action |
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

If an *m* argument is also allowed, omitting it will likely result in
different behavior than if specified. See the specific command for
details.

TECO can be built for either 32- or 64-bit support. If it has been
build for the former, then numeric arguments may range from
-2\^31 to 2\^32-1. If it has been built for the latter, then numeric
arguments may range from -2\^63 to 2\^32-1.

### Colon Modifiers

Colon modifiers change the action of the following command, as
illustrated in the examples below.

| Example Command(s) | Description |
| ------------------ | ----------- |
| :ER, :S, :FN       | Do not abort the command if it failed, but return a value of 0 to flag that an error occurred, and return -1 if it succeeded. This can be used by subsequent commands to take corrective action or print an informative message if needed. |
| :G*q*              | Type the result on the terminal instead of copying it to the text buffer. |
| :=                 | Print a numeric value, but do not print a &lt;CR>&lt;LF> after it. |

### Text Arguments

Many TECO commands allow or require a text argument, which is a
character string that follows the command and is usually terminated
by a &lt;*delim*> character. This provides additional information to
the command being executed. The string may not include a &lt;*delim*>,
since this would terminate the string prematurely, but it may
include any other character.

Commands that allow a single text argument are normally delimited as follows:

| Command | Description |
| ------- | ----------- |
| command&lt;*delim*> | General command, where &lt;*delim*> is ESCape or an alternative delimiter such as accent grave. |
| <span>\^Ahello, world!&lt;CTRL/A></span> | The type-out command may start with a CTRL/A or literal \^A, but it must be terminated by a CTRL/A.
| !tag! | Any tag or comment must be start and end with an exclamation mark. |

| Example | Description |
| ------- | ----------- |
| Sabc\`  | Search for the string "abc" |
| \^UAhello, world!\` | Insert the text "hello, world!" into Q-register A |
| OBEGIN\` | Branch to the tag specified by the string "BEGIN" |

Depending on the command, it may also be possible to use a "null" text
argument by terminating the command with a &lt;*delim*>. The effect of
doing so is dependent on the specific command, but using the
example of the S command above, if that was followed by an " S\' " command,
that would cause TECO to search for the next occurrence of "abc".

Some TECO commands allow or require two text arguments. Each argument
must be followed by a &lt;*delim*> character, as shown below:

```FSabc`def` ```

### At Sign Modifier

At sign modifiers are used to allow an alternate form of delimiting
text arguments that follow a command, by specifying that they will
instead be delimited by a pair of matching characters:

```@FS/foo/baz/```

```@^A+hello, world!+```

If both text arguments are used, then only one delimiter is required
between them.

Also, when an at sign modifier is used, whitespace is allowed between
the command and the first delimiter:

```@S /abc/ ```

Delimiters used with an at sign modifier may be any graphic ASCII
character (ASCII 33 through 126, inclusive). Control characters and
spaces are not allowed as delimiters. This is a change from earlier
TECOs that would allow any character as a delimiter.

If the E1&4 flag bit is set, then paired braces may additionally be
used to delimit text arguments. This also has the advantage of allowing
whitespace, not only before the first delimiter, but between the
text arguments.

```@S{foo}```

```@FN{foo}{baz}```

```@FS {foo} {baz}```

If text arguments are delimited using this form, then commands such
as the following are no longer valid:

```@^A{hello{```

```@^A}goodbye}```

### Numeric Values and Flags

TECO has many special numeric values and flags which are accessible through
TECO commands. Some of these values, such as the text buffer pointer, reflect
the state of TECO. Others control TECO’s behavior in various ways.
