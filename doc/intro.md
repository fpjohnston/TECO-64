### TECO-64 - Introduction

TECO is a powerful text editing language. It may be used to edit any form
of ASCII text: program sources, command procedures, or manuscripts, for
example. TECO is a character-oriented editor, and also offers a number
of facilities for dealing with lines of text.

Because TECO is an interpreter, you can use it interactively, issuing
commands which are to be executed as soon as they are typed in.
In this mode, TECO can be used for tasks ranging from simple to complex.

You can also write sequences of commands called macros, which can be
stored internally and then invoked by short commands.
Additionally, you can create files containing command sequences to
be executed as desired.

TECO can be used as an implementation language, as it provides powerful
primitive functions for text processing, screen handling, and keyboard
management.

TECO correctly handles most sequential ASCII formats supported by the
operating systems upon which it runs. It provides flexible pattern-matching
constructs for text searching, and file wildcarding.
A split screen scrolling feature allows command dialogue to share the
screen with an automatically-updated window into the editing buffer.

TECO’s syntax is terse; commands are designed to minimize keystrokes. Also,
the versatility of TECO "in all its glory" makes it complex. However, just a
few commands suffice to get real work done, and a novice TECO user can begin
creating and editing text files after a brief instruction. More
powerful features can be learned one at a time, and at leisure.

This manual presents TECO in two stages. The first part contains basic
information and introduces that set of "just a few" commands.

Subsequent chapters describe the full TECO command set, including a review
of the those commands presented in (TBD). These chapters also introduce
the concept of TECO as a programming language and explain how basic editing
commands may be combined into programs sophisticated enough to handle the
most complicated editing tasks.

The early sections of this manual include few specific examples of commands,
since all TECO commands have a consistent, logical format which will quickly
become apparent to the beginning user. There is, however, an extensive example
at the end of Chapter 1 which employs most of the commands introduced up to
that point. Students of TECO should experiment with each command as it is
introduced, and then duplicate the examples on their computer.

(If a video terminal which supports split-screen scrolling is available,
and the TECO being used supports it, seeing both your commands and their
effects in the text buffer simultaneously can make learning easier. A
command such as "5,7:W" allows 5 lines for command dialogue, while watching
the text buffer on the remainder of the screen. The 7:W command is described
(TBD).)

This manual is intended to be used for reference, and except for the section
titled Basics of TECO, is not a tutorial. After Chapter 1, it is assumed that
the reader has a general familiarity with TECO and is referring to this manual
to find detailed information.

The table below shows the notation which is used in this manual to represent
special characters.

| Notation        | ASCII code | Name |
| --------------- | ---------- | ---- |
| \<NULL\>        | 0          | NUL |
| \<BS\>          | 8          | Backspace |
| \<TAB\>         | 9          | Horizontal tab |
| \<LF\>          | 10         | Line feed |
| \<VT\>          | 11         | Vertical tab |
| \<FF\>          | 12         | Form feed |
| \<CR\>          | 13         | Carriage return |
| \<ESCAPE\>      | 27         | ESCape |
| \<CTRL/x\>      | \-         | Control-x |
| ^x              | \-         | Either the two-character sequence ^ and x, or \<CTRL/x\> |
| \<space\>       | 20         | Space |
| \<DELETE\>      | 127        | Delete or Rubout |
| \<DELIM\> or \` | \-         | See below |

\<DELIM\> is a character used to signify the end of text strings and TECO
commands typed at the terminal. (TECO presumes that you may need to deal
with carriage return and line feed as ordinary characters in ASCII text, so
another character must be used as TECO’s text-string and command-string
delimiter.)

When a command string is being executed, TECO looks for the ESCape character
as its delimiter. As it may be desirable to use function or cursor keys
that generate ESCape sequences, TECO is capable of recognizing a surrogate
(some other, user-specified) character as signifying an ESCape when typed
at the terminal. Such a character echoes as accent grave — you see an \`
character, and TECO receives an ESCape character. (Note that TECO programs,
command files, and macros cannot use the surrogate, since it is translated
to ESCape only when you type it at the terminal.) For details on enabling
a surrogate for ESCape, see the EE flag or the 8192 ET bit.
