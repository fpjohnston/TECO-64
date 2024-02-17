///
///  @file    x_cmd.c
///  @brief   Execute X command.
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
#include "editbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "qreg.h"


///
///  @brief    Execute X command: copy lines to Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_X(struct cmd *cmd)
{
    assert(cmd != NULL);
    confirm(cmd, NO_NEG_M);

    int_t n = 1;
    int_t m;

    if (cmd->n_set)                     // n argument?
    {
        n = cmd->n_arg;
    }

    if (cmd->m_set)                     // m argument too?
    {
        m = cmd->m_arg;

        if (m > n)                      // Swap m and n if needed
        {
            m ^= n;
            n ^= m;
            m ^= n;
        }

        if (m < t->B || m > t->Z || n < t->B || n > t->Z)
        {
            throw(E_POP, "X");          // Pointer off page
        }
        else if (m == 0 && n == 0)
        {
            // Use of 0,0Xq is supposedly not valid, according to the May 1990
            // manual; regardless, it is used in macros to delete Q-register
            // text storage, so we've implemented it here.

            delete_qtext(cmd->qindex);

            return;
        }

        // Change absolute positions to relative positions.

        m -= t->dot;
        n -= t->dot;
    }
    else
    {
        int_t delta = len_edit(n);

        if (n <= 0)
        {
            m = delta;
            n = 0;
        }
        else
        {
            m = 0;
            n = delta;
        }
    }

    if (!cmd->colon)                    // Delete any text if not appending
    {
        delete_qtext(cmd->qindex);
    }

    for (int_t i = m; i < n; ++i)
    {
        int c = read_edit(i);

        if (c == EOF)
        {
            break;
        }

        append_qchr(cmd->qindex, c);
    }
}


///
///  @brief    Scan X command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_X(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_DCOLON, NO_ATSIGN);

    default_n(cmd, (int_t)1);           // X => 1X

    if (!scan_qreg(cmd))
    {
        throw(E_IQN, cmd->qname);       // Invalid Q-register name
    }

    return false;
}
