### J - Jump to Position

nJ
- Moves the pointer to a position immediately following the nth
character in the buffer.

J
- Moves the pointer to a position immediately preceding the first
character in the buffer. Equivalent to 0J.

ZJ
- Moves the pointer to a position immediately following the last
character in the buffer.

n:J
- Same as the nJ command except that if pointer position n is
outside of the buffer, the pointer does not move and a value
of 0 is returned. If the command succeeded, a value of -1 is
returned.

:J
- Equivalent to 0:J.

