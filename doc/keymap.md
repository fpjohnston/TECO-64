## TECO-64 - Key Mapping Commands

### Display Mode Mapping Commands

If display support has been enabled, and the ED&32 flag bit is set, then
it is possible to map function, cursor, and other special keycodes to command
strings or Q-register macros, such that when those keys are subsequently
input as immediate action commands, the associated command strings or
macros are executed. It is not necessary that the macros or command strings
be terminated with an ESCape.

| Command | Function |
| ------- | -------- |
| FM*key*\`*cmds*\` | Map *key* (e.g., F1, Home) to the command string *cmds*. |
| :FM*key*\`*cmds*\` | Execute FM command, but return -1 if command succeeded and 0 if command failed (usually because of an invalid key). |
| @FM/*key*// | Unmap *key*. Used to undo previous FM commands as well as previous FQ commands. The at sign form of this command is recommended, since the second text argument is omitted. |
| @FM///      | Unmap all keys. The at sign form of this command is recommended, since both text arguments are omitted. |
| FQ*q**key*\` | Map *key* to Q-register *q*. More than one key may be mapped to the same Q-register. |
| :FM*key*\`*cmds*\` | Execute FM command, but return -1 if command succeeded and 0 if command failed (usually because of an invalid key). |
| @FQ*q*// | Ignored. Use the FM command to unmap a key from a Q-register. |

Take care when mapping a key to a local Q-register, since each macro level
normally gets an entirely new set of local Q-registers when it is invoked.

#### User-Definable Keycodes

The following are the keycodes that can be mapped to macros or Q-registers,
using FM or FQ commands, respectively. The text arguments used for those
commands are not case-sensitive.

Some keycodes were omitted from this list if it was not possible to determine
their value using the *ncurses* library. Also, the availability of some of
the keycodes may depend on settings in the operating environment.

The arrow keys (up, down, right, left) have predefined actions, but
can be remapped if desired. If they are subsequently unmapped, they revert
to their original behavior. To completely disable the arrow keys, they
should be defined with a non-null command string that does nothing
(for example, @FM/up/ /).

| Key    | Shift + Key | Ctrl + Key | Alt + Key | Description |
| ------ | ----------- | ---------- | --------- | ----------- |
| F1     | S_F1        | C_F1       |           | F1 function key |
| F2     | S_F2        | C_F2       |           | F2 function key |
| F3     | S_F3        | C_F3       |           | F3 function key |
| F4     | S_F4        | C_F4       |           | F4 function key |
| F5     | S_F5        | C_F5       |           | F5 function key |
| F6     | S_F6        | C_F6       |           | F6 function key |
| F7     | S_F7        | C_F7       |           | F7 function key |
| F8     | S_F8        | C_F8       |           | F8 function key |
| F9     | S_F9        | C_F9       |           | F9 function key |
| F10    | S_F10       | C_F10      |           | F10 function key |
| F11    | S_F11       | C_F11      |           | F11 function key |
| F12    | S_F12       | C_F12      |           | F12 function key |
| left   | S_left      | C_left     | A_left    | Left arrow key |
| right  | S_right     | C_right    | A_right   | Right arrow key |
| up     | S_up        | C_up       | A_up      | Up arrow key |
| down   | S_down      | C_down     | A_down    | Down arrow key |
| home   | S_home      | C_home     | A_home    | Home key |
| end    | S_end       | C_end      | A_end     | End key |
| pgup   | S_pgup      | C_pgup     | A_pgup    | Page up key |
| pgdn   | S_pgdn      | C_pgdn     | A_pgdn    | Page down key |
| delete | S_delete    | C_delete   | A_delete  | Delete key |
| insert |             |            | A_insert  | Insert key |

