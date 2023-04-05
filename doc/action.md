## TECO-64 - Immediate Action Commands

<style>
    span {
        white-space: nowrap;
    }
</style>

### Editing Commands

These commands may be used while entering a command string, in order
to edit the string, to print some or all of the string, to start command
execution, or abort input and return to the command prompt.

| Command | Function |
| ------- | -------- |
| \<CTRL/C\> | Deletes the command string being input and returns to TECO's prompt. |
| <span>\<CTRL/C\>\<CTRL/C\></span> | Deletes the command string being input, kills the editing session, deletes the edit buffer, and exits TECO. Effectively executes "\<CTRL/G\>\<CTRL\G\> EK HK EX". |
| <span>\<CTRL/G\>\<CTRL/G\></span> | Causes all commands which have been entered but not executed to be erased. Used to erase an entire command string. |
| <span>\<CTRL/G\>\<SPACE\></span> | Re-types the current input line. |
| \<CTRL/G\>\* | Re-types the entire command string. Equivalent to \<CTRL/G\>\<SPACE\> if the command string is only one line. |
| \<CTRL/U\> | Deletes the current input line. |
| \<DEL\> | Deletes the last character typed. |
| <span>\<*delim*\>\<*delim*\></span> | Causes TECO to begin execution of the command string. The two \<*delim*\>s must be typed successively without any intervening characters. |

### Type-Out Commands

These commands only work as described if they are entered immediately
after TECO's prompt, and perform different functions if they are not.

| Command | Function |
| ------- | -------- |
| \<BS\> | Causes TECO to execute a "-LT" command. This lets you walk backwards through a file when display mode is disabled. If you are already positioned at the start of the edit buffer, this command does nothing. <br><br>If the EV flag is non-zero, then the T portion of this command is redundant and is not executed.<br/><br/>If scrolling is enabled in display mode, changes are shown immediately in the edit window, so the T portion of this command is not executed. |
| \<LF\> | Causes TECO to execute an "LT" command. This lets you walk forwards through a file when display mode is disabled. If you are already positioned at the end of the edit buffer, this command does nothing. <br><br>If the EV flag is non-zero, then the T portion of this command is redundant and is not executed. <br/><br/>If scrolling is enabled in display mode, changes are shown immediately in the edit window, so the T portion of the command is not executed. |
| \<CTRL/]\>\<CTRL/]\> | If the ET&16384 flag bit is set, causes last command to be executed again. |
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

If TECO is in display mode, and scrolling has been enabled with the 7:W command,
then the following commands may be used. These functions may be overridden with
the FM or FQ commands, as described [here](keymap.md).

| Command | Function |
| ------- | -------- |
| \<CTRL/W\> | Re-paints the display. |
| \<CTRL/K\> | Toggles colors off or on. This command is intended for a the situation where a user inadvertently sets a window to use the same foreground and background colors, thus rendering it unreadable. |
| \<End\> | Go to end of line, then to end of window, then to end of buffer. |
| \<Ctrl/End\> | Like \<End\>, but involves more steps before reaching the end of buffer. |
| \<Home\> | Go to start of line, then to start of window, then to start of buffer. |
| \<Ctrl/Home\> | Like \<Home\>, but involves more steps before reaching the start of buffer. |
| \<PgUp\> | Move up n lines, where n is the number of rows visible in the window. |
| \<Ctrl/PgUp\> | Move up n/2 lines, where n is the number of rows visible in the window. |
| \<PgDn\> | Move down n lines, where n is the number of rows visible in the window. |
| \<Ctrl/PgDn\> | Move down n/2 lines, where n is the number of rows visible in the window. |
| \<Up\> | Move the cursor up one row. |
| \<Ctrl/Up\> | Scroll the window down one row. |
| \<Down\> | Move the cursor down one row. |
| \<Ctrl/Down\> | Scroll the window up one row. |
| \<Left\> | Move the cursor left one column. |
| \<Ctrl/Left\> | Scroll the window right one column. |
| \<Right\> | Move the cursor right one column. |
| \<Ctrl/Right\> | Scroll the window left one column. |

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
command string with the FM command, or a Q-register with the FQ command,
as described [here](keymap.md).
- The special key must be entered directly after TECO's command prompt.

If all of these conditions have been met, then typing the key causes the
the associated command string or Q-register macro to be immediately
executed.
