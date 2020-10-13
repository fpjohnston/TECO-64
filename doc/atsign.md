## TECO-64 - At Sign-Modified Commands

The following is a list of all the commands that allow the use of
an at sign modifier. If an at sign is used with a command other
than one of the following, and the E2&4 flag bit is set, then
TECO will issue a ATS error.

| Command | Description |
| ------- | ----------- |
| ^A      | Output text string |
| ^U      | Copy text to Q-register |
| !       | Tag or comment |
| =       | Print numeric value |
| E%      | Write Q-register to file |
| EB      | Edit backup |
| EG      | Exit and go |
| EI      | Execute indirect command file |
| EL      | Open or close log file |
| EN      | Search for matching files |
| EQ      | Read file into Q-register |
| ER      | Edit read |
| EW      | Edit write |
| E_      | Global search w/o yank protection |
| FB      | Bounded search |
| FC      | Bounded search and replace |
| FD      | Search and delete string |
| FK      | Search and delete intervening text |
| FN      | Global search and replace |
| FR      | Replace last string |
| FS      | Local search and replace |
| F_      | Destructive search and replace |
| I       | Insert text string or character |
| N       | Global search |
| O       | GOTO tag |
| S       | Local search |
| _       | Global search w/ yank protection |
