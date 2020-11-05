## TECO-64 - User-Definable Keys

The following are the keys that can be mapped to macros or Q-registers,
using FM or FQ commands, respectively. The text arguments used for those
commands are not case-sensitive.

Some keys or combinations of keys were omitted from this list if it was
not possible to read them in TECO using *ncurses* (for example, the F11
and F12 keys when using Gnome on Ubuntu). Also, some of the keys in this
list may under certain conditions be executed by the operating environment
and not available to TECO.

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
|        |             | C_F10      |           | F10 function key |
|        | S_F11       | C_F11      |           | F11 function key |
| F12    | S_F12       | C_F12      |           | F12 function key |
| left   | S_left      | C_left     | A_left    | Left arrow key |
| right  | S_right     | C_right    | A_right   | Right arrow key |
| up     | S_up        | C_up       | A_up      | Up arrow key |
| down   | S_down      | C_down     | A_down    | Down arrow key |
| home   | S_home      | C_home     | A_home    | Home key |
| end    | S_end       | C_end      | A_end     | End key |
| pgup   | S_pgup      |            | A_pgup    | Page up key |
| pgdn   | S_pgdn      |            | A_pgdn    | Page down key |
| delete | S_delete    | C_delete   | A_delete  | Delete key |
| insert |             |            | A_insert  | Insert key |
