! TECO-64 test script: Open wildcard filespec -- @EN// !
! Expect: PASS !
! Options: -B '*.com' !

HXA HK          ! Copy file name to Q-register A, clear buffer !

@EN/^EQA/       ! Initialize for wildcard lookup !

0UA
<
    :@EN//;     ! Loop for all matching filespecs. !
    %A          ! And count how many we found !
>

QA :@=/PASS: %u file/

QA-1 "N @^A/s/ '

:@^A/ matched/

^[^[
