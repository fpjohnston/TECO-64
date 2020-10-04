### File Specification Commands

You must specify an input file whenever you want TECO to accept text from any
source other than the terminal. You must specify an output file whenever you
want to make a permanent change to the input file. Input and output files are
selected by means of commands which specify the name of the files a user wants
to open, as well as the device, directory, path, or other information that
specifies where the files are to be found.

The format of file specifications are operating system dependent, and are not
described here. Almost every editing job begins with at least one file
specification command. Additional file specification commands may be executed
during an editing job whenever required; however, TECO will keep only one input
file and one output file selected at a time.

TECO recognizes two input and two output "streams" called the primary and
secondary streams. The primary input and output streams are initially selected
when TECO is invoked. Most file selection commands, and all of the other TECO
commands (page manipulation, etc.), operate on the currently selected input
and/or output stream.

The following sections list all of the file specification commands. Unless otherwise
noted, all of these commands leave the text buffer unchanged.

#### File Open Commands

The following commands are used to open files for input and output:

[EB - Edit Backup](cmds/EB.md)

[ER - Edit Read](cmds/ER.md)

[EW - Edit Write](cmds/EW.md)

#### File Close and Exit Commands

The following commands are used to close files and exit from TECO:

[EC - Edit Close](cmds/EC.md)

[EF - Close Output File](cmds/EF.md)

[EG - Exit and Go](cmds/EG.md)

[EK - Edit Kill](cmds/EK.md)

[EX - Exit TECO](cmds/EX.md)

#### Secondary Stream Commands

TECO provides secondary input and output streams. These permit the user
to have two input and two output files open at the same time, and to switch
processing back and forth between them. Each stream maintains its file position
independently, so that one can read from one stream (for example), switch to the
other, and then switch back to the first and resume from where one left off. In
addition, a separate command stream allows one to execute TECO commands
from a file without disturbing either input stream.

The following commands manipulate the secondary input and output streams:

[EP - Switch to Secondary Input Stream](cmds/EP.md)

[ER - Switch to Primary Input Stream](cmds/ER.md)

[EA - Switch to Secondary Output Stream](cmds/EA.md)

[EW - Switch to Primary Output Stream](cmds/EW.md)

#### Indirect File Commands

[EI - Open Indirect Command File](cmds/EI.md)

#### Wildcard Commands

TECO supports wild card file processing with a set of special commands, to
allow TECO programs to operate on a set of files.

[EN - Open Wildcard File Specification](cmds/EN.md)

#### Direct I/O to Q-Registers

TECO provides commands to do I/O directly to and from the Q-registers,
allowing I/O to bypass the text buffer.

[EQ - Read File to Q-Register](cmds/EQ.md)

[E% - Write Q-Register to File](cmds/E_pct.md)

#### Log TECO Commands to File

[EL - Open/Close Log File](cmds/EL.md)

