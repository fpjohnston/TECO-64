### E% - Write File From Q-Register

Write the contents of a Q-register directly to a specified file.

E%*qfilespec*$
- Write the contents of Q-register *q* to *filespec*.

@E%*q*/*filespec*/
- Equivalent to E%*qfilespec*$.

:E%*qfilespec*$
- Equivalent to E%*qfilespec*$, but return -1 if the file open
succeeded, and 0 if the file could not be opened.

:@E%*q*/*filespec*/
- Equivalent to :E%*qfilespec*$.

