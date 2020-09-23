### ; - Branch Out of Current Iteration

Unlike with other TECOs, and for improved readability,
a ; or :; command, as described below, now allows for
whitespace between it and the preceding command.

;
- This command causes TECO to branch out of the current
iteration, if the immediately preceding search (or search and
replace) command failed. In that case, control resumes at
the character following the matching > at the end of the
current iteration. On the other hand, if the preceding search
succeeded, command execution continues with the character
following the ;. If this command is encountered from outside of
an iteration (in the current macro level), then the ?SNI error
message is issued.

*n*;
- This command causes TECO to branch out of the current
iteration if the value of *n* is greater than or equal to 0.
In that case, command execution resumes at the character
following the matching > at the end of the current iteration.
On the other hand, if *n* is less than 0, command execution
continues with the character following the ;. If this command
is encountered from outside of an iteration (in the current
macro level), then the ?SNI error message is issued.

:;
- This command causes TECO to branch out of the current
iteration if the immediately preceding search (or search and
replace) command succeeded. In that case, control resumes
at the character following the matching > at the end of
the current iteration. On the other hand, if the preceding
search failed, command execution continues with the character
following the ;. If this command is encountered from outside of
an iteration (in the current macro level), then the ?SNI error
message is issued.

*n*:;
- This command causes TECO to branch out of the current
iteration if the value of *n* is less than 0. In that case, command
execution resumes at the character following the matching >
at the end of the current iteration. On the other hand, if *n* is
greater than or equal to 0, command execution continues with
the character following the ;. If this command is encountered
from outside of an iteration (in the current macro level), then
the ?SNI error message is issued.


