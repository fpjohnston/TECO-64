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
certain commands such as -P or -N that need to page backwards through a file.

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
Also, TECO allows for what are called primary and secondary input and output
streams, and the ability to switch between them.

The following outlines some simple ways to start an editing session.

teco
 - The effect of typing *teco* without a file name will depend on whether the
TECO_MEMORY environment variable (described below) is defined.
 - If TECO_MEMORY is not defined, then start TECO without opening any files.
 - If TECO_MEMORY is defined, then start TECO after re-opening the file last
edited.

teco *foo*
 - Open *foo* for editing (using the EB command) and read in the first page.
 - If the TECO_MEMORY environment variable is defined, then save the file
name *foo* on exit.

teco *foo* *baz*
 - Open *foo* for input (using the ER command) and read in the first page,
and open *baz* for output (using the EW command), overwriting any existing file.
The EK command may be used to undo this and recover the original file.
 - If the TECO_MEMORY environment variable is defined, then save the file
name *baz* on exit.

### Command-Line Options

In addition to the name of a file to edit, there are a number of command-line
options that may be specified when starting TECO.
These are described below.

Note that some options (-E, -I, and -V) allow quoted strings to be used
as arguments.
Depending on the shell or command interpreter, some characters, such as
single or double quotes, may need to be escaped so that they will be
properly passed to TECO.

-A*nn*, --arguments=*nn*
 - Pass numeric argument *nn* to an indirect command file
specified with a subsequent --execute option.

-A*mm*,*nn*, --arguments=*mm*,*nn*
 - Pass *mm* and *nn* numeric arguments to an indirect command file
specified with a subsequent --execute option.

-C, --create (default)
 - If the specified file does not exist, then create it (using the EW command).
This is effectively a *make* command in other versions of TECO.

-c, --nocreate
 - If the specified file does not exist, then issue an error and exit.

-D, --display
 - Start TECO in display mode (using the -W command).

-E*command*, --execute=*command*
 - Open *command* as an indirect command file (using the EI command). If file does
not exist, TECO will try opening *command.tec*. This is similar to a *mung* command
in other versions of TECO.

 - If *command* is a string in single or double quotes, then TECO will execute that
as a command string instead of opening any file.

-F, --formfeed
 - Enables FF as a page delimiter.

-f, --noformfeed (default)
 - Disables FF as a page delimiter.

-H, --help
 - Print help message and exit.

-I*initfile*, --initialize=*initfile*
 - Open *initfile* as an indirect command file (using the EI command). If file does
not exist, TECO will try opening *initfile.tec*. This overrides any initialization
file specified using the TECO_INIT environment variable.

-i, --noinitialize
 - Do not use any initialization file, and do not check TECO_INIT.

-L*logfile*, --log=*logfile*
 - Open *logfile* as a log file for TECO input and/or output (the E3 flag
variable controls whether either or both are logged).

-M, --memory (default)
 - If no file is specified, then use the TECO_MEMORY environment to find the
name of a file which contains the name of the last file edited.

-m, --nomemory
 - Do not use TECO_MEMORY to find the name of the last edited file.

-n, --nodefaults
 - Disable all defaults (equivalent to -c -i -m -v).

-R, --readonly
 - Open specified for inspection (using the ER command), and read in first page.

-r, --noreadonly (default)
 - Open specified file for input and output (using the EB command).

-S*nn*, --scroll=*nn*
 - Start TECO in split-screen display mode, with *nn* lines for a command window
(a.k.a. scrolling region), using the -W and nn,7:W commands.

-T*string*, --text=*string*
 - Insert *string* into edit buffer as text before TECO starts (using the I command).
Normally used in conjunction using the –execute option.

-V --vtedit=*vtfile*
 - Open *vtfile* as an indirect command file (using the EI command). If file does
not exist, TECO will try opening *vtfile.tec*. This overrides any initialization
file specified using the TECO_VTEDIT environment variable.

-v, --novtedit
 - Ignore TECO_VTEDIT environment and don't use any indirect command file to
initialize the display.

-X, --exit
 - Used with -E to exit from TECO (using the EX command) once the indirect
command file has been processed. Because of that, this option implicitly
disables display mode (as though a --novtedit or -v option was used).

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

 - If TECO_INIT translates to a command string in single or double quotes,
that string is executed as a series of TECO commands at start-up.


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
to the library directory (so if TECO_LIBRARY translates to *lib*, and the
indirect command file is *test/foo*, then the EI command will search first
in *test/foo*, and then in *lib/test/foo*.

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
These will be executed when TECO is initialized, unless the -v or --novtedit
option is specified.

 - If there is no period ('.') in the file name, it is assumed to end in '.tec'.

 - If TECO_VTEDIT translates to a command string in single or double quotes,
that string is executed as a series of TECO commands at start-up.
