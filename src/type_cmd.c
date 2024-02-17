///
///  @file    type_cmd.c
///  @brief   Execute T and V commands.
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
#include "ascii.h"
#include "editbuf.h"
#include "errors.h"
#include "eflags.h"
#include "estack.h"
#include "exec.h"
#include "term.h"


// Local functions

static void exec_type(int_t m, int_t n);


///
///  @brief    Execute T command: type line(s).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_T(struct cmd *cmd)
{
    assert(cmd != NULL);

    int_t m = 0;
    int_t n = 1;

    if (cmd->h)
    {
        m = -t->dot;
        n = t->Z - t->dot;
    }
    else if (cmd->m_set && cmd->n_set)
    {
        if (cmd->m_arg > cmd->n_arg)    // Swap m and n if needed
        {
            cmd->m_arg ^= cmd->n_arg;
            cmd->n_arg ^= cmd->m_arg;
            cmd->m_arg ^= cmd->n_arg;
        }

        if (cmd->m_arg < t->B || cmd->m_arg > t->Z
            || cmd->n_arg < t->B || cmd->n_arg > t->Z)
        {
            throw(E_POP, "T");          // Pointer off page
        }

        m = cmd->m_arg - t->dot;
        n = cmd->n_arg - t->dot;
    }
    else if (cmd->n_set)
    {
        if (cmd->n_arg == 0)
        {
            m = -t->pos;
            n = 0;
        }
        else if (cmd->n_arg < 0)
        {
            m = len_edit(cmd->n_arg);
            n = 0;
        }
        else
        {
            m = 0;
            n = len_edit(cmd->n_arg);
        }
    }

    exec_type(m, n);
}


///
///  @brief    Execute type command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_type(int_t m, int_t n)
{
    int last = EOF;

    for (int_t i = m; i < n; ++i)
    {
        int c = read_edit(i);

        if (c == EOF)
        {
            break;
        }
        else if (f.e3.CR_type && c == LF && last != CR)
        {
            type_out(CR);
        }

        type_out(c);

        last = c;
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
    confirm(cmd, NO_NEG_M);

    int_t m = cmd->m_arg;
    int_t n = cmd->n_arg;

    if (cmd->m_set)
    {
        m = len_edit(1 - m);
        n = len_edit(n - 1);
    }
    else
    {
        m = len_edit(1 - n);
        n = len_edit(n);
    }

    exec_type(m, n);
}


///
///  @brief    Scan T command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_T(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    if (cmd->m_set)
    {
        default_n(cmd, (int_t)0);       // m,T => m,0T

        if (cmd->m_arg > cmd->n_arg)    // Swap m and n if m > n
        {
            int_t n_arg= cmd->n_arg;

            cmd->n_arg = cmd->m_arg;
            cmd->m_arg = n_arg;
        }
    }
    else
    {
        default_n(cmd, (int_t)1);       // T => 1T
    }

    return false;
}


///
///  @brief    Scan V command.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_V(struct cmd *cmd)
{
    assert(cmd != NULL);

    scan_x(cmd);
    confirm(cmd, NO_COLON, NO_DCOLON, NO_ATSIGN);

    if (!cmd->n_set || cmd->n_arg == 0) // V => 1V, 0V => 1V
    {
        cmd->n_set = true;
        cmd->n_arg = 1;
    }

    return false;
}
