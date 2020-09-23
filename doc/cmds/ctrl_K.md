### CTRL/K - Reset Window Colors

CTRL/K
- When used immediately after a TECO prompt, resets the foreground and
background colors for the command, text, and status line windows. It
provides a way to reset the screen in the event that the user
inadvertently sets the foreground and background to the same color,
thus making that region unreadable.

- The command and text windows are set to a black foreground and a white
background.

- The status line (if active) is set to a white foreground and a black
background.

- This command must be a CTRL/K, not the up-arrow equivalent.