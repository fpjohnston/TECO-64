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
| \<BS\> | Causes TECO to execute a "-LT" command. This lets you walk backwards through a file when display mode is disabled. If you are already positioned at the start of the edit buffer, this command does nothing. <br><br>If the EV flag is non-zero, then the T portion of this command is redundant and is not executed.<br/><br/>If scrolling is enabled in display mode, changes are shown immediately in the edit region, so the T portion of this command is not executed. |
| \<LF\> | Causes TECO to execute an "LT" command. This lets you walk forwards through a file when display mode is disabled. If you are already positioned at the end of the edit buffer, this command does nothing. <br><br>If the EV flag is non-zero, then the T portion of this command is redundant and is not executed. <br/><br/>If scrolling is enabled in display mode, changes are shown immediately in the edit region, so the T portion of the command is not executed. |
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

If TECO is in display mode, and scrolling is enabled, then the following
commands may be used. These functions may be overridden with the FM or
FQ commands, described below.

| Command | Function |
| ------- | -------- |
| \<CTRL/W\> | Re-paints the display. |
| \<CTRL/K\> | Changes the edit and command region to a black foreground on a white background, and the status line (if any) to a white foreground on a black background. This command is intended to remedy the situation where a user inadvertently sets the same foreground and background colors for a region, thus rendering it unreadable. |
| \<End\> | Go to end of line. |
| \<End\>\<End\> | Go to end of window. |
| <nobr>\<End\>\<End\>\<End\></nobr> | Go to end of file. |
| \<Home\> | Go to start of line. |
| \<Home\>\<Home\> | Go to start of window. |
| <nobr>\<Home\>\<Home\>\<Home\></nobr> | Go to start of file. |
| \<PgDn\> | Go to next window. |
| \<PgUp\> | Go to previous window. |

### User-Definable Keys

Most terminals have function, cursor, keypad, or other special keys. TECO provides
a facility by which you can cause the pressing of one of these keys at
the prompting asterisk to be interpreted as an immediate command. When this
facility is enabled, a key such as one of the cursor control keys can cause TECO
to execute a command string or Q-register macro.

In order for this to happen, the following are required:

- Display mode support must be enabled.
- The ED&32 flag bit must be set, to enable ESCape sequences and other
special characters.
- A function, cursor, or other special key must have been mapped to a
command string with the FM command, or a Q-register with the FQ command.
- The special key must be entered directly after TECO's command prompt.

If all of these conditions have been met, then typing the key causes the
the associated command string or Q-register macro to be immediately
executed.
