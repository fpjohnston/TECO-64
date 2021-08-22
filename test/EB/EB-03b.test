! TECO-64 test script: Open file for backup -- :EB !
! Expect: FAIL !
! Options: -B baz !

HXA HK          ! Copy file name to Q-register A, clear buffer !

:@EB/^EQA/      ! Open file for backup, using name in Q-register A !

"S
    :@^A/PASS/
|
    :@^A/FAIL/
'

EK HK
^[^[
