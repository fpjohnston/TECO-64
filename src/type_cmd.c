///
///  @file    type_cmd.c
///  @brief   Execute T and V commands.
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
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "editbuf.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"
#include "term.h"


// Local functions

static void exec_type(int m, int n);


///
///  @brief    Execute T command: type line(s).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_T(struct cmd *cmd)
{
    assert(cmd != NULL);

    int m;
    int n;

    if (cmd->h_set)
    {
        m = -t.dot;
        n = t.Z - t.dot;
    }
    else if (cmd->m_set && cmd->n_set)
    {
        if (cmd->m_arg > t.Z || cmd->n_arg > t.Z)
        {
            throw(E_POP, 'T');          // Pointer off page
        }

        if (cmd->m_arg > cmd->n_arg)
        {
            return;
        }

        m = cmd->m_arg - t.dot;
        n = cmd->n_arg - t.dot;
    }
    else if (cmd->n_set)
    {
        if (cmd->n_arg == 0)
        {
            m = getdelta_ebuf(0);
            n = 0;
        }
        else if (cmd->n_arg < 0)
        {
            m = getdelta_ebuf(cmd->n_arg);
            n = 0;
        }
        else
        {
            m = 0;
            n = getdelta_ebuf(cmd->n_arg);
        }
    }
    else
    {
        m = 0;
        n = getdelta_ebuf(1);
    }

    exec_type(m, n);
}


///
///  @brief    Execute type command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_type(int m, int n)
{
    for (int i = m; i < n; ++i)
    {
        int c = getchar_ebuf(i);

        if (f.et.image)
        {
            print_chr(c);
        }
        else
        {
            if (c == LF)
            {
                echo_out(CR);
            }

            echo_out(c);
        }
    }
}


///
///  @brief    Execute V command: type line(s).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_V(struct cmd *cmd)
{
    assert(cmd != NULL);

    int m;
    int n;

    if (cmd->n_set)
    {
        if (cmd->m_set)
        {
            m = getdelta_ebuf(1 - cmd->m_arg);
            n = getdelta_ebuf(cmd->n_arg - 1);
        }
        else
        {
            m = getdelta_ebuf(1 - cmd->n_arg);
            n = getdelta_ebuf(cmd->n_arg);
        }
    }
    else
    {
        m = getdelta_ebuf(0);
        n = getdelta_ebuf(1);
    }

    exec_type(m, n);
}
