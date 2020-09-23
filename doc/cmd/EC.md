### EC - Close File

EC
- Moves the contents of the text buffer, plus the remainder of
the current input file on the currently selected input stream, to
the current output file on the currently selected output stream;
then closes those input and output files. Control remains in
TECO. EC leaves the text buffer empty.

### nEC - Set Memory Size

*n*EC
- *n*EC tells TECO to expand or contract until it uses *n*K
words of memory for its text buffer. If this is not possible,
then TECO’s memory usage does not change. The 0EC command
tells TECO to use the least amount of memory possible.
