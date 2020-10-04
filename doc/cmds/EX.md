### EX - Exit TECO

EX
-  Performs the same function as the EC command, but then
exits from TECO. For safety reasons, this command is aborted
if there is text in the text buffer but no output file is open.
To exit TECO after inspecting a file, use the command string *HK EX*.
To exit TECO without making any changes if an output file is open,
use the command string *EK HK EX*.