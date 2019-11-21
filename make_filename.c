//****************************************************************************
//        BldStr()
//        This function "builds" a string.  This means converting string
//        build constructs in the input string into their intended equivalents
//        in the output string.  The string build constructs are as follows:
//        ^Q    use next character literally,  not as a string build char
//        ^R    use next character literally,  not as a string build char
//        ^V    lowercase the next character
//        ^V^V    lowercase all following characters
//        ^W    uppercase the next character
//        ^W^W    uppercase all following characters
//        ^EQq    use string in q - register q here
//        
//        ^EUq    use ASCII char for number in q - register q here
//        When this function is called,  CBfPtr points to the first character
//        of the input string.  It is assumed that the string is terminated by an ESCAPE
//        character ( or something else if the calling command was @ -modified).  If the
//        string is not properly terminated this function will die with "unterminated
//        command" when it encounters CStEnd while looking for the terminator character.
//        When this function returns,  CBfPtr points to the ESCAPE which
//        terminates the string, the built string is in the buffer pointed to by
//        XBfBeg,  and XBfPtr points to the character after the last character in
//        the built string.
//        The commands which contain a filename (EB, EI, EN, ER and
//                                               EW) use this function.
//        The EG command, which exits with an operating system
//        command line, uses this function.  The O command, which jumps to a tag, uses
//        this function.  The search commands (E_, FK, FN, FS, F_, N, S and _) use this
//        function.

static void DoCtVW(charptr EndArg, unsigned char TmpChr);

static void DoCtE(charptr EndArg, charptr XBfEnd);

static charptr BBfPtr;            // pointer into XBf
static int CaseCv;            // case conversion
static unsigned char WVFlag;        // ^W or ^V flag

static void DoCtVW(EndArg, TmpChr)    // do a control-V or control-W
charptr EndArg;                // ptr to end of string argument
unsigned char TmpChr;            // temporary character
{
    WVFlag = TmpChr;

    if (++CBfPtr == EndArg)          // move past ^W or ^V,  too far?
    {
        print_err(ERR_ISS);        // yes, illegal search string
    }

    if ((*CBfPtr == '^') && ((EdFlag & ED_CARET_OK) == 0))
    {
        if (++CBfPtr == EndArg)
        {
            print_err(ERR_ISS);
        }

        TmpChr = To_Upper(*CBfPtr);

        if ((TmpChr < '@') || (TmpChr > '_'))
        {
            printc_err(ERR_IUC, *CBfPtr);
        }

        TmpChr &= '\077';
    }
    else
    {
        TmpChr = *CBfPtr;
    }

    if (WVFlag == CTRL_V)
    {
        if (TmpChr == CTRL_V)
        {
            CaseCv = LOWER;
        }
        else
        {
            *BBfPtr++ = To_Lower(TmpChr);
        }
    }
    else
    {
        if (TmpChr == CTRL_W)
        {
            CaseCv = UPPER;
        }
        else
        {
            *BBfPtr++ = To_Upper(TmpChr);
        }
    }

    WVFlag = '\0';
}

static void DoCtE(EndArg, XBfEnd)    // do a control-E
charptr EndArg;                // ptr to end of string argument
charptr XBfEnd;                // end of build-string buffer
{
    int    Status;            // returned from FindQR
    charptr    TCStEn;            // temporary holder of CStEnd

    if (++CBfPtr == EndArg)          // move past ^E,  too far?
    {
        print_err(ERR_ICE);        // yes, illegal ^E command
    }

    if ((*CBfPtr == 'Q') || (*CBfPtr == 'q'))
    {
        if (++CBfPtr == EndArg)
        {
            print_err(ERR_ISS);
        }

// handle filespec buffer and search string buffer

        if (*CBfPtr == '*' || *CBfPtr == '_')
        {
            charptr  BufPtr, BufBeg;

            if (*CBfPtr == '*')
            {
                BufPtr = FBfPtr;
                BufBeg = FBfBeg;
            }
            else
            {
                BufPtr = SBfPtr;
                BufBeg = SBfBeg;
            }

            if ((BufPtr - BufBeg) > (XBfEnd - BBfPtr))
            {
                print_err(ERR_STL);
            }

            memmove(BBfPtr, BufBeg, (SIZE_T)(BufPtr - BufBeg));
            BBfPtr += BufPtr - BufBeg;
        }
        else
        {

// it really must be a Q reg reference after all

            TCStEn = CStEnd;            // save CStEnd
            CStEnd = EndArg;
            FindQR();
            CStEnd = TCStEn;            // restore CStEnd

            if ((QR->End_P1 - QR->Start) > (XBfEnd - BBfPtr))
            {
                print_err(ERR_STL);
            }

            memmove(BBfPtr, QR->Start, (SIZE_T)(QR->End_P1 - QR->Start));
            BBfPtr += QR->End_P1 - QR->Start;
        }
    }
    else if ((*CBfPtr == 'U') || (*CBfPtr == 'u'))
    {
        if (++CBfPtr == EndArg)
        {
            print_err(ERR_ISS);
        }

        TCStEn = CStEnd;            // save CStEnd
        CStEnd = EndArg;
        Status = FindQR();
        CStEnd = TCStEn;            // restore CStEnd

        *BBfPtr++ = (char)QR->Number;
    }
    else
    {
        *BBfPtr++ = CTRL_E;
        *BBfPtr++ = *CBfPtr;
    }
}

void BldStr(XBfBeg, XBfEnd, XBfPtr)        // build a string
charptr XBfBeg;            // beginning of build-string buffer
charptr XBfEnd;            // end of build-string buffer
charptr (*XBfPtr);        // pointer into build-string buffer
{
    charptr    EndArg;        // end of input string, plus 1
    unsigned char TmpChr;    // temporary character

    FindES(ESCAPE);         // move CBfPtr to end of argument

    WVFlag = '\0';        // initialize ^W and ^V flag
    CaseCv = IniSrM;        // initialize internal search mode
    BBfPtr = XBfBeg;        // initialize ptr into build-string buffer
    EndArg = CBfPtr;        // save pointer to end of argument
    CBfPtr = ArgPtr;        // reset to beginning of argument

    while (CBfPtr < EndArg)
    {
        if ((*CBfPtr == '^') && ((EdFlag & ED_CARET_OK) == 0))
        {
            if (++CBfPtr == EndArg)
            {
                print_err(ERR_ISS);
            }

            TmpChr = To_Upper(*CBfPtr);

            if ((TmpChr < '@') || (TmpChr > '_'))
            {
                ErrChr(ERR_IUC, *CBfPtr);
            }

            TmpChr &= '\077';
        }
        else
        {
            TmpChr = *CBfPtr;
        }

        switch (TmpChr)
        {
        case CTRL_R:
        case CTRL_Q:
            if (++CBfPtr == EndArg)
            {
                print_err(ERR_ISS);
            }

            *BBfPtr++ = *CBfPtr;
            break;

        case CTRL_V:
        case CTRL_W:
            DoCtVW(EndArg, TmpChr);

            break;

        case CTRL_E:
            DoCtE(EndArg, XBfEnd);

            break;

        default:
            if (CaseCv == LOWER)
            {
                TmpChr = To_Lower(TmpChr);
            }
            else if (CaseCv == UPPER)
            {
                TmpChr = To_Upper(TmpChr);
            }

            *BBfPtr++ = TmpChr;
        }

        if (BBfPtr > XBfEnd)
        {
            print_err(ERR_STL);    // string too long
        }

        ++CBfPtr;
    }

    *XBfPtr = BBfPtr;
}
