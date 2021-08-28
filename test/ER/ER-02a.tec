! TECO-64 test script: Open input file -- @ER// !
! Expect: PASS !
! Options: -B foo !

HXA HK          ! Copy file name to Q-register A, clear buffer !

@ER/^EQA/       ! Open file for read, using name in Q-register A !

:@^A/PASS/

HK
^[^[
