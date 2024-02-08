## TECO-64 - Colon-Modified Commands

Colon modifiers can be divided between those that cause commands to return
success or failure status, and those that change commands in other ways.
Note that if colon modifiers are specified for any command that does not
allow them, and the E2&8 flag bit is set, TECO will issue a COL error.

### Commands That Return Status

For many commands, the effect of using a colon modifier is to cause those
commands to return 0 if they failed, rather than aborting after issuing an
error, or -1 if they succeeded.
This allows the user to detect failure and take remedial action, rather than
having TECO issue an error and abort processing of a macro or command string.

| Command  | Description |
| -------- | ----------- |
| :A       | Append line. |
| :C       | Move *dot* forward. |
| :D       | Delete character(s). |
| :E%      | Write Q-register to file. |
| :EB      | Edit backup file. |
| :EI      | Execute indirect command file. |
| :EL      | Open log file. |
| :EN      | Search for matching files. |
| :EQ      | Read file into Q-register. |
| :ER      | Open input file. |
| :EW      | Open output file. |
| :EY      | Yank w/o yank protection. |
| :EZ      | Execute system command. |
| :E_      | Global search w/o yank protection. |
| :FB      | Bounded search. |
| :FC      | Bounded search and replace. |
| :FD      | Search and delete string. |
| :FF      | Map or unmap CTRL/F*n* to command string. |
| :FK      | Search and delete intervening text. |
| :FM      | Map or unmap key to command string. |
| :FN      | Global search and replace. |
| :FR      | Replace last string. |
| :FS      | Local search and replace. |
| :F_      | Destructive search and replace. |
| :J       | Move *dot* to new position. |
| :N       | Global search. |
| :P       | Write current page. |
| :R       | Move *dot* backward. |
| :S       | Local search. |
| :Y       | Yank text into edit buffer. |
| :_       | Global search w/ yank protection. |
| :]       | Pop Q-register. |

### Other Commands

For other commands, the effect of using a colon modifier can range from being
relatively minor, as in the case of :\^A commands, to completely changing the
command, as in the case of :EG commands.

| Command  | Description |
| -------- | ----------- |
| :\^A     | Output text string, followed by newline (LF or CR/LF). |
| :\^T     | Output character in binary mode. |
| :\^U     | Append character or text to Q-register. |
| :%       | Increment Q-register and discard returned value. |
| :=       | Print numeric value, but don't add newline (LF or CR/LF). |
| :;       | Exit iteration on success. |
| :EG      | Read environment variables. |
| :EJ      | Get alternate environment characteristics. |
| :G       | Print Q-register on terminal. |
| :L       | Count lines in text buffer. |
| :M       | Execute macro without creating new local Q-registers. |
| :Q       | Return the number of characters in the text storage area of Q-register. |
| :W       | Read or set display mode parameter. |
| :X       | Append lines to Q-register. |
