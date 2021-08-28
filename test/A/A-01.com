! TECO-64 test script: Read edit buffer values -- nA !
! Expect: PASS !

@I/hello, world!/

0 UA 0J
Z < QA A UB QB ^T %A >
-1^T

:@^A/PASS/

^C
