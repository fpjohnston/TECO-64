### EJ - Get environment information

0EJ
- For Linux, returns the ID of the current process. May be different for
other operating systems.

0:EJ
- For Linux, returns the ID of the parent process. May be different for
other operating systems.

-1EJ
- Return a number representing the processor and operating system upon
which TECO is running. Equivalent to (-3EJ * 256) + -2EJ.

-2EJ
- Return a number representing the operating system upon which TECO
is running. Currently, the only value returned is 1 for Linux.

-3EJ
- Return a number representing the processor upon which TECO is
running. Currently, the only value returned is 10 for x86.

-4EJ
- Return a number representing the number of bits in the word size on
the processor upon which TECO is currently running.
