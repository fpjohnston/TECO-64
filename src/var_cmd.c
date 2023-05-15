///
///  @file    var_cmd.c
///  @brief   Scan commands that are simple variables.
///
///  @copyright 2019-2023 Franklin P. Johnston / Nowwith Treble Software
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
#include <stdio.h>

#include "teco.h"
#include "cbuf.h"
#include "editbuf.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "page.h"
#include "qreg.h"


///
///  @brief    Execute CTRL/Q command: get no. of characters between dot and nth
///            line terminator. n may be negative.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_Q(struct cmd *cmd)
{
    assert(cmd != NULL);

    int_t nchrs;

    if (cmd->n_set)
    {
        nchrs = len_edit(cmd->n_arg);
    }
    else
    {
        nchrs = t->pos;
    }

    store_val(nchrs);
}


///
///  @brief    Scan CTRL/P command: get current page number.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_P(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_ATSIGN);

    store_val((int_t)page_count());

    return true;
}


///
///  @brief    Scan CTRL/Q command.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_Q(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_ATSIGN);

    return false;
}


///
///  @brief    Scan CTRL/S command: return negative of last insert, string
///            found, or string inserted with a G command, whichever occurred
///            last.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_S(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_ATSIGN);

    store_val(-(int_t)last_len);

    return true;
}


///
///  @brief    Scan CTRL/Y command: equivalent to .+^S,.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_Y(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_ATSIGN);

    // The following prevents expressions such as 123+^Y.

    int_t n;

    if (f.e2.args)
    {
        if (query_x(&n) || cmd->m_set)
        {
            throw(E_ARG);               // Invalid arguments
        }
    }

    (void)query_x(&n);                  // Ignore any existing operand

    cmd->ctrl_y = true;
    cmd->m_set = true;
    cmd->m_arg = t->dot - (int_t)last_len;

    store_val(t->dot);

    return true;
}


///
///  @brief    Scan CTRL/Z command: get no. of chrs. in all Q-registers.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_Z(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_ATSIGN);

    int_t n = (int_t)get_qall();

    store_val(n);

    return true;
}


///
///  @brief    Scan CTRL/^ command: get literal value of next character
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_ctrl_up(struct cmd *cmd)
{
    assert(cmd != NULL);

    int c = require_cbuf();

    store_val((int_t)c);

    return true;
}


///
///  @brief    Scan . command: get current position in buffer.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_dot(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_ATSIGN);

    store_val(t->dot);

    return true;
}


///
///  @brief    Scan B command: read first position in buffer (always 0).
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_B(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_ATSIGN);

    store_val(t->B);

    return true;
}


///
///  @brief    Scan H command: equivalent to B,Z.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_H(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_ATSIGN);

    // The following prevents expressions such as 123+H.

    int_t n;

    if (f.e2.args)
    {
        if (query_x(&n) || cmd->m_set)
        {
            throw(E_ARG);               // Invalid arguments
        }
    }

    (void)query_x(&n);                  // Ignore any existing operand

    cmd->h = true;
    cmd->m_set = true;
    cmd->m_arg = t->B;

    store_val(t->Z);

    return true;
}


///
///  @brief    Scan Z command: read last position in buffer.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_Z(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_COLON, NO_ATSIGN);

    store_val(t->Z);

    return true;
}
