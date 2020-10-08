### ES - Search Verification Flag

ES
- Current value of the ES flag.

nES
- The search verification flag, which controls the text typed out
after searches.
- If *n* is equal to 0, nothing is typed out after searches.
If *n* is -1, the current line is typed out when a successful search at top
level is completed (i.e., a V command is done automatically).
- If *n* is between 1 and 31, the current line is typed out with a
line feed immediately following the position of the pointer to
identify its position.
- If *n* is between 32 and 126, the current
line is typed out with the ASCII character corresponding to the
value of n immediately following the position of the pointer to
identify its position. If you want to see more than one line of
type out, use the form *m*\*256+*n*. The *n* is the same as above.
The *m* is the number of lines of view. For example, 3\*256+^^!
would give two lines on either side of the found line, and the
found line with the character "!" at the pointer’s position.
- The ES flag does not apply to searches executed inside iterations or
macros; lines found inside iterations or macros are never typed
out.
- The initial value of ES is 0.