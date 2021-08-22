! TECO-64 test script: Open output file -- :EW !
! Expect: PASS !
! Options: -B foo !

HXA HK          ! Copy file name to Q-register A, clear buffer !

:@EW/^EQA/      ! Open file for write, using name in Q-register A !

"S
    :@^A/PASS/
|
    :@^A/FAIL/
'

EK HK
^[^[
