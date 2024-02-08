## TECO-64 - File Commands

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
noted, all of these commands leave the edit buffer unchanged.

### Alternate Command Forms

All of the commands that open files, and use text arguments to specify
a file name, can be modified with both at signs and colons, as shown in the
table below, using ER as an illustration.

| Command | Description |
| ------- | -------- |
| ER*foo*\` | Classic form of command, using accent grave (or possibly ESCape) as a delimiter for the file name. |
| @ER/*foo*/ | Equivalent to ER*foo*\`, except that a pair of slashes are used to delimit the file name. Other characters may be used instead of slashes, as long as they match. If the E1&4 bit is set, then the filename may be enclosed in paired braces (e.g., @ER\{*foo*\}). |
| :ER*foo*\` | Equivalent to ER*foo*\`, except that -1 is returned if the file was successfully opened, and 0 if it could not be opened. Reasons for failure include the file not being found, not being a device, directory, pipe, or anything other than a regular file, or lacking the required read or write permissions. |
| :@ER/foo/ | Equivalent to :ER*foo*\`. |

In the descriptions below, the at sign form of each command will be used
for clarity.

Note that although the EG command may also use an at sign to delimit its
text argument, the :EG command is a completely different command and is
documented elsewhere.

### File Open Commands

The following commands are used to open files for input and output:

| Command | Function |
| ------- | -------- |
| @EB/*foo*/ | This command is recommended for most editing jobs. It opens the specified file (*foo* in the example here) for input on the currently selected input stream and for output on the currently selected output stream. The EB command also keeps the unmodified file (the latest copy of the input file) available to the user; details of this process are system dependent. <br><br>On Linux, the name of the backup file is formed by appending a tilde (\~) to the original file name. So, using the example command above, the backup file would be *foo*\~. |
| @ER/*foo*/ | Opens the specified file (*foo* in the example here) for input on the currently selected input stream. |
| @EW/*foo*/ | Opens the specified file (*foo* in the example here) for output on the currently selected output stream. Any existing file is overwritten (this can be undone with an EK command before the file is closed). |

### File Close and Exit Commands

The following commands are used to close files and exit from TECO:

| Command | Function |
| ------- | -------- |
| EC | Moves the contents of the edit buffer, plus the remainder of the current input file on the currently selected input stream, to the current output file on the currently selected output stream; then closes those input and output files. Control remains in TECO. EC leaves the edit buffer empty. |
| EF | Closes the current output file on the currently selected output stream. The EF command does not write the current contents of the edit buffer to the file before closing it, nor does it clear or change the edit buffer. |
| @EG/*cmd*/ | Performs the same function as the EX command, but then exits from TECO and passes *cmd* to the operating system as a command string to be executed. |
| EK | Kill the current output file on the currently selected output stream. This command, which purges the output file without closing it, is useful to abort an undesired edit. Executing the EK command after an EW which is superseding an existing file leaves the old file intact. The EK command also "undoes" an EB command. |
| EX |  Performs the same function as the EC command, but then exits from TECO. For safety reasons, this command is aborted if there is text in the edit buffer but no output file is open. To exit TECO after inspecting a file, use the command string HK EX. To exit TECO without making any changes if an output file is open, use the command string EK HK EX. 

### Secondary Stream Commands

TECO provides secondary input and output streams. These permit the user
to have two input and two output files open at the same time, and to switch
processing back and forth between them. Each stream maintains its file position
independently, so that one can read from one stream (for example), switch to the
other, and then switch back to the first and resume from where one left off. In
addition, a separate command stream allows one to execute TECO commands
from a file without disturbing either input stream.

The following commands manipulate the secondary input and output streams:

| Command | Function |
| ------- | -------- |
| EP | Switches the input to the secondary input stream. Use the ER\' command to switch back to the primary input stream. |
| @ER// | Switch to primary input stream. This is only needed after an EP command has been executed to switch to the secondary input stream. |
| EA | Switches the output to the secondary output stream. Use the EW\' command to switch back to the primary output stream. |
| @EW// | Switch to primary output stream. This is only needed after an EA command has been executed to switch to the secondary output stream.  |

### Indirect File Commands

| Command | Function |
| ------- | -------- |
| @EI/*mung*/ | Opens *mung* as an indirect command file, so that any further TECO requests for terminal input will come from this file. At end-of-file, or upon TECO’s receipt of any error message, the indirect command file will be closed and terminal input will be switched back to the terminal. Note that this command only presets where input will come from; it does not "splice" the file’s data into the current command string (unless the E1&16 flag bit is set). <br><br>The contents of an indirect command file are executed as though they were a Q-register macro. Which is, it is not necessary that the command string be terminated with a double &lt;ESC>. This is a difference between TECO-64 and other TECOs. <br><br>All commands encountered in the indirect command file will have their normal TECO meaning (as opposed to any immediate action meaning). For example, a &lt;CTRL/U> encountered in an indirect command file will not erase the command line in which it occurs. Instead, it will be treated as the TECO \^U*qtext*\` command. The only exception to this rule is the &lt;ESC>&lt;ESC> command, which directs TECO to execute the preceding command string and then return to the indirect command file at the point following the &lt;ESC>&lt;ESC>. (We can say &lt;ESC> explicitly here, because &lt;*delim*> can be other than ESCape only in commands typed at the terminal.) |
| *n*@EI/*mung*/ | Opens *mung* as an indirect command file, as above, and passes it the numeric argument *n*, which may be read by including a U command such as UA as the first command in the file. |
| *m*,*n*@EI/*mung*/ | Opens *mung* as an indirect command file, as above, and passes it the numeric arguments *m* and *n*, which may be read by including U commands such as UA and UB as the first commands in the file. |
| @EI// | If an indirect command file is active, this command will close it and resume terminal input from the terminal. Any portion of the file after a double &lt;*delim*> which has not yet been read is discarded. This command has no effect if no indirect command file is already open. |

### Wildcard Commands

TECO supports wild card file processing with a set of special commands, to
allow operation on a set of files.

| Command | Function |
| ------- | -------- |
| @EN/*foo*/ | This command presets the "wildcard" lookup file specification (shown in the example here as *foo*). It is only a preset; it does not open, close, or try to find any file. The "wildcard" lookup is the only file specification command that  can contain any wildcard notations. |
| @EN// | Once the wild card lookup file specification has been preset, executing this command will find the next file that matches the preset wild card lookup filespec and will load the filespec buffer with that file’s name. The G\* command can be used to retrieve the fully expanded file specification. When no more occurrences of the wildcard filespec exist, the ?FNF error is returned. |

### Direct I/O to Q-Registers

TECO provides commands to do I/O directly to and from the Q-registers,
allowing I/O to bypass the edit buffer.

| Command | Function |
| ------- | -------- |
| @EQ*q*/*filespec*/ | Read specified file from *filespec* into Q-register *q*. |
| @E%*q*/*filespec*/ | Write the contents of Q-register *q* to *filespec*. |

### Log TECO Commands to File

| Command | Function |
| ------- | -------- |
| @EL/*filespec*/ | Open *filespec* for output as a log file. Any currently open log file will be closed. All TECO output, as well as echoed input, will be written to the file (E3 flag bits may be used to disable logging of either input or output). |
| @EL// | Close the log file. No error is returned if no log file is currently open. |
