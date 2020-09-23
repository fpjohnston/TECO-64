### EW - Open Output File

EW*filespec*$
- Opens  a file for output on the currently selected output stream.

@EW/*filespec*/
- Equivalent to EW*filespec*$.

:EW*filespec*$
- Equivalent to EW*filespec*$, but returns -1 is returned if the file
open succeeded; 0 is returned if the file could not be opened.

:@EW/*filespec*/
- Equivalent to :EW*filespec*$.
