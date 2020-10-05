## TECO-64 – Unimplemented Features

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

^T - Read ASCII character
- :^T - Reads and decodes the next keystroke typed at the terminal.
Note that only the colon-modified ^T command is unimplemented.
The colon-modified command is used to detect ESCape sequences.

ED - Edit Level Flag
- ED&4 - Expand memory as much as possible.
- ED&32 - Enable immediate-mode ESCape-sequence commands.
- ED&128 - Automatic refresh inhibit.

EG - Exit and Go Command
- :EGINI$ - Can be used to read the TECO_INIT environment variable,
but not to set or clear it.
- :EGLIB$ - Can be used to read the TECO_LIBRARY environment variable,
but not to set or clear it.
- :EGMEM$ - Can be used to read the TECO_MEMORY environment variable,
but not to set or clear it.
- :EGVTE$ - Can be used to read the TECO_VTEDIT environment variable,
but not to set or clear it.

EJ - Get Environment Information
- 1EJ - Always returns 0.
- 2EJ - Always returns 0.
- m,1EJ - Causes NYI error.
- m,2EJ - Causes NYI error.

ET - Terminal Mode Flag
- ET&16 - Cancel CTRL/O on type out.
- ET&64 - Detach flag.
- ET&256 - Truncate output lines to terminal width.
- ET&512 - The scope “WATCH” feature of TECO is present (always set).
- ET&1024 - The refresh scope “WATCH” feature of TECO is present
(always clear).
- ET&4096 - Terminal is capable of handling eight-bit character codes.
Can be set or reset by user, and is used by TECO, but operating system
is not informed.
- ET&16384 - Special VT220 mode.

W - Video Terminal Watch Commands

These commands facilitated screen updates that are now handled by the
*ncurses* library. Other than -1W and W, for enabling and disabling display
mode, respectively, and 7:W, for setting the size of a scrolling region,
they appear to be largely unnecessary. 1:W and 2:W may be used to determine
the terminal width and height, respectively, but should not be necessary
for setting either since TECO determines the terminal size automatically
at start-up and whenever the display is resized.

- -1W - Refresh the terminal’s screen to show the contents of the text buffer.
- -nW - Ignored.
- nW - Ignored.
- 0W - Ignored.
- W - Forget screen image and special scope modes.
- -1000W - Ignored.
- 0:W - Can be read or set, but is otherwise ignored.
- :W - Equivalent to 0:W.
- 1:W - Read or set the horizontal size of the user’s editing display.
- 2:W - Read or set the vertical size of the user’s editing display.
- 3:W - Read or set SEEALL mode. This is currently not used, but may be
added in a future release.
- 4:W - Can be read or set, but is otherwise ignored.
- 5:W - Can be read or set, but is otherwise ignored.
- 6:W - Can be read or set, but is otherwise ignored.
- 7:W - Read or set the number of lines in the scrolling (command dialogue)
portion of the screen.
- 8:W - Can be read or set, but is otherwise ignored.
- 9:W - Can be read or set, but is otherwise ignored.
