### Mode Control Flags

TECO has flags which control various aspects of its operation. You can find a
flag’s current setting by executing its command name without an argument; the
current setting of the flag is returned as a value. A flag may be set to a specific
value by executing its command name preceded by a numerical argument; the
flag is set to the value of the argument.

The following table describes the commands that set and clear flags; *\<flag\>*
represents any of the flags listed below.

| Command | Function |
| ------- | -------- |
| \<flag\>  | Return value of flag. |
| *n*\<flag\> | Set value of flag to *n*. |
| *m*,*n*\<flag\> | In the flag, turn off those bits specified by *m* and turn on those bits specified by *n*. |
| 0,*n*\<flag\> | Turn on the bits in the flag specified by *n*. |
| *m*,0\<flag\> | Turn off the bits in the flag specified by *m*. |

[ED - Edit level flag](cmds/ED.md)

[EE - Alternate command delimiter](cmds/EE.md)

[EH - Help level flag](cmds/EH.md)

[ES - Search verification flag](cmds/ES.md)

[ET - Terminal flag](cmds/ET.md)

[EU - Upper/lower case flag](cmds/EU.md)

[EV - Edit verify flag](cmds/EV.md)
