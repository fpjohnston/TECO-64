! TECO-64 test script: Open indirect command file -- :EI !
! Expect: FAIL !
! Options: -B baz.com !

HXA HK

:@EI/^EQA/
"S
    :@^A/PASS/
|
    :@^A/FAIL/
'

HK
^[^[
