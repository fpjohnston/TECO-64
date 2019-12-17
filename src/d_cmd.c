///
///  @file    d_cmd.c
///  @brief   Execute D command.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
///
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files (the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "textbuf.h"
#include "errors.h"
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

    uint Z = getsize_tbuf();
    int n = 1;
    int m;

    if (cmd->n_set)
    {
        n = cmd->n_arg;
    }

    if (cmd->m_set)                     // m,nD - same as m,nK
    {
        m = cmd->m_arg;

        if (m < 0 || (uint)m > Z || n < 0 || (uint)n > Z || m > n)
        {
            print_err(E_POP);           // Pointer off page
        }

        setpos_tbuf((uint)m);           // Go to first position

        n -= m;                         // And delete this many chars
    }

    uint dot = getpos_tbuf();

    if ((n < 0 && (uint)-n > dot) || (n > 0 && (uint)n > Z - dot))
    {
        if (!cmd->colon_set)
        {
            print_err(E_DTB);           // Delete too big
        }
        
        push_expr(TECO_FAILURE, EXPR_VALUE);

        return;
    }

    delete_tbuf(n);

    if (cmd->colon_set)
    {
        push_expr(TECO_SUCCESS, EXPR_VALUE);
    }
}

