## TECO-64 - Starting TECO

### Overview

TECO can generally edit only one file at a time, although it is possible
to read one file and write another. Also, it is possible to switch between
primary and secondary input and output streams.

As TECO processes the user’s options, it builds a command string that is
processed when initialization has completed. However, this command string is
not created until all options have been processed, and it therefore does not
matter in what order they occur. In addition to the options described below,
there are also several optional environment variables that may be used to
affect processing of these options, or which affect TECO’s behavior after
initialization.

### Environment Variables

All of the following environment variables are optional; none of them need to
be defined prior to invoking TECO.

TECO_INIT
 - This specifies the path of a file containing TECO commands
to be executed at start-up. It is often used to configure editing features.

 - If TECO_INIT translates to a string in single or double quotes, that string
is executed as a series of TECO commands at start-up. Note that the quotes,
and possibly also other characters, may need to be escaped so that the shell
or command interpreter properly passes them to TECO.

TECO_LIBRARY
 - This specifies the path of a library of TECO macros (i.e., *.tec* files)
that is searched by the EI command if the specified indirect file
is not found in the local directory.

 - If the file specification includes a relative path, that will be appended
to the library directory (so if TECO_LIBRARY translates to *lib*, and the
indirect command file is *test/foo*, then the EI command will search first
in *test/foo*, and then in *lib/test/foo*.

TECO_MEMORY
 - This specifies is used to determine the name of the last file
edited. With some operating systems, this environment variable will contain that
name directly; with others (such as Linux, Windows, and MacOS), it contains the
name of a file that in turn specifies the last file edited.

TECO_PROMPT
 - The default TECO prompt is just a single asterisk (\*), but this
environment variable can be used to define an alternate prompt.

TECO_VTEDIT
 - This specifies the path for a file containing commands to be
used for editing in display mode.

### Starting TECO

To start TECO, type *teco* (possibly also specifying its location),
optionally followed by a file name and any desired command-line
options. Assuming no errors occur, TECO will process those options
and open any specified file name before printing its prompt.

teco
 - Start TECO without opening any files. If the TECO_MEMORY environment
variable is defined, and if a file was previously edited, typing this
command will cause TECO to edit that file as though it had been
explicitly specified on the command line.

teco *foo*
 - Open *foo* for input and output (using the EB command), then read in first page.

teco
 - Open last file edited, using the information in the TECO_MEMORY environment
variable. If TECO_MEMORY is not defined, TECO will start without opening a file.

### Options

Some options allow quoted strings to be used as arguments.
Depending on the shell or command interpreter, quotes, and possibly also
other characters, may need to be escaped so that they will be properly
passed to TECO.

-A*nn*, --argument=*nn*
 - Pass *nn* argument to the indirect command file specified with the --execute
 option.

-A*mm*,*nn*, --argument=*mm*,*nn*
 - Pass *mm* and *nn* arguments to the indirect command file specified with
the --execute option.

-C, --create (default)
 - If the specified file does not exist, then create it (using the EW command).
This is effectively a *make* command in other versions of TECO.

-c, --nocreate
 - If the specified file does not exist, then issue an error and exit.

-D, --display
 - Start TECO in display mode (using the 1W command).

-E*command*, --execute=*command*
 - Execute *command* as an indirect command file (using the EI command) which is
executed at start-up. This is effectively a *mung* command with other versions of
TECO. If file does not exist, TECO will try opening *mungfile.tec*.

 - If *command* is a string in single or double quotes, then that is treated as
a command string instead of a file name.

-F, --formfeed
 - Enables FF as a page delimiter.

-f, --noformfeed
 - Disables FF as a page delimiter.

-H, --help
 - Print help message.

-I*initfile*, --initialize=*initfile*
 - Open *initfile* as an indirect command file (using the EI command). If file does
not exist, TECO will try opening *initfile.tec*.

 - If *initfile* is a string in single or double quotes, then that is treated as a
command string instead of a file name.

-I, --initialize (default)
 - Use the TECO_INIT environment variable to find the name of the initialization
file. If TECO_INIT is not defined, then no initialization occurs before TECO
starts.

 - If the value of TECO_INIT is in single or double quotes, then that is treated
as a command string instead of a file name.

-i, --noinitialize
 - Do not use any initialization file, and do not check TECO_INIT.

-L*logfile*, --log=*logfile*
 - Open *logfile* as a log file for TECO input and/or output (a flag variable
controls whether either or both are logged).

-M, --memory (default)
 - If no file is specified, then use the TECO_MEMORY environment to find the
name of a file which contains the name of the last file edited.

-m, --nomemory
 - Do not use TECO_MEMORY to find the name of the last edited file.

-n, --nodefaults
 - Disable all defaults (equivalent to -c -i -m -v).

-O*baz*, --output=*baz*
 - Open specified file for input (using the ER command), and open *baz* for
output (using EW command), then read in first page.

-o, --nooutput (default)
 - Use the same name for the output file as the input file (unless –readonly is specified).

-R, --readonly
 - Open specified for inspection (using the ER command), and read in first page.

-r, --noreadonly (default)
 - Open specified file for input and output (using the EB command).

-S*nn*, --scroll=*nn*
 - Start TECO in split-screen display mode, with *nn* lines for a scrolling
region (using the 1W and nn,7:W commands).

-T*string*, --text=*string*
 - Insert *string* into edit buffer as text before TECO starts (using the I command).
Normally used in conjunction with the –execute option.

-V --vtedit=*vtfile*
 - Use macro in the file *vtfile* to initialize the display (overrides any file
specified with the TECO_VTEDIT environment variable).

-v, --novtedit
 - Ignore TECO_VTEDIT environment and don't use any indirect command file to
initialize the display.

-X, --exit
 - Used with -E to exit from TECO (using the EX command) once the indirect
command file has been processed. Because of that, this option implicitly
disables display mode (as though a --novtedit or -v option was used).

-Z, --zero
 - Start TECO with strict enforcement of command syntax (using the -1E2 command).
