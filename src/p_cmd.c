///
///  @file    p_cmd.c
///  @brief   Execute P command.
///
///  @copyright  2019-2020 Franklin P. Johnston / Nowwith Treble Software
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
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "file.h"
#include "page.h"


///
///  @brief    Execute P command (write out buffer, and read next page).
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_P(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    struct ofile *ofile = &ofiles[ostream];

    if (ofile->fp == NULL)
    {
        throw(E_NFO);                   // No file for output
    }

    int start = t.B;
    int end   = t.Z;
    int count  = 1;
    bool ff    = false;
    bool yank  = false;

    // Use of a colon only makes sense for P and nP

    if (f.e2.page && cmd->colon)
    {
        if (cmd->m_set || cmd->h || cmd->w)
        {
            throw(E_COL);               // Illegal colon
        }
    }

    if (cmd->h)                         // HPW, HP
    {
        ;                               // Just accept all presets above
    }
    else if (cmd->m_set)                // We assume n was also set
    {
        if (cmd->m_arg < (int)start || cmd->m_arg >= (int)end ||
            cmd->n_arg < (int)start || cmd->n_arg >= (int)end)
        {
            throw(E_POP, 'P');          // Pointer off page
        }

        start = cmd->m_arg;
        end   = cmd->n_arg;
    }
    else if (cmd->n_set)                // nP, n:P, nPW
    {
        if ((count = cmd->n_arg) == 0)
        {
            throw(E_NPA);               // Negative or zero argument to P or PW
        }
        else if (count < 0)             // -nP?
        {
            page_backward(ofile->fp, -count); // Try to read previous page(s)

            return;
        }

        if (cmd->w)                     // Is it nPW?
        {
            ff = !f.e3.nopage;
        }
        else                            // Must be nP or n:P
        {
            ff = f.ctrl_e;
            yank = true;
        }
    }
    else                                // P, :P, PW
    {
        count = 1;

        if (cmd->w)                     // Is it PW?
        {
            ff = !f.e3.nopage;
        }
        else                            // Must be P or :P
        {
            ff = f.ctrl_e;
            yank = true;
        }
    }

    for (int i = 0; i < count; ++i)
    {
        if (!next_page(start, end, ff, yank))
        {
            if (cmd->colon)
            {
                push_expr(0, EXPR_VALUE);
            }

            return;
        }

        // If the command isn't m,nP or m,nPW, then we're always writing out
        // the entire buffer, so check the start and end positions each time.

        if (!cmd->m_set)
        {
            start = t.B;
            end   = t.Z;
        }
    }

    if (cmd->colon)
    {
        push_expr(-1, EXPR_VALUE);
    }
}


///
///  @brief    Write current page and, if requested, read next page.
///
///  @returns  false if unable to read another page because already at EOF,
///            else true (note that true can also mean that we were not
///            asked to attempt to read another page).
///
////////////////////////////////////////////////////////////////////////////////

bool next_page(int start, int end, bool ff, bool yank)
{
    page_forward(ofiles[ostream].fp, start - t.dot, end - t.dot, ff);

    if (yank)                           // Yank next page if we need to
    {
        setpos_ebuf(t.B);

        delete_ebuf(t.Z);               // Kill the whole buffer

        struct ifile *ifile = &ifiles[istream];

        if (ifile->fp == NULL || !append((bool)false, 0, (bool)false))
        {
            return false;               // False if no more data
        }
    }

    return true;
}
