### CTRL/W - Upper Case Text

^W
- Puts TECO into upper case conversion mode. In this mode, all
alphabetic characters in string arguments are automatically changed
to upper case. This mode can be overridden by explicit case control
within the search string. This command makes all strings behave as
if they began with ^W^W.

- Two successive ^W characters indicates to TECO that all following
alphabetic characters in this string are to be converted to upper case
unless an explicit ^V is encountered to override this state. This state
continues until the end of the string or until a ^V^V construct is
encountered. 

0^W
- Returns TECO to its original mode. No special case conversion occurs
within strings except those case conversions that are explicitly specified
by ^V and ^W string build constructs located within the string.

