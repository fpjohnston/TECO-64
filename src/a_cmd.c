///
///  @file    a_cmd.c
///  @brief   Execute A command.
///
///  @copyright 2019-2022 Franklin P. Johnston / Nowwith Treble Software
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
#include "eflags.h"
#include "errcodes.h"
#include "estack.h"
#include "exec.h"
#include "file.h"


///
///  @brief    Append to edit buffer (A, :A, and n:A commands).
///
///  @returns  true if success, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool append(bool n_set, int_t n_arg, bool colon)
{
    struct ifile *ifile = &ifiles[istream];

    if (ifile->fp == NULL)
    {
        throw(E_NFI);                   // No file for input
    }

    if (n_set && n_arg < 0)
    {
        throw(E_IAA);                   // Invalid A argument
    }

    f.ctrl_e = false;                   // Assume not appending FF

    int_t olddot = t->dot;

    set_dot(t->Z);                      // Go to end of buffer

    if (feof(ifile->fp))                // Already at EOF?
    {
        return false;
    }
    else if (colon && n_set)            // n:A -> append n lines
    {
        for (int_t i = 0; i < n_arg; ++i)
        {
            if (!append_edit(ifile, (bool)true)) // Append a single line
            {
                break;
            }
        }
    }
    else                                // A or :A
    {
        (void)append_edit(ifile, (bool)false); // Append all we can
    }

    set_dot(olddot);

    return true;
}


///
///  @brief    Execute A command: append lines to buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_A(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->colon)                     // :A or n:A command
    {
        bool success = append(cmd->n_set, cmd->n_arg, cmd->colon);

        push_x(success ? SUCCESS : FAILURE, X_OPERAND);
    }
    else if (!cmd->n_set)               // A command
    {
        (void)append((bool)false, (int_t)0, cmd->colon);
    }
}


///
///  @brief    Scan A command: get value of character in buffer.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_A(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (!cmd->n_set || cmd->colon)      // A, :A, or n:A command
    {
        return false;
    }

    int_t n = read_edit(cmd->n_arg);

    push_x(n, X_OPERAND);               // Note: n may be EOF (-1)

    return true;
}
