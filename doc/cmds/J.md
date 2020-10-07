### J - Jump to Position

*n*J
- Moves *dot* to a position immediately following the *n*th
character in the buffer. *n* must be non-negative.

J
- Moves *dot* to a position immediately preceding the first
character in the buffer. Equivalent to 0J.

ZJ
- Moves *dot* to a position immediately following the last
character in the buffer.

n:J
- Same as the *n*J command except that -1 is
returned if new position of *dot* is inside the buffer. If the new
position of *dot* is outside the buffer, 0 is returned and *dot*
is not changed.

:J
- Equivalent to 0:J.

