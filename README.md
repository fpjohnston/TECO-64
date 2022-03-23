## TECO-64 - README

### Description

TECO-64 is:
- A character-oriented editor, primarily used for manipulating text files.
- A pipeline editor that can manipulate files of arbitrary size.
- Capable of editing binary files.
- Portable, because it is not bound to a system-specific GUI or other
system-specific features.
- Very small compared with other editors.
- Easy to use for simple editing tasks.
- A programming language that is powerful enough for complex tasks.

### History

TECO (Text Editor and Corrector) was originally developed on the PDP-1 in
1962 by Dan Murphy, and later ported to many other operating systems,
especially those using DEC (Digital Equipment Corporation) hardware,
notably VMS, RSTS/E, TOPS, RSX-11, and RT-11. All of these implementations
were written in hardware-specific assembler language.

TECO C was a version created by Pete Siemsen in 1983, and subsequently developed
by him and many others, most recently by Tom Almy and Blake McBride. This
implementation made it possible for TECO to be ported to non-DEC operating
systems such as Linux and Windows.

And although six decades old, it remains in use today.

### Implementation

TECO-64 is written in C, for maximum portability, and was inspired by
Pete Siemsen's TECO C. Its command set is backwards-compatible with TECO C,
as well as other TECOs. TECO-64 was written from scratch to take full
advantage of current features of the C language and run-time library.

### Building TECO

To compile and link TECO, type

    make

or

    make teco

An executable image will be created in the *bin/* directory, which can then be used
to print a list of command-line options by typing

    bin/teco --help    

To build TECO with display mode support, type

    make display=1

TECO normally includes virtual memory paging, which is required for commands
such as -P and -N. To disable this feature and use TECO as a simple pipeline
editor, type

    make paging=std

To print a list of all targets and options, type

    make help

### System Requirements

Version 200 of TECO-64 was primarily developed on Linux, using the GNU
toolchain. It has been also compiled and run on Windows and MacOS.

TECO can be used in a command-line mode, as well as a display mode
using *ncurses*.

The commands that implement backwards paging and searching require virtual
memory support, but TECO can be used as a simple pipeline editor on systems
without virtual memory.

Doxygen must be installed in order to use the *doc* target.

PC-lint must be installed in order to use the *lint* and *lobs* targets.

### Version Numbers

The highest version of any DEC TECO (TECO-11 and TECO-32) was 40. To avoid
any conflict with those TECOs, since TECO macros sometimes obtain the version
number (using the EO command) in order to determine what features are
available, the initial version of TECO C was set to 100, with subsequent
versions eventually reaching 147.

For similar reasons, a baseline version number of 200 was chosen for
TECO-64. This is the major version. A minor version and patch version
also exist, per *github*'s guidelines. Values for all three can be
accessed via the EO command, although only the major version is likely
to be significant for TECO macros.

### Additional Documentation

[TECO-64 Reference Manual](doc/TECO.md) - this is a work in progress
that has been adapted, corrected, and updated from a user's guide and
language reference manual titled *Standard TECO Text Editor and
Corrector for the VAX, PDP-11, PDP-10, and PDP-8*, published by
Digital Equipment Corporation in May 1990.

### Future Directions

The following may be implemented in future versions of TECO.

- Support for compilers other than *gcc*.
- Support for other operating systems, especially OpenVMS.
- An alternative buffer handling module (e.g., a rope buffer).
- An alternative paging module (to allow backward paging when no virtual memory
is available).

### Contact Information

Any questions or comments, including bug reports and feature requests, may be
sent to NowwithTreble@gmail.com.
