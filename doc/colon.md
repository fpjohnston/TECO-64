## TECO-64 - Colon-Modified Commands

The following is a list of all the commands that allow the use of
colon or double colon modifiers. If these are used with a command
that does not use them, and the E2&8 flag bit is set, then TECO
will issue a COL error.

| Command | Description |
| ------- | ----------- |
| ^A      | Output text string |
| ^T      | Input character |
| ^U      | Copy text to Q-register |
| %       | Decrement Q-register value |
| ?       | Toggle trace |
| =       | Print numeric value |
| A       | Append line |
| C       | Move *dot* forward |
| D       | Delete character |
| E%      | Write Q-register to file |
| EB      | Edit backup |
| EG      | Exit and go |
| EI      | Execute indirect command file |
| EJ      | Get environment characteristics |
| EL      | Open or close log file |
| EN      | Search for matching files |
| EO      | Get TECO version |
| EQ      | Read file into Q-register |
| ER      | Edit read |
| EW      | Edit write |
| EY      | Yank w/o yank protection |
| E_      | Global search w/o yank protection |
| FB      | Bounded search |
| FC      | Bounded search and replace |
| FD      | Search and delete string |
| FK      | Search and delete intervening text |
| FN      | Global search and replace |
| FR      | Replace last string |
| FS      | Local search and replace |
| F_      | Destructive search and replace |
| G       | Print Q-register on terminal |
| I       | Insert text string or character |
| J       | Move *dot* to new position |
| L       | Move lines |
| M       | Execute macro |
| N       | Global search |
| P       | Write current page |
| R       | Move *dot* backward |
| S       | Local search |
| T       | Type character |
| W       | Read or set display mode parameter |
| X       | Copy to Q-register |
| Y       | Yank text into edit buffer |
| _       | Global search w/ yank protection |
| ]       | Pop Q-register |