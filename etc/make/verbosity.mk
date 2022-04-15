#
#  Set= verbosity level.
#

ifdef   verbose

AT    =
NULL  = >/dev/null 2>&1
NULL2 = 

else

AT    = @
NULL  =
NULL2 = >/dev/null 2>&1

endif
