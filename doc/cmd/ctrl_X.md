### CTRL/X - Search Mode Flag

0^X
- The text argument in a search command will match text in the text
buffer independent of case in either the search argument or the text
buffer. The lower case alphabetics match the upper case alphabetics,
and "`", "{", " | ", "}", "~" match "@", "[", "\ ", "]", "^"
respectively.

-1^X
- The search will succeed only if the text argument is identical to text
in the text buffer. (This means that lower case does NOT match upper
case).

1^X
- The text argument in a search command will match text in the text
buffer independent of case in either the search argument or the text
buffer.

The initial value of the ^X flag is 1. (For control of caret or
uparrow treatment in search strings, see the ED&1 flag bit.)

