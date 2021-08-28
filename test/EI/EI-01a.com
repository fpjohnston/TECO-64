! TECO-64 test script: Open indirect command file -- EI !
! Expect: PASS !
! Options: -B foo.com !

HXA HK

EI^EQA

:@^A/PASS/

HK
^[^[
