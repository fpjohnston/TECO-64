///
///  @file    delete_cmd.c
///  @brief   Execute delete commands.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "editbuf.h"
#include "errcodes.h"
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
    assert(cmd != NULL);                // Error if no command block

    int n = 1;
    int m;

    if (cmd->n_set)
    {
        n = cmd->n_arg;
    }

    if (cmd->m_set)                     // m,nD - same as m,nK
    {
        m = cmd->m_arg;

        if (m < 0 || m > t.Z || n < 0 || n > t.Z || m > n)
        {
            throw(E_POP, 'D');          // Pointer off page
        }

        setpos_ebuf(m);                 // Go to first position

        n -= m;                         // And delete this many chars
    }

    if ((n < 0 && -n > t.dot) || (n > 0 && n > t.Z - t.dot))
    {
        if (!cmd->colon)
        {
            throw(E_DTB);               // Delete too big
        }

        push_expr(0, EXPR_VALUE);

        return;
    }

    delete_ebuf(n);

    if (cmd->colon)
    {
        push_expr(-1, EXPR_VALUE);
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
    assert(cmd != NULL);                // Error if no command block

    if (cmd->h)                         // HK?
    {
        if (t.Z != 0)
        {
            setpos_ebuf(t.B);

            delete_ebuf(t.Z);           // Yes, kill the whole buffer
        }

        return;
    }

    int n = 1;
    int m;

    if (cmd->n_set)
    {
        n = cmd->n_arg;
    }

    if (cmd->m_set)                     // m,nK
    {
        m = cmd->m_arg;

        if (m < 0 || m > t.Z || n < 0 || n > t.Z || m > n)
        {
            throw(E_POP, 'K');          // Pointer off page
        }

        setpos_ebuf(m);                 // Go to first position

        n -= m;                         // And delete this many chars
    }
    else
    {
        n = getdelta_ebuf(n);
    }

    if (n != 0)
    {
        delete_ebuf(n);
    }
}
