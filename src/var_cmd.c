///
///  @file    var_cmd.c
///  @brief   Scan commands that are simple variables.
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
#include "cbuf.h"
#include "editbuf.h"
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "page.h"
#include "qreg.h"
#include "term.h"


///
///  @brief    Scan "^P" (CTRL/P) command: get current page number.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_P(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    push_x((int_t)page_count, X_OPERAND);

    return true;
}


///
///  @brief    Scan "^Q" (CTRL/Q) command: get no. of characters between
///            dot and nth line terminator. n may be negative.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_Q(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    if (!cmd->n_set)
    {
        cmd->n_arg = 0;
    }

    int_t nchrs = getdelta_ebuf(cmd->n_arg);

    push_x(nchrs, X_OPERAND);

    return true;
}


///
///  @brief    Scan "^S" (CTRL/S) command: return negative of last insert,
///            string found, or string inserted with a G command, whichever
///            occurred last.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_S(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    push_x(-(int_t)last_len, X_OPERAND);

    return true;
}


///
///  @brief    Scan "^Y" (CTRL/Y) command: equivalent to .+^S,.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_Y(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    // The following prevents expressions such as 123+^Y.

    if (f.e2.args)
    {
        if (isoperand() || cmd->m_set)
        {
            throw(E_ARG);                   // Invalid arguments
        }
    }
    else if (isoperand())
    {
        (void)pop_x();                  // Ignore any existing operand
    }

    cmd->ctrl_y = true;
    cmd->m_set = true;
    cmd->m_arg = t.dot - (int_t)last_len;

    push_x(t.dot, X_OPERAND);

    return true;
}


///
///  @brief    Scan "^Z" (CTRL/Z) command: get no. of chrs. in all
///            Q-registers.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_Z(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    int_t n = (int_t)get_qall();

    push_x(n, X_OPERAND);

    return true;
}


///
///  @brief    Scan "^^" command: get literal value of next character
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_up(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c = require_cbuf();

    trace_cbuf(c);

    push_x((int_t)c, X_OPERAND);

    return true;
}


///
///  @brief    Scan "." command: get current position in buffer.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_dot(struct cmd *cmd)
{
    assert(cmd != NULL);

    push_x(t.dot, X_OPERAND);

    return true;
}


///
///  @brief    Scan "B" command: read first position in buffer (always 0).
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_B(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    push_x(t.B, X_OPERAND);

    return true;
}


///
///  @brief    Scan "H" command: equivalent to B,Z.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_H(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    // The following prevents expressions such as 123+H.

    if (f.e2.args)
    {
        if (isoperand() || cmd->m_set)
        {
            throw(E_ARG);               // Invalid arguments
        }
    }
    else if (isoperand())
    {
        (void)pop_x();                  // Ignore any existing operand
    }

    cmd->h = true;
    cmd->m_set = true;
    cmd->m_arg = t.B;

    push_x(t.Z, X_OPERAND);

    return true;
}


///
///  @brief    Scan "Z" command: read last position in buffer.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_Z(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    push_x(t.Z, X_OPERAND);

    return true;
}
