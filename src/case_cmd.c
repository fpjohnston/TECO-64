///
///  @file    case_cmd.c
///  @brief   Execute commands to change lower and upper case.
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "editbuf.h"
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

    int dot = (int)t.dot;
    int Z   = (int)t.Z;
    int m, n;

    if (cmd->h_set)                     // HFU/HFL?
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

            m = 0;
            n = getdelta_ebuf(n);
        }
    }

    for (int i = m; i < n; ++i)
    {
        int c = getchar_ebuf(i);

        if (c == EOF)
        {
            break;
        }

        if (lower)
        {
            if (isupper(c))
            {
                (void)putchar_ebuf(i, tolower(c));
            }
        }
        else
        {
            if (islower(c))
            {
                (void)putchar_ebuf(i, toupper(c));
            }
        }
    }

    setpos_ebuf(dot + n);
}
