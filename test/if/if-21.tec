! TECO-64 test script: Test for upper case -- "W !
! Expect: PASS !

0 UC            ! Start at NUL !

^^A < %C-1 "W :@^A/FAIL/ ' >          ! NUL to upper case !

^^Z-^^A+1 < %C-1 "W | :@^A/FAIL/ ' >  ! Upper case !

255-^^Z-1 < %C-1 "W :@^A/FAIL/ ' >    ! Remaining 8-bit characters !

0 UC            ! Start at NUL !

^^A < %C-1 "w :@^A/FAIL/ ' >          ! NUL to upper case !

^^Z-^^A+1 < %C-1 "w | :@^A/FAIL/ ' >  ! Upper case !

255-^^Z-1 < %C-1 "w :@^A/FAIL/ ' >    ! Remaining 8-bit characters !

:@^A/PASS/

^C