! TECO-64 test script: Try to load Q-register using M -- U !
! Expect: FAIL !
! Options: !

UN
QN @:=/QN=%d: /

:@^A/PASS/
^[^[