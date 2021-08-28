! TECO-64 test script: Load Q-register w/ default -- :U !
! Expect: PASS !
! Options: !

:UN
QN @:=/QN=%d: /

:@^A/PASS/
^[^[