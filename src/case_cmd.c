///
///  @file    case_cmd.c
///  @brief   Execute commands to change lower and upper case.
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "editbuf.h"
#include "errcodes.h"
#include "exec.h"


// Local functions

static void exec_case(struct cmd *cmd, bool lower);


///
///  @brief    Execute "FL" command: convert characters to lower case.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_FL(struct cmd *cmd)
{
    exec_case(cmd, (bool)true);
}


///
///  @brief    Execute "FU" command: convert characters to upper case.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_FU(struct cmd *cmd)
{
    exec_case(cmd, (bool)false);
}


///
///  @brief    Execute upper or lower case command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exec_case(struct cmd *cmd, bool lower)
{
    assert(cmd != NULL);

    int_t dot = t.dot;
    int_t Z   = t.Z;
    int_t m, n;

    if (cmd->h)                         // HFU/HFL?
    {
        m = 0 - dot;
        n = Z - dot;
    }
    else
    {
        if (cmd->m_set)                 // m,nFU/m,nFL
        {
            if (cmd->n_set)
            {
                n = cmd->n_arg;
            }
            else
            {
                n = 0;
            }

            m = cmd->m_arg;

            if (m < 0 || m > Z || n < 0 || n > Z || m > n)
            {
                throw(E_POP, '?');      // Pointer off page
            }

            m -= dot;
            n -= dot;
        }
        else
        {
            if (cmd->n_set)
            {
                n = cmd->n_arg;
            }
            else
            {
                n = 1;
            }

            if (n < 1)
            {
                m = getdelta_ebuf(n);
                n = 0;
            }
            else
            {
                m = 0;
                n = getdelta_ebuf(n);
            }
        }
    }

    int_t saved_dot = t.dot;

    for (int_t i = m; i < n; ++i)
    {
        int c = getchar_ebuf((int_t)0);

        if (c == EOF)
        {
            break;
        }

        if (lower && isupper(c))
        {
            delete_ebuf((int_t)1);
            (void)add_ebuf(tolower(c));
        }
        else if (islower(c))
        {
            delete_ebuf((int_t)1);
            (void)add_ebuf(toupper(c));
        }
        else
        {
            setpos_ebuf(t.dot + 1);
        }
    }

    setpos_ebuf(saved_dot);
}


///
///  @brief    Scan "FL" and "FU" commands.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_FLFU(struct cmd *cmd)
{
    assert(cmd != NULL);

    reject_neg_m(cmd->m_set, cmd->m_arg);
    require_n(cmd->m_set, cmd->n_set);
    reject_colon(cmd->colon);
    reject_atsign(cmd->atsign);

    return false;
}
