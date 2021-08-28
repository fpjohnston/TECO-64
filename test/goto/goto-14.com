! TECO-64 test script: Branch backward into a conditional -- O !
! Expect: FAIL !

@O!tag1!

0 "E

    !tag2!

    :@^A/PASS/

    ^C
'

!tag1!

@O!tag2!

:@^A/FAIL/

^C

