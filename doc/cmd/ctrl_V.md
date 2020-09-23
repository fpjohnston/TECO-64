### CTRL/V - Lower Case Text

^V
- Puts TECO into lower case conversion mode. In this mode, all
alphabetic characters in string arguments are automatically changed
to lower case. This mode can be overridden by explicit case control
within the search string. This command makes all strings behave as
if they began with a ^V^V.

- Two successive ^V characters in a string argument indicate to TECO
that all following alphabetic characters in this string are to be
converted to lower case unless an explicit ^W is given to override
this state. This state continues until the end of the string or until
a ^W^W construct is encountered.

0^V
- Returns TECO to its original mode. No special case conversion occurs
within strings except those case conversions that are explicitly specified
by ^V and ^W string build constructs located within the string.

