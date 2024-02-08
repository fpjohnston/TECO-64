﻿## TECO-64 – Unimplemented Features

### Overview

Some features of other TECOs have not been included in this
version of TECO-64 for one or more of the following reasons:

- Implementation has been deferred to a future release.
- Implementation requires definition of how features should work in the supported operating environments.
- Implementation makes no sense for obsolete or irrelevant features.

### File Specification Switches

General command-line options for TECO are described in other
documentation, but no options for special file modes or file
formats have been implemented to specify such things as output
file cluster sizes, non-standard open modes, BASIC-PLUS-2 line
continuation, or FORTRAN carriage control.

### Unimplemented Commands

\^T - Read ASCII character
- :\^T - Reads and decodes the next keystroke typed at the terminal.
Note that only the colon-modified \^T command is unimplemented.
The colon-modified command is used to detect ESCape sequences.

ED - Edit Level Flag
- ED&4 - Expand memory as much as possible.
- ED&128 - Automatic refresh inhibit.

EG - Exit and Go Command
- :@EG/INI/ - Can be used to read the TECO_INIT environment variable,
but not to set or clear it.
- :@EG/LIB/ - Can be used to read the TECO_LIBRARY environment variable,
but not to set or clear it.
- :@EG/MEM/ - Can be used to read the TECO_MEMORY environment variable,
but not to set or clear it.
- :@EG/VTE/ - Can be used to read the TECO_VTEDIT environment variable,
but not to set or clear it.

EJ - Get Environment Information
- 1EJ - Always returns 0.
- 2EJ - Always returns 0.
- m,1EJ - Causes NYI error.
- m,2EJ - Causes NYI error.

ET - Terminal Mode Flag
- ET&16 - Cancel CTRL/O on type out.
- ET&512 - The scope “WATCH” feature of TECO is present (always set).
- ET&1024 - The refresh scope “WATCH” feature of TECO is present
(always clear).
- ET&4096 - Terminal is capable of handling eight-bit character codes.
Can be set or reset by user, and is used by TECO, but operating system
is not informed.
- ET&16384 - Special VT220 mode.

W - Video Terminal Watch Commands

These commands facilitated screen updates that are now handled by the
*ncurses* library, as a result of which, many of these commands are now
obsolete.

- -1W - Enable disable mode.
- -W - Equivalent to -1W.
- -nW - Not implemented.
- nW - Not implemented.
- W - Disable display mode.
- 0W - Implemented as equivalent to W.
- -1000W - Not implemented.
- 0:W - Read the terminal type. Cannot be set.
- :W - Equivalent to 0:W.
- 1:W - Read or set the horizontal size of the user’s editing display.
- 2:W - Read or set the vertical size of the user’s editing display.
- 3:W - Read or set SEEALL mode.
- 4:W - Read or set "mark" status.
- 5:W - Always returns 0. Cannot be set.
- 6:W - Read the buffer position of the character in the top left corner of the screen. Cannot be set.
- 7:W - Read or set the number of lines in the scrolling region (command window) of the screen.
- 8:W - Read or set scrolling region status. If set to a non-zero value, the scrolling region is disabled.
- 9:W - Read the terminal characteristics flag. Cannot be set.
- 10:W - Read or set the tab size.
- 11:W - Read or set the maximum line length.
- 256:W - Not implemented.
- -256+n:W - Not implemented.
