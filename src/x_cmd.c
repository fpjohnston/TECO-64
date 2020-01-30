///
///  @file    x_cmd.c
///  @brief   Execute X command.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
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
#include "errors.h"
#include "exec.h"
#include "qreg.h"
#include "textbuf.h"


///
///  @brief    Execute X command: copy lines to Q-register.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_X(struct cmd *cmd)
{
    assert(cmd != NULL);

    int n = 1;
    int m;
    uint dot = getpos_tbuf();

    if (cmd->n_set)                     // n argument?
    {
        n = cmd->n_arg;
    }

    if (cmd->m_set)                     // m argument too?
    {
        uint Z = getsize_tbuf();

        m = cmd->m_arg;

        if (m < 0 || (uint)m > Z || n < 0 || (uint)n > Z)
        {
            printc_err(E_POP, 'X');     // Pointer off page
        }
        else if (m == 0 && n == 0)
        {
            // Use of 0,0Xq is supposedly not valid, according to the May 1990
            // manual; regardless, it is used in macros to delete Q-register
            // text storage, so we've implemented it here.

            delete_qtext(cmd->qname, cmd->qlocal);

            return;
        }
        else if (m >= n || n == 0)
        {
            print_err(E_ARG);           // Invalid arguments
        }
    }
    else
    {
        int delta = getdelta_tbuf(n);

        if (n <= 0)
        {
            m = (int)dot + delta;
            n = (int)dot;
        }
        else
        {
            m = (int)dot;
            n = (int)dot + delta;
        }
    }

    // TODO: clean this up: m and n should be relative to dot.

    for (int i = m - (int)dot; i < n - (int)dot; ++i)
    {
        int c = getchar_tbuf(i);

        if (i == m - (int)dot && !cmd->colon_set)
        {
            store_qchr(cmd->qname, cmd->qlocal, c);
        }
        else
        {
            append_qchr(cmd->qname, cmd->qlocal, c);
        }
    }
}
