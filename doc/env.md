### Environment Commands

#### :EG - Read environment variables

This command reads an environment variable and returns a value as follows:

| Value | Meaning |
| ----- | ------- |
| -1    | The environment variable is valid and has a value. |
|  0    | The environment variable is invalid. |
|  1    | The environment variable is valid, but has no value. |

There are four environment variables that can be read:

| Command | Function |
| ------- | -------- |
| :@EG/INI/ | Loads Q-register \* with the value of the TECO_INIT environment variable. If the environment variable is not defined, or has no value, then Q-register \* is set to the null string. |
| :@EG/LIB/ | Loads Q-register \* with the value of the TECO_LIBRARY environment variable. If the environment variable is not defined, or has no value, then Q-register \* is set to the null string. |
| :@EG/MEM/ | Loads Q-register \* with the value of the TECO_MEMORY environment variable. If the environment variable is not defined, or has no value, then Q-register \* is set to the null string. |
| :@EG/VTE/ | Loads Q-register \* with the value of the TECO_VTEDIT environment variable. If the environment variable is not defined, or has no value, then Q-register \* is set to the null string. |

#### ::EG - Execute system command

| Command | Function |
| ------- | -------- |
| ::@EG/*cmd*/ | Executes *cmd* and loads Q-register + with the output of the system command, which may then be accessed with the G+ and :G+ commands. ::EG is the same as EG, but does not exit TECO. |

#### EJ - Get environment information

| Command | Function |
| ------- | -------- |
| EJ | On Linux, returns the ID of the current process. May be different for other operating systems. |
| 0:EJ | On Linux, returns the ID of the parent process. May be different for other operating systems. |
| -1EJ | Return a number representing the processor and operating system upon which TECO is running. Equivalent to (-3EJ * 256) + -2EJ. |
| -2EJ | Return a number representing the operating system upon which TECO is running. On Linux, this value is 1. |
| -3EJ | Return a number representing the processor upon which TECO is running. On x86 processors, this value is 10. |
| -4EJ | Return a number representing the number of bits in the word size on the processor upon which TECO is currently running. |
