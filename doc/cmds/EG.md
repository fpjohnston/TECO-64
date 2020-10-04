### EG - Exit and Go

The EG command performs several functions.
The first, when used without a colon, causes TECO to execute an EX command,
and then pass a text string to the operating system for execution.
The second, when used with a colon, allows the user to read or modify
certain environment variables.

EG*text*$
- Performs the same function as the EX command, but then
exits from TECO and passes *text* to the operating system as
a command string to be executed.

@EG/*text*/
- Equivalent to EG*text*$.

:EG*cmd args*$
- Reads or sets an environment variable. This command always returns a
value as follows:

| Value | Meaning |
| ----- | ------- |
| -1    | The environment variable *cmd* is valid and has a value. |
|  0    | The environment variable *cmd* is invalid. |
|  1    | The environment variable *cmd* is valid, but has no value. |

There are four environment variables that can be accessed, as follows:

| *cmd*   | Function |
| ----- | -------- |
| INI   | Equivalent to the TECO_INIT environment variable. |
| LIB   | Equivalent to the TECO_LIBRARY environment variable.  |
| MEM   | Equivalent to the TECO_MEMORY environment variable. |
| VTE   | Equivalent to the TECO_VTEDIT environment variables. |

These four functions work as follows:

| Command | Function |
| ------- | -------- |
| :EG*cmd*$ | Loads Q-register \* with the value of the environment variable *cmd*. If the |
|         | environment variable has no value, then Q-register \* is set to the null string. |
| :EG*cmd* $ | Clears the environment variable (note space after *cmd*). Subsequent |
|         | :EG*cmd*$ commands will set Q-register \* to the null string. Note that |
|         | some operating systems to not allow TECO clear environment variables |
|         | in its shell or parent process. |
| :EG*cmd text*$ | Sets the environment variable to *text*. Subsequent :EG*cmd*$ commands |
|         | will set Q-register \* to *text*. Note that some operating systems do not allow |
|         | TECO to set environment variables in its shell or parent process. |
