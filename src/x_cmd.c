///
///  @file    x_cmd.c
///  @brief   Execute X command.
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
#include <stdio.h>
#include <stdlib.h>

#include "teco.h"
#include "editbuf.h"
#include "errcodes.h"
#include "exec.h"
#include "qreg.h"


///
///  @brief    Execute "X" command: copy lines to Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_X(struct cmd *cmd)
{
    assert(cmd != NULL);

    int_t n = 1;
    int_t m;

    if (cmd->n_set)                     // n argument?
    {
        n = cmd->n_arg;
    }

    if (cmd->m_set)                     // m argument too?
    {
        m = cmd->m_arg;

        if (m < 0 || m > t.Z || n < 0 || n > t.Z)
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
        else if (m > n)
        {
            throw(E_ARG);               // Invalid arguments
        }

        // Change absolute positions to relative positions.

        m -= t.dot;
        n -= t.dot;
    }
    else
    {
        int_t delta = getdelta_ebuf(n);

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
        int c = getchar_ebuf(i);

        if (c == EOF)
        {
            break;
        }

        append_qchr(cmd->qindex, c);
    }
}


///
///  @brief    Scan "X" command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_X(struct cmd *cmd)
{
    assert(cmd != NULL);

    default_n(cmd, (int_t)1);           // X => 1X
    reject_neg_m(cmd->m_set, cmd->m_arg);
    reject_dcolon(cmd->dcolon);
    reject_atsign(cmd->atsign);
    scan_qreg(cmd);

    return false;
}
