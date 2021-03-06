///
///  @file    cmd_scan.c
///  @brief   Parse and scan command arguments.
///
///  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
///
///  Permission is hereby granted, free of charge, to any person obtaining a
///  copy of this software and associated documentation files (the "Software"),
///  to deal in the Software without restriction, including without limitation
///  the rights to use, copy, modify, merge, publish, distribute, sublicense,
///  and/or sell copies of the Software, and to permit persons to whom the
///  Software is furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIA-
///  BILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
///  THE SOFTWARE.
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "teco.h"
#include "ascii.h"
#include "errcodes.h"
#include "eflags.h"
#include "estack.h"
#include "file.h"
#include "qreg.h"
#include "term.h"

#include "cbuf.h"


///
///  @brief    Scan at sign command.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_atsign(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->atsign && f.e2.atsign)
    {
        throw(E_ATS);                   // Too many at signs
    }

    cmd->atsign = true;

    return true;
}


///
///  @brief    Scan bad command.
///
///  @returns  Nothing (we throw an exception).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_bad(struct cmd *cmd)
{
    assert(cmd != NULL);

    throw(E_ILL, cmd->c1);              // Illegal command
}


///
///  @brief    Scan colon command.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_colon(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c;

    if ((c = peek_cbuf()) == ':')       // Double colon?
    {
        if (cmd->dcolon && f.e2.colon)
        {
            throw(E_COL);               // Too many colons
        }

        next_cbuf();                    // Yes, count it
        trace_cbuf(c);

        cmd->dcolon = true;             // And flag it
    }

    cmd->colon = true;                  // Flag the first colon

    return true;
}


///
///  @brief    Parse command with format "@X/text1/".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_1(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd);
    reject_n(cmd);
    reject_colon(cmd);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Parse command with format "+m,nX".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_M(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_neg_m(cmd);
    require_n(cmd);
    reject_colon(cmd);
    reject_atsign(cmd);

    return false;
}


///
///  @brief    Parse command with format "+m,n@X/text1/".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_M1(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_neg_m(cmd);
    require_n(cmd);
    reject_colon(cmd);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Parse command with format "+m,n:X".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_Mc(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_neg_m(cmd);
    require_n(cmd);
    reject_dcolon(cmd);
    reject_atsign(cmd);

    return false;
}


///
///  @brief    Parse command with format "+m,n:@X/text1/".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_Mc1(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_neg_m(cmd);
    require_n(cmd);
    reject_dcolon(cmd);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Parse command with format "+m,n:@X/text1/text2/".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_Mc2(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_neg_m(cmd);
    require_n(cmd);
    reject_dcolon(cmd);
    scan_texts(cmd, 2, ESC);

    return false;
}


///
///  @brief    Parse command with format "+m,n:Xq".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_Mcq(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_neg_m(cmd);
    require_n(cmd);
    reject_dcolon(cmd);
    reject_atsign(cmd);
    scan_qreg(cmd);

    return false;
}


///
///  @brief    Parse command with format "+m,n:Xq//".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_Mcq1(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_neg_m(cmd);
    require_n(cmd);
    reject_dcolon(cmd);
    scan_qreg(cmd);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Parse command with format "+n@X/text1/".
////
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_N1(struct cmd *cmd)
{
    assert(cmd != NULL);
    reject_neg_n(cmd);
    require_n(cmd);
    reject_colon(cmd);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Parse command with format "X" (no arguments).
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_X(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd);
    reject_n(cmd);
    reject_colon(cmd);
    reject_atsign(cmd);

    return false;
}


///
///  @brief    Parse command with format ":X".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_c(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd);
    reject_n(cmd);
    reject_dcolon(cmd);
    reject_atsign(cmd);

    return false;
}


///
///  @brief    Parse command with format ":@X/text1/".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_c1(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd);
    reject_n(cmd);
    reject_dcolon(cmd);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Parse command with format ":@X/text1/text2/".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_c2(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd);
    reject_n(cmd);
    reject_dcolon(cmd);
    scan_texts(cmd, 2, ESC);

    return false;
}


///
///  @brief    Parse command with format ":@Xq/text1/".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_cq1(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd);
    reject_n(cmd);
    reject_dcolon(cmd);
    scan_qreg(cmd);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Parse command with format "::@X/text1/".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_d1(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd);
    reject_n(cmd);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Parse command with format "X$" that act like ESCape, in that they
///            accept m and n arguments, but just consume them without using
///            them.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_escape(struct cmd *cmd)
{
    assert(cmd != NULL);

    cmd->m_set = cmd->n_set = false;

    reject_colon(cmd);
    reject_atsign(cmd);

    return false;
}


///
///  @brief    Parse "flag" command with format "nX!".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool parse_flag1(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd);
    reject_colon(cmd);
    reject_atsign(cmd);

    if (cmd->n_set)                     // n argument?
    {
        return false;                   // Yes, not an operand
    }

    switch (cmd->c1)
    {
        case CTRL_E:
        push_x(f.ctrl_e ? SUCCESS : FAILURE, X_OPERAND);

            return true;

        case CTRL_N:
        {
            reject_n(cmd);

            struct ifile *ifile = &ifiles[istream];

            push_x(feof(ifile->fp), X_OPERAND);

            return true;
        }

        case CTRL_X:
            push_x(f.ctrl_x, X_OPERAND);

            return true;

        case 'E':
        case 'e':
            switch (cmd->c2)
            {
                case 'E':
                case 'e':
                    push_x(f.ee, X_OPERAND);

                    return true;

                case 'O':
                case 'o':
                    push_x(f.eo, X_OPERAND);

                    return true;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    throw(E_ILL, cmd->c1);              // Should never get here!
}


///
///  @brief    Parse "flag" command with format "m,nX!".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool parse_flag2(struct cmd *cmd)
{
    assert(cmd != NULL);

    require_n(cmd);
    reject_colon(cmd);
    reject_atsign(cmd);

    if (cmd->n_set)                     // n argument?
    {
        return false;                   // Yes, not an operand
    }

    assert(cmd->c1 == 'E' || cmd->c1 == 'e');

    switch (cmd->c2)
    {
        case '1':                       // E1
            push_x(f.e1.flag, X_OPERAND);

            return true;

        case '2':                       // E2
            push_x(f.e2.flag, X_OPERAND);

            return true;

        case '3':                       // E3
            push_x(f.e3.flag, X_OPERAND);

            return true;

        case '4':                       // E4
            push_x(f.e4.flag, X_OPERAND);

            return true;

        case 'D':                       // ED
        case 'd':
            push_x(f.ed.flag, X_OPERAND);

            return true;

        case 'H':                       // EH
        case 'h':
            push_x(f.eh.flag, X_OPERAND);

            return true;

        case 'S':                       // ES
        case 's':
            push_x(f.es, X_OPERAND);

            return true;

        case 'T':                       // ET
        case 't':
            push_x(f.et.flag, X_OPERAND);

            return true;

        case 'U':                       // EU
        case 'u':
            push_x(f.eu, X_OPERAND);

            return true;

        case 'V':                       // EV
        case 'v':
            push_x(f.ev, X_OPERAND);

            return true;

        default:
            break;
    }

    throw(E_ILL, cmd->c1);              // Should never get here!
}


///
///  @brief    Parse command with format "m,n@X/text1/text2/".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_m2(struct cmd *cmd)
{
    assert(cmd != NULL);

    require_n(cmd);
    reject_colon(cmd);
    scan_texts(cmd, 2, ESC);

    return false;
}


///
///  @brief    Parse command with format "m,n:X".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_mc(struct cmd *cmd)
{
    assert(cmd != NULL);

    require_n(cmd);
    reject_dcolon(cmd);
    reject_atsign(cmd);

    return false;
}


///
///  @brief    Parse command with format "m,n:@X/text1/".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_mc1(struct cmd *cmd)
{
    assert(cmd != NULL);

    require_n(cmd);
    reject_dcolon(cmd);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Parse command with format "m,n:@X/text1/text2/".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_mc2(struct cmd *cmd)
{
    assert(cmd != NULL);

    require_n(cmd);
    reject_dcolon(cmd);
    scan_texts(cmd, 2, ESC);

    return false;
}


///
///  @brief    Parse command with format "m,n:Xq".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_mcq(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_dcolon(cmd);
    reject_atsign(cmd);
    scan_qreg(cmd);

    return false;
}


///
///  @brief    Parse command with format "m,n::@X/text1/".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_md1(struct cmd *cmd)
{
    assert(cmd != NULL);

    require_n(cmd);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Parse command with format "m,n::@X/text1/text2/".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_md2(struct cmd *cmd)
{
    assert(cmd != NULL);

    require_n(cmd);
    scan_texts(cmd, 2, ESC);

    return false;
}


///
///  @brief    Parse command with format "m,nXq".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_mq(struct cmd *cmd)
{
    assert(cmd != NULL);

    require_n(cmd);
    reject_colon(cmd);
    scan_qreg(cmd);
    reject_atsign(cmd);

    return false;
}


///
///  @brief    Parse command with format "nX".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_n(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd);
    reject_colon(cmd);
    reject_atsign(cmd);

    return false;
}


///
///  @brief    Parse command with format "n@X/text1/".
////
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_n1(struct cmd *cmd)
{
    assert(cmd != NULL);
    reject_m(cmd);
    require_n(cmd);
    reject_colon(cmd);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Parse command with format "n:X".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_nc(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd);
    reject_dcolon(cmd);
    reject_atsign(cmd);

    return false;
}


///
///  @brief    Parse command with format "n:@X/text1/".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_nc1(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd);
    reject_dcolon(cmd);
    scan_texts(cmd, 1, ESC);

    return false;
}


///
///  @brief    Parse command with format "n:Xq".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_ncq(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd);
    reject_dcolon(cmd);
    reject_atsign(cmd);
    scan_qreg(cmd);

    return false;
}


///
///  @brief    Parse command with format "nXq".
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_nq(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_m(cmd);
    reject_colon(cmd);
    reject_atsign(cmd);
    scan_qreg(cmd);

    return false;
}


///
///  @brief    Parse operand or operator.
///
///  @returns  true (command is an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool parse_oper(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_colon(cmd);
    reject_atsign(cmd);

    return true;
}


///
///  @brief    Scan nothing.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_nop(struct cmd *unused)
{
    return false;
}
