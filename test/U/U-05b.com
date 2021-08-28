! TECO-64 test script: Load Q-registers w/ defaults using nM -- :U !
! Expect: PASS !
! Options: -A 123 !

:UN
:UM
QM :@=/QM=%d: /
QN :@=/QN=%d: /

:@^A/PASS/
^[^[