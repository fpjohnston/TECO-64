### EQ - Read File to Q-Register

EQ*qfilespec*$
- Read specified file into Q-register *q*.

- This command originated in TECO-10, but unlike that TECO, no trailing
NULs (if any) are removed from the file read in.

@EQ*q*/*filespec*/
- Equivalent to @EQ*qfilespec*$.

:EQ*qfilespec*$
- Read filespec into Q-register *q*. -1 is returned if the file open
succeeded; 0 is returned if the file could not be found or opened.

:@EQ*q*/*filespec*/
- Equivalent to :EQ*qfilespec*$.