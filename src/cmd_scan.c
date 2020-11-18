///
///  @file    cmd_scan.c
///  @brief   Scan commands in order to complete command block.
///
///  @copyright 2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include "exec.h"
#include "qreg.h"


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

    if (peek_cbuf() == ':')             // Double colon?
    {
        (void)fetch_cbuf();             // Yes, count it

        if (cmd->dcolon && f.e2.colon)
        {
            throw(E_COL);               // Too many colons
        }

        cmd->dcolon = true;             // And flag it
    }

    cmd->colon = true;                  // Flag the first colon

    return true;
}


///
///  @brief    Scan command with format "@X/text1/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_a1(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    check_m_arg(cmd);
    check_n_arg(cmd);
    check_colon(cmd);
    scan_texts(cmd, 1);

    return false;
}


///
///  @brief    Scan command with format "@X/text1/text2/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_a2(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    check_m_arg(cmd);
    check_n_arg(cmd);
    check_colon(cmd);
    scan_texts(cmd, 2);

    return false;
}


///
///  @brief    Scan command with format "@Xq/text1/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_aq1(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    check_m_arg(cmd);
    check_n_arg(cmd);
    check_colon(cmd);
    scan_qreg(cmd);
    scan_texts(cmd, 1);

    return false;
}


///
///  @brief    Scan command with format ":X".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_c(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    check_m_arg(cmd);
    check_n_arg(cmd);
    check_atsign(cmd);

    return false;
}


///
///  @brief    Scan command with format ":@X/text1/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_ca1(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    check_m_arg(cmd);
    check_n_arg(cmd);
    check_dcolon(cmd);
    scan_texts(cmd, 1);

    return false;
}


///
///  @brief    Scan command with format ":@Xq/text1/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_caq1(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    check_m_arg(cmd);
    check_n_arg(cmd);
    check_dcolon(cmd);
    scan_qreg(cmd);
    scan_texts(cmd, 1);

    return false;
}


///
///  @brief    Scan command with format "::@X/text1/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_da1(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    check_m_arg(cmd);
    check_n_arg(cmd);
    scan_texts(cmd, 1);

    return false;
}


///
///  @brief    Scan command with format "m,nX".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_m(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->m_set)
    {
        if (cmd->m_arg < 0)
        {
            throw(E_NCA);
        }
        else if (!cmd->n_set)
        {
            throw(E_NON);
        }
    }

    check_colon(cmd);
    check_atsign(cmd);

    return false;
}


///
///  @brief    Scan command with format "m,n@X/text1/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_ma1(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->m_set)
    {
        if (cmd->m_arg < 0)
        {
            throw(E_NCA);
        }
        else if (!cmd->n_set)
        {
            throw(E_NON);
        }
    }

    check_colon(cmd);
    scan_texts(cmd, 1);

    return false;
}


///
///  @brief    Scan command with format "m,n@X/text1/text2/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_ma2(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->m_set)
    {
        if (cmd->m_arg < 0)
        {
            throw(E_NCA);
        }
        else if (!cmd->n_set)
        {
            throw(E_NON);
        }
    }

    check_m_arg(cmd);
    check_colon(cmd);
    scan_texts(cmd, 2);

    return false;
}


///
///  @brief    Scan command with format "m,n:X".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_mc(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->m_set)
    {
        if (cmd->m_arg < 0)
        {
            throw(E_NCA);
        }
        else if (!cmd->n_set)
        {
            throw(E_NON);
        }
    }

    check_atsign(cmd);

    return false;
}


///
///  @brief    Scan command with format "m,n:@X/text1/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_mca1(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->m_set)
    {
        if (cmd->m_arg < 0)
        {
            throw(E_NCA);
        }
        else if (!cmd->n_set)
        {
            throw(E_NON);
        }
    }

    check_dcolon(cmd);

    if (cmd->c1 == CTRL_A)
    {
        cmd->delim = CTRL_A;            // Default text delimiter
    }

    scan_texts(cmd, 1);

    return false;
}


///
///  @brief    Scan command with format "m,n:@X/text1/text2/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_mca2(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->m_set)
    {
        if (cmd->m_arg < 0)
        {
            throw(E_NCA);
        }
        else if (!cmd->n_set)
        {
            throw(E_NON);
        }
    }

    check_dcolon(cmd);
    scan_texts(cmd, 2);

    return false;
}


///
///  @brief    Scan command with format "m,n:@Xq/text1/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_mcaq1(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->m_set)
    {
        if (cmd->m_arg < 0)
        {
            throw(E_NCA);
        }
        else if (!cmd->n_set)
        {
            throw(E_NON);
        }
    }

    check_dcolon(cmd);
    scan_qreg(cmd);
    scan_texts(cmd, 1);

    return false;
}


///
///  @brief    Scan command with format "m,n:Xq".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_mcq(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->m_set)
    {
        if (cmd->m_arg < 0)
        {
            throw(E_NCA);
        }
        else if (!cmd->n_set)
        {
            throw(E_NON);
        }
    }

    check_dcolon(cmd);
    scan_qreg(cmd);

    return false;
}


///
///  @brief    Scan command with format "m,n::@X/text1/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_mda1(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->m_set)
    {
        if (cmd->m_arg < 0)
        {
            throw(E_NCA);
        }
        else if (!cmd->n_set)
        {
            throw(E_NON);
        }
    }

    scan_texts(cmd, 1);

    return false;
}


///
///  @brief    Scan command with format "m,n::@X/text1/text2/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_mda2(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->m_set)
    {
        if (cmd->m_arg < 0)
        {
            throw(E_NCA);
        }
        else if (!cmd->n_set)
        {
            throw(E_NON);
        }
    }

    scan_texts(cmd, 2);

    return false;
}


///
///  @brief    Scan command with format "m,nXq".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_mq(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (cmd->m_set)
    {
        if (cmd->m_arg < 0)
        {
            throw(E_NCA);
        }
        else if (!cmd->n_set)
        {
            throw(E_NON);
        }
    }

    check_colon(cmd);
    scan_qreg(cmd);

    return false;
}


///
///  @brief    Scan command with format "nX".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_n(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    check_m_arg(cmd);
    check_colon(cmd);
    check_atsign(cmd);

    return false;
}


///
///  @brief    Scan command with format "n@X/text1/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_na1(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    check_m_arg(cmd);
    check_colon(cmd);
    scan_texts(cmd, 1);

    return false;
}


///
///  @brief    Scan command with format "n:X".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_nc(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    check_m_arg(cmd);
    check_dcolon(cmd);
    check_atsign(cmd);

    return false;
}


///
///  @brief    Scan command with format "n:@X/text1/".
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_fmt_nca1(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    check_m_arg(cmd);
    check_dcolon(cmd);
    scan_texts(cmd, 1);

    return false;
}


///
///  @brief    Scan nothing.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_nop(struct cmd *unused)
{
    return false;
}
