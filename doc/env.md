### TECO-64 - Environment Commands

#### Alternate Command Forms

The :EG and ::EG commands can be modified with at-signs, as shown in the
table below, using :EG as an example.

| Command | Description |
| ------- | -------- |
| :EG/text/ | Classic form of command, using accent grave (or possibly ESCape) as a delimiter for the file name. |
| :@EG/*text*/ | Equivalent to :EG/*text*/, except that a pair of slashes are used to delimit the file name. Other characters may be used instead of slashes, as long as they match. If the E1&4 flag bit is set, then the text string may be enclosed in paired braces (e.g., :@EG\{*text*\}). |

In the descriptions below, the at-sign form of :EG and ::EGcommands will be used
for clarity.

#### :EG - Read environment variables

This command reads an environment variable and returns a value as follows:

| Value | Meaning |
| ----- | ------- |
| -1    | The environment variable is valid and has a value. |
|  0    | The environment variable is invalid (it is not one of the four listed in the table below). |
|  1    | The environment variable is valid, but has no value. |

There are four environment variables that can be read using the :EG command:

| Command | Function |
| ------- | -------- |
| :@EG/INI/ | Loads Q-register \* with the value of the TECO_INIT environment variable. If the environment variable is not defined, or has no value, then Q-register \* is set to the null string. |
| :@EG/LIB/ | Loads Q-register \* with the value of the TECO_LIBRARY environment variable. If the environment variable is not defined, or has no value, then Q-register \* is set to the null string. |
| :@EG/MEM/ | Loads Q-register \* with the value of the TECO_MEMORY environment variable. If the environment variable is not defined, or has no value, then Q-register \* is set to the null string. |
| :@EG/VTE/ | Loads Q-register \* with the value of the TECO_VTEDIT environment variable. If the environment variable is not defined, or has no value, then Q-register \* is set to the null string. |

Other TECOs have sometimes provided the ability to set or clear environment variables or equivalent information. This is not generally possible on operating systems such as Linux, Windows, and MacOS, so TECO-64 currently only supports reading such variables.

#### ::EG - Execute system command

| Command | Function |
| ------- | -------- |
| ::@EG/*cmd*/ | Executes *cmd* and loads Q-register + with the output of the system command, which may then be accessed with the G+ and :G+ commands. ::EG performs the same function as EG, but does not exit TECO. |

#### EJ - Get environment information

| Command | Function |
| ------- | -------- |
| EJ | On Linux, returns the ID of the current process. May be different for other operating systems. |
| 0:EJ | On Linux, returns the ID of the parent process. May be different for other operating systems. |
| -1EJ | Return a number representing the processor and operating system upon which TECO is running. Equivalent to (-3EJ * 256) + -2EJ. |
| -2EJ | Return a number representing the operating system upon which TECO is running. On Linux, this value is 1. |
| -3EJ | Return a number representing the processor upon which TECO is running. On x86 processors, this value is 10. |
| -4EJ | Return a number representing the number of bits in the word size on the processor upon which TECO is currently running. |
