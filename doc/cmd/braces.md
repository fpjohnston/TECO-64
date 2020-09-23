### {} - Braced Text Arguments

If a command allows an at-sign modifier for text arguments, and if the
E1&4 is set, then braces may be used to delimit the text arguments.
Note that a consequence of doing this is that whitespace before a left
brace or after a right brace is ignored.

Examples:

@S{foo}
- Equivalent to @S/foo/.

@FN{foo}{baz}
- Equivalent to @FN/foo/baz/.

@FS {foo} {baz}
- Equivalent to @FN/foo/baz/.

