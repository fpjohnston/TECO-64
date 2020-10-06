### Page Manipulation Commands

The following commands permit text to be read into the text buffer from
an input file or written from the buffer onto an output file.

All of the input commands listed in this table assume that the input file
is organized into pages small enough to fit into available memory. If any
page of the input file contains more characters than will fit into
available memory, the input command will continue reading characters into
the buffer until a line feed is encountered when the buffer is about 3/4
full. Special techniques for handling pages larger than the buffer
capacity will be presented later in this chapter.

#### Append Commands

[A - Append lines](cmds/A.md)

#### Page Commands

[P - Write page to file](cmds/P.md)

#### Yank Commands

[Y - Yank page w/ protection](cmds/Y.md)

[EY - Yank page w/o protection](cmds/EY.md)

