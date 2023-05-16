///
///  @file    delete_cmd.c
///  @brief   Execute delete commands.
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
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "editbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "estack.h"
#include "exec.h"


///
///  @brief    Execute D command: delete characters at dot.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_D(struct cmd *cmd)
{
    assert(cmd != NULL);

    int_t n = cmd->n_arg;
    int_t m;

    if (cmd->m_set)                     // m,nD - same as m,nK
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
            throw(E_POP, "D");          // Pointer off page
        }

        set_dot(m);                     // Go to first position

        n -= m;                         // And delete this many chars
    }

    if ((n < 0 && -n > t->dot) || (n > 0 && n > t->Z - t->dot))
    {
        if (!cmd->colon)
        {
            throw(E_DTB);               // Delete too big
        }

        store_val(FAILURE);

        return;
    }

    delete_edit(n);

    if (cmd->colon)
    {
        store_val(SUCCESS);
    }
}


///
///  @brief    Execute K command: kill (delete) lines.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_K(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->h)                         // HK?
    {
        kill_edit();                    // Kill the current buffer

        return;
    }

    int_t n = cmd->n_arg;
    int_t m;

    if (cmd->m_set)                     // m,nK
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
            throw(E_POP, "K");          // Pointer off page
        }

        set_dot(m);                     // Go to first position

        n -= m;                         // And delete this many chars
    }
    else
    {
        n = len_edit(n);
    }

    delete_edit(n);
}


///
///  @brief    Scan D command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_D(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_NEG_M, NO_DCOLON, NO_ATSIGN);

    default_n(cmd, (int_t)1);           // D => 1D

    return false;
}


///
///  @brief    Scan K command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_K(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_NEG_M, NO_COLON, NO_ATSIGN);

    default_n(cmd, (int_t)1);           // K => 1K

    return false;
}
