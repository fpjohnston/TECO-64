### D - Delete Characters

D
- Delete the first character following the current position of the
buffer pointer.

nD
- Execute the D command n times. If n is positive, the n
characters following the current pointer position are deleted.
If n is negative, the n characters preceding the current pointer
position are deleted. If n is zero, the command is ignored.

-D
- Equivalent to -1D.

m,nD
- Equivalent to m,nK.

n:D
- Same as nD but returns a value (-1 if command succeeds, 0 if
command failed because the range of characters to be deleted
fell outside the text buffer).
