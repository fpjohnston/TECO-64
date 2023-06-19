## TECO-64 - Linux Setup

### Overview

TECO has been used on multiple Linux distros, notably Ubuntu and Fedora,
but it is intended to work on any Linux system with a standard C run-time
library.
It is built with the GNU *make* utility, as well as the *gcc* compiler and
linker, and it can additionally be debugged with *gdb* or profiled with
*gprof*.
There are no plans currently to add support for other toolchains, but this
may be done on an as-needed basis.

### Building TECO

Once you have downloaded the source code for TECO, use *cd* or *chdir* to change
to the base of the directory tree for that code.
To then compile and link the code, type:

    make teco

or, more simply:

    make

An executable image named *teco* will be created in the *bin/* subdirectory.
You should then be able to confirm that you have a usable executable image
by typing:

    bin/teco --help    

This will print a list of command-line options and then exit.

#### Display Mode

TECO is normally built using the *ncurses* library, in order to provide support
for display mode.
If *ncurses* is not available, or if display mode is not required, TECO may be
built by typing:

    make display=off

#### Virtual Memory Paging

TECO normally expects to be able to use virtual memory, which is required for
certain commands such as -P or -N that page backwards through a file.

If virtual memory is unavailable, or backwards paging is not needed, or it is
necessary or desirable to keep memory use to a minimum, it is possible to build
TECO as a simple pipeline editor by typing:

    make paging=std

#### Other Options

The *Makefile* included with TECO includes many other options and targets.
To print a complete list, type

    make help

### Starting TECO

On Linux, TECO is normally started from a command-line prompt within a shell,
such as *bash*.
Once an executable image has been built, and assuming it is in one of the
directories in your PATH environment variable, you can start the editor by
typing:
    
teco

If this does not work, you may need to also include the path where TECO can
be found.

Once TECO starts, it will prompt a prompt, signaling that you may start
typing commands. By default, this prompt is simply a single asterisk:

teco
<br>
\*

TECO can generally edit only one file at a time, although it is possible
to read one file and write another.
Also, TECO allows for primary and secondary streams for both input and output,
and the ability to switch between them.

The following outlines some simple ways to start an editing session.

teco
 - The effect of typing *teco* without a file name will depend on whether the
TECO_MEMORY environment variable (described below) is defined.
 - If TECO_MEMORY is not defined, then start TECO without opening any files.
 - If TECO_MEMORY is defined, then start TECO after re-opening the file last
edited.

teco *foo*
 - Open *foo* for editing and read in the first page.
 - If the TECO_MEMORY environment variable is defined, then save the file
name *foo* on exit.

teco *foo* *baz*
 - Open *foo* for input and read in the first page, and open *baz* for output,
overwriting any existing file.
The EK command may be used to undo this and recover the original file.
 - If the TECO_MEMORY environment variable is defined, then save the file
name *baz* on exit.

### Environment Variables

All of the following environment variables are optional; none of them need to
be defined prior to invoking TECO.

TECO_INIT
 - This specifies a relative or absolute path of a file containing TECO commands
to be executed at start-up.
These commands are then executed when TECO is initialized.
It can be used configure editing features for the local environment, such as
settings for flag variables.

 - If there is no period ('.') in the file name, it is assumed to end in '.tec'.

| Example | Description |
| ------- | ----------- |
| TECO_INIT=init.tec | Use *init.tec* in current directory. |
| TECO_INIT=init | Equivalent to *init.tec*. |
| TECO_INIT=/path/init.tec | Use *init.tec* in absolute path */path/*. |
| TECO_INIT=path/init.tec | Use *init.tec* in relative path *path/*. |

TECO_LIBRARY
 - This specifies a relative or absolute path of a library of TECO macros
(i.e., *.tec* files) that is searched by the EI command if the specified
file is not found in the local directory.

 - If the file specification includes a relative path, that will be appended
to the library directory if necessary (for example, if TECO_LIBRARY translates to
*lib*, and the indirect command file is *foo*, then the EI command will search first
in the current directory, and then in *lib/foo*).

 - This environment variable is also used with any initialization file specified
with the TECO_INIT or TECO_VTEDIT environment variables.

TECO_MEMORY
 - This specifies is used to determine the name of the last file
edited. With some operating systems, this environment variable will contain that
name directly; with others (such as Linux, Windows, and MacOS), it contains the
name of a file that in turn specifies the last file edited.

TECO_PROMPT
 - The default TECO prompt is just a single asterisk (\*), but this
environment variable can be used to define an alternate prompt.

TECO_VTEDIT
 - This specifies a relative or absolute path for a file containing commands to be
used for editing in display mode.
This file will be executed when TECO is initialized, unless the -d or --nodisplay
option is specified by the user.

 - If there is no period ('.') in the file name, it is assumed to end in '.tec'.

### Command-Line Options

In addition to the name of a file to edit, there are a number of command-line
options that may be specified when starting TECO.
These are described below.

-A*nn*, --arguments=*nn*
 - Pass numeric argument *nn* to the next indirect command file specified with
a -E or --execute option.

-A*mm*,*nn*, --arguments=*mm*,*nn*
 - Pass *mm* and *nn* numeric arguments to the next indirect command file
specified with a -E or --execute option.

-C, --create (default)
 - If the specified file does not exist, then create it.

-c, --nocreate
 - If the specified file does not exist, then issue an error and exit.

-D*file*, --display=*file*
 - Execute *file* as an initialization file for display mode.
This takes precedence over a file specified by the TECO_VTEDIT environment
variable.

-D, --display
- If an initialization file was specified by the TECO_VTEDIT environment
variable or a --display=*file* command, then this option is a no-op.
Otherwise, it executes a -1W command to start display mode.

-d, --nodisplay
 - Ignore any initialization file for display mode specified by the
TECO_VTEDIT environment variable or a previous -D or --display option.

-E*file*, --execute=*file*
 - Open *file* as an indirect command file, and execute any TECO commands
within it.

 - If *command* is a string in single or double quotes, then TECO will directly
execute that string as a sequence of TECO commands (note that depending
on the shell being used, some characters may need to be escaped in order
to be properly passed to TECO).

- The --execute option may be specified multiple times, each optionally
preceded by --arguments or --text options.

-F, --formfeed
 - Specifies that any form feed characters in edited files are page delimiters.

-f, --noformfeed (default)
 - Specifies that form feed characters in edited files are not treated specially.

-H, --help
 - Print help message and exit.

-I*file*, --initialize=*file*
 - Open *file* as an indirect command file.
This takes precedence over a file specified by the TECO_INIT environment
variable.

-i, --noinitialize
 - Ignore any initialization file specified by the TECO_INIT environment
variable or any previous -I or --initialize option.

-L*file*, --log=*file*
 - Open *file* as a log file for TECO input and/or output.

-m, --nomemory
 - Ignore the TECO_MEMORY environment variable.

--make *file*
 - Create the specified file, even if it already exists.
This is similar to the TECO MAKE command.

--mung *file*
 - Execute the specified file as an indirect command file.
This is similar to the TECO MUNG command.

-n, --nodefaults
 - Equivalent to --noinitialize --nodisplay --nomemory.

-R, --readonly
 - Open any specified file for input.

-r, --noreadonly (default)
 - Open any specified file for input and output.

-S*nn*, --scroll=*nn*
 - Start TECO in split-screen display mode, with *nn* lines for a command window
(a.k.a. scrolling region).
 - This option implies the --display option.

-T*string*, --text=*string*
 - Insert *string* into edit buffer as text before TECO starts.
Normally used in conjunction with the –execute option.

--version
 - Print version and copyright information.

-X, --exit
 - Exits from TECO after all other options have been processed.
It is normally used in conjunction with the --execute option.
 - This option implies the --nodisplay option.


