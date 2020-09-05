///
///  @file    var_cmd.c
///  @brief   Execute commands that are simple variables.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include <stdlib.h>

#include "teco.h"
#include "editbuf.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "qreg.h"


///
///  @brief    Scan B command: read first position in buffer (always 0).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_B(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    push_expr(t.B, EXPR_VALUE);
}


///
///  @brief    Scan ^Q (CTRL/Q) command: get no. of characters between dot and
///            nth line terminator. n may be negative.
///
///  @returns  nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_Q(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    if (!cmd->n_set)
    {
        cmd->n_arg = 0;
    }

    int nchrs = getdelta_ebuf(cmd->n_arg);

    push_expr(nchrs, EXPR_VALUE);
}


///
///  @brief    Parse ^S (CTRL/S) command: return negative of last insert, string
///            found, or string inserted with a G command, whichever occurred
///            last.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_S(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    push_expr(-(int)last_len, EXPR_VALUE);
}


///
///  @brief    Scan ^Y (CTRL/Y) command: equivalent to .+^S,.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_Y(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    uint i = estack.level;

    // The following prevents expressions such as 123+^Y.

    if (f.e2.args && (check_expr() || cmd->m_set ||
                      (i != 0 && estack.obj[i].value != TYPE_GROUP)))
    {
        throw(E_ARG);                   // Invalid arguments
    }

    cmd->ctrl_y = true;
    cmd->m_set = true;
    cmd->m_arg = t.dot - (int)last_len;

    push_expr(t.dot, EXPR_VALUE);
}


///
///  @brief    Scan ^Z (CTRL/Z) command: get no. of chrs. in all Q-registers.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_ctrl_Z(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    uint n = get_qall();

    push_expr((int)n, EXPR_VALUE);
}



///
///  @brief    Scan . (dot) command: get current position in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_dot(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    push_expr((int)t.dot, EXPR_VALUE);
}


///
///  @brief    Scan H command: equivalent to B,Z.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_H(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    uint i = estack.level;

    // The following prevents expressions such as 123+H.

    if (f.e2.args && (check_expr() || cmd->m_set ||
                      (i != 0 && estack.obj[i].value != TYPE_GROUP)))
    {
        throw(E_ARG);                   // Invalid arguments
    }

    cmd->h = true;
    cmd->m_set = true;
    cmd->m_arg = t.B;

    push_expr((int)t.Z, EXPR_VALUE);
}


///
///  @brief    Scan Z command: read last position in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_Z(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    push_expr((int)t.Z, EXPR_VALUE);
}
