## Immediate Action Commands

### Editing Commands

These commands may be used while entering a command string, in order
to edit the string, to print some or all of the string, to start command
execution, or abort input and return to the command prompt.

| Command | Function |
| ------- | -------- |
| \<CTRL/C\> | Deletes the command string being input and returns to TECO's prompt. |
| <nobr>\<CTRL/C\>\<CTRL/C\></nobr> | Deletes the command string being input, kills the editing session, deletes the edit buffer, and exits TECO. Effectively executes "\<CTRL/G\>\<CTRL\G\> EK HK EX". |
| <nobr>\<CTRL/G\>\<CTRL/G\></nobr> | Causes all commands which have been entered but not executed to be erased. Used to erase an entire command string. |
| <nobr>\<CTRL/G\>\<SPACE\></nobr> | Re-types the current input line. |
| \<CTRL/G\>\* | Re-types the entire command string. Equivalent to \<CTRL/G\>\<SPACE\> if the command string is only one line. |
| \<CTRL/U\> | Deletes the current input line. |
| \<DEL\> | Deletes the last character typed. |
| <nobr>\<*delim*\>\<*delim*\></nobr> | Causes TECO to begin execution of the command string. The two \<*delim*\>s must be typed successively without any intervening characters. |

### Type-Out Commands

These commands only work as described if they are entered immediately
after TECO's prompt, and perform different functions if they are not.

| Command | Function |
| ------- | -------- |
| \<BS\> | Causes TECO to execute a "-LT" command. This lets you walk backwards through a file when display mode is disabled. If you are already positioned at the start of the edit buffer, this command does nothing. <br><br>If the EV flag is non-zero, then the T portion of this command is redundant and is not performed. |
| \<LF\> | Causes TECO to execute an "LT" command. This lets you walk forwards through a file when display mode is disabled. If you are already positioned at the end of the edit buffer, this command does nothing. <br><br>If the EV flag is non-zero, then the T portion of this command is redundant and is not performed. |
| \<DEL\> | Equivalent to the \<BS\> command. |
| \<*delim*\> | Equivalent to the \<LF\> command. |

### Error Commands

These commands only work as described if they are entered immediately
after TECO's prompt, and perform different functions if they are not.

| Command | Function |
| ------- | -------- |
| ? | Causes TECO to print the erroneous command string from the beginning of the current macro level up to and including the character that caused the error. Used immediately after an error occurs. |
| / | Causes TECO to print a more detailed explanation of the error just printed. |
| \**q* | Causes TECO to copy the last command string into Q-register *q*. Often useful after an error, to allow a command string to be fixed. | 

### Display Commands

These commands only work as described if they are entered immediately
after TECO's prompt, and perform different functions if they are not.

| Command | Function |
| ------- | -------- |
| \<CTRL/W\> | If display mode is active, causes TECO to re-paint the display. |
| \<CTRL/K\> | If display mode is active, changes the edit and command region to a black foreground on a white background, and the status line (if any) to a white foreground on a black background. This command is intended to remedy the situation where a user inadvertently sets the same foreground and background colors for a region, thus rendering it unreadable. |

### User-Defined Immediate Commands

Most terminals have function, cursor, or other special keys. TECO provides
a facility by which you can cause the pressing of one of these keys at
the prompting asterisk to be interpreted as an immediate command. When this
facility is enabled, a key such as one of the cursor control keys can cause TECO
to execute a macro in a Q-register.

In order for this to happen, the following are required:

- Display mode support must be enabled.
- The ED&32 flag bit must be set, to enable ESCape sequences and other
special characters.
- A function, cursor, or other special key must have been mapped to a
Q-register with the FM command.
- The special key must be entered directly after TECO's command prompt.

If all of these conditions have been met, then when such a key
has been input, the associated Q-register is immediately executed as
though an M*q*\<*delim*\>\<*delim*\> command had been entered.
