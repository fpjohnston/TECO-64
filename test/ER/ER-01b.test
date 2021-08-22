! TECO-64 test script: Open input file -- ER !
! Expect: FAIL !
! Options: -B baz !

HXA HK          ! Copy file name to Q-register A, clear buffer !

ER^EQA        ! Open file for read, using name in Q-register A !

:@^A/PASS/

HK
^[^[