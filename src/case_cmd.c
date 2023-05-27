 ///
///  @file    case_cmd.c
///  @brief   Execute commands to change lower and upper case.
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "editbuf.h"
#include "eflags.h"                 // Needed for confirm()
#include "errors.h"
#include "exec.h"


// Local functions

static void exec_case(struct cmd *cmd, bool lower);


///
///  @brief    Execute FL command: convert characters to lower case.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_FL(struct cmd *cmd)
{
    exec_case(cmd, (bool)true);
}


///
///  @brief    Execute FU command: convert characters to upper case.
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

    int_t dot = t->dot;
    int_t Z   = t->Z;
    int_t m, n;

    if (cmd->h)                         // HFU/HFL?
    {
        m = 0 - dot;
        n = Z - dot;
    }
    else if (cmd->m_set)                // m,nFL or m,nFU
    {
        m = cmd->m_arg;
        n = cmd->n_set ? cmd->n_arg : t->dot;

        if (m > n)                      // Swap m and n if needed
        {
            m ^= n;
            n ^= m;
            m ^= n;
        }

        if (m < 0 || m > Z || n < 0 || n > Z)
        {
            if (lower)
            {
                throw(E_POP, "FL");     // Pointer off page
            }
            else
            {
                throw(E_POP, "FU");     // Pointer off page
            }
        }

        // Make position relative to dot

        m -= dot;
        n -= dot;
    }
    else                                // nFL or nFU
    {
        n = cmd->n_set ? cmd->n_arg : 1;

        if (n < 1)
        {
            m = len_edit(n);
            n = 0;
        }
        else
        {
            m = 0;
            n = len_edit(n);
        }
    }

    int_t saved_dot = t->dot;

    set_dot(t->dot + m);

    for (int_t i = m; i < n; ++i)
    {
        int c = t->c;

        if (c == EOF)
        {
            break;
        }

        if (lower && isupper(c))
        {
            change_dot(tolower(c));
        }
        else if (!lower && islower(c))
        {
            change_dot(toupper(c));
        }
        else
        {
            set_dot(t->dot + 1);
        }
    }

    set_dot(saved_dot);
}


///
///  @brief    Scan FL and FU commands.
///
///  @returns  false (command is not an operand or operator).
///
////////////////////////////////////////////////////////////////////////////////

bool scan_case(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_NEG_M, NO_M_ONLY, NO_COLON, NO_DCOLON, NO_ATSIGN);

    return false;
}
