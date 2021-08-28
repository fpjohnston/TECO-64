! TECO-64 test script: Branch backward outside a conditional -- O !
! Expect: PASS !

@O!tag1!

!tag2!

:@^A/PASS/

^C

!tag1!

0 "E

    @O!tag2!

    :@^A/FAIL/

    ^C
'

