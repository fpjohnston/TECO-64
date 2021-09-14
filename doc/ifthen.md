## TECO-64 - If/Then/Else Commands

All conditional execution commands are of the form:

*n*"X command-string ’

or

*n*"X then-command-string | else-command-string ’

In the first form of the command, *n* is a numeric argument on which the
decision is based, "X" is any of the conditional execution commands listed
in table below, and "command string" is the command string which will be
executed if the condition is satisfied. The numeric argument is separated
from the conditional execution command by a double quote (") and the
command string is terminated with an apostrophe (’). If the condition
is not satisfied, the command string will not be executed; execution
will continue with the first command after the apostrophe.

In the second form of the command, two command strings are specified. The
first one is executed if the condition is satisfied and the second is
executed if the condition is not satisfied. Only one of the command
strings will be executed. After execution of the appropriate command
string, control will continue with the first command after the apostrophe
(unless the command string caused a branch out of the conditional to occur),
since execution of the vertical bar command ( | ) causes TECO to scan to
the next matching apostrophe.

Conditional commands are similar to the *if-then-else* constructs found
in other structured programming languages, although none can match
the brevity and elegance of TECO’s implementation. Nonetheless, you must
use these facilities wisely. Good programming practice dictates that a
branch into the range of a conditional (from outside that range) should
not occur.

Conditional execution commands may be nested in the same manner as
iteration commands. That is, the command string which is to be executed
if the condition on *n* is met may contain conditional execution commands,
which may, in turn, contain further conditional execution commands.

| Command | Function |
| ------- | -------- |
| *n*"A   | Execute the following command string if *n* equals the ASCII code for an alphabetic character (upper or lower case A to Z). |
| *n*"C   | Execute the following command string if *n* is the ASCII code of any character that is a symbol constituent. This includes the upper and lower case letters A to Z, the digits 0 to 9, and the symbols period (.), dollar sign ($), and underscore (_). |
| *n*"D   | Execute the following command string if *n* equals the ASCII code for a digit (0 to 9). |
| *n*"E   | Execute the following command string if *n* is equal to zero. |
| *n*"F   | Execute the following command string if *n* is FALSE. Equivalent to *n*"E. |
| *n*"G   | Execute the following command string if *n* is greater than zero.
| *n*"L   | Execute the following command string if *n* is less than zero.
| *n*"N   | Execute the following command string if *n* is not equal to zero.
| *n*"R   | Execute the following command string if *n* equals the ASCII code for an alphanumeric (upper or lower case A to Z or 0 to 9). |
| *n*"S   | Execute the following command string if *n* is successful. Equivalent to *n*"L. |
| *n*"T   | Execute the following command string if *n* is TRUE. Equivalent to *n*"L. |
| *n*"U   | Execute the following command string if *n* is unsuccessful. Equivalent to *n*"E. |
| *n*"V   | Execute the following command string if *n* equals the ASCII code for a lower case alphabetic character (lower case A to Z). |
| *n*"W   | Execute the following command string if *n* equals the ASCII code for an upper case alphabetic character (upper case A to Z). |
| *n*"<   | Identical to *n*"L. |
| *n*">   | Identical to *n*"G. |
| *n*"=   | Identical to *n*"E. |
| ’ | This "command" is actually part of the syntax of TECO conditionals. It has no effect if "executed" other than to signify termination of the current conditional level. |
| \| | Causes control to branch to the end of the conditional. Command execution resumes with the character following the ’ that ends the current conditional with the ELSE clause being skipped. |
| F’ | Causes TECO to branch (flow) to the end of the current conditional. TECO effectively resumes execution at the first command following the ’ at the end of the current conditional. Numeric arguments are eaten up by this command. If this command is issued while not in a conditional, the ?MAP error (or its equivalent) is issued. |
| F\| | Causes TECO to branch (flow) to the else clause of the current conditional. TECO effectively resumes execution at the first command following the |\ at the end of the current THEN clause. If the current conditional has no ELSE clause, or if an unmatched ’ is encountered before an unmatched \|,  then control resumes at the command following the ’ . Numeric arguments are eaten up by this command. If this command is issued while not in a conditional, the ?MAP error is issued. Well-structured programs should not need to use this command. |

