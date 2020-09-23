### EL - Open/Close Log File

EL*filespec*$
- Open the *filespec* for output as a log file. Any currently open
log file will be closed. All TECO output, including echoed input,
will be copied to the file (E3 flag bits may be used to disable
logging of either input or output).

@EL/*filespec*/
- Equivalent to EL*filespec*$.

:EL*filespec*$
- Equivalent to EL*filespec*$, but -1 is returned if the file open
succeeded; 0 is returned if the file could not be opened.

:@EL/*filespec*/
- Equivalent to :EL*filespec*$.

EL$
- Close any open log file.

@EL//
- Equivalent to EL$.

