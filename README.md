## TECO-64 - Introduction
    
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

To build TECO with windows support, type

    make WINDOWS=1

To build TECO with virtual memory paging, type

    make PAGING=vm

To print a list of all targets and options, type

    make help

### System Requirements

Version 200 of TECO-64 was primarily developed on Linux, using the GNU
toolchain. It has been also successfully compiled and run on Windows and
MacOS, although these have not yet been extensively tested, nor have any
compilers other than *gcc* yet been tested.

*ncurses* is required in order to use TECO in window mode, although it
can be built without *ncurses*.

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

The following documents describe features of TECO-64, as well as the
differences between it and other TECOs.

[Running TECO](doc/running.md)

[New Features](doc/new.md)

[Missing Features](doc/missing.md)

[TECO C Changes](doc/changes.md)

[Internals Notes](doc/internals.md)

The following document provides the best introduction to TECO, even
though it is 30 years old and would certainly benefit from being
updated and corrected. It is hoped that this may happen with a future
release of TECO-64.

[DEC Standard TECO](doc/DEC_Standard_TECO.pdf)
