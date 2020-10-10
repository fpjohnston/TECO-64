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

TECO C was created by Pete Siemsen in 1983, and subsequently developed by many
others, most recently by Tom Almy and Blake McBride. This implementation made
it possible for TECO to be ported to non-DEC operating systems such as Linux
and Windows.

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

    make DISPLAY=1

To build TECO with virtual memory paging, type

    make PAGING=vm

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

### Notes on Versioning

At the time TECO C was created, the highest version of any DEC TECO was
about 36. In order to avoid any conflict with future versions (since some
TECO macros use obtain the version number with the EO command to determine
what features are available), the initial version of TECO C was set to 100,
with subsequent versions eventually reaching 147.

For similar reasons, a baseline version number of 200 was chosen for
TECO-64. This is the major version. For the benefit of *github*, a minor
version and patch version also exist, and will be modified per their
guidelines. All three versions can be accessed via the EO command,
although only the major version is likely to be significant for TECO
macros.

### Additional Documentation

[DEC Standard TECO](doc/DEC_Standard_TECO.pdf) - this document is
30 years old and is in many ways archaic, incomplete, and
erroneous, but it also provides information about TECO not
available elsewhere.

[TECO-64 Manual](doc/manual.md) - this is adapted from the previous
document, and is a work in progress that will eventually replace it.

[Running TECO](doc/running.md) - describes how to start TECO-64 from
the command line.

[New Features](doc/new.md) - describes what features
TECO-64 has compared with other TECOs.

[Missing Features](doc/missing.md) - describes what features
TECO-64 is missing compared with other TECOs.

[TECO C Changes](doc/changes.md) - describes how TECO-64
differs from TECO C.

[Internals Notes](doc/internals.md) - provides information
about the development and internal structure of TECO-64.
