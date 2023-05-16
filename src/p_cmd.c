///
///  @file    p_cmd.c
///  @brief   Execute P command.
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
#include "cbuf.h"
#include "editbuf.h"
#include "eflags.h"                 // Needed for confirm()
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
    assert(cmd != NULL);

    struct ofile *ofile = &ofiles[ostream];

    if (ofile->fp == NULL)
    {
        throw(E_NFO);                   // No file for output
    }

    int_t start = t->B;
    int_t end = t->Z;
    int_t count = 1;
    bool ff   = false;
    bool yank = false;

    // Use of a colon only makes sense for P and nP

    if (f.e2.page && cmd->colon)
    {
        if (cmd->m_set || cmd->h || cmd->c2 == 'W')
        {
            throw(E_COL);               // Invalid colon
        }
    }

    if (cmd->h)                         // HPW, HP
    {
        ;                               // Just accept all presets above
    }
    else if (cmd->m_set)                // We assume n was also set
    {
        if (cmd->m_arg < t->B || cmd->m_arg > t->Z ||
            cmd->n_arg < t->B || cmd->n_arg > t->Z)
        {
            throw(E_POP, "P");          // Pointer off page
        }

        if (start > end)
        {
            start = cmd->n_arg;
            end   = cmd->m_arg;
        }
        else
        {
            start = cmd->m_arg;
            end   = cmd->n_arg;
        }
    }
    else if (cmd->n_set)                // nP, n:P, nPW
    {
        if ((count = cmd->n_arg) == 0 ||
            (count < 0 && cmd->c2 == 'W'))
        {
            throw(E_IPA);               // Negative or zero argument to P or PW
        }

        if (cmd->c2 == 'W')             // Is it nPW?
        {
            ff = !f.e3.nopage;
        }
        else                            // Must be nP or n:P
        {
            ff = f.ctrl_e;
        }

        if (count < 0)                  // -nP?
        {
            (void)page_backward(count, ff); // Try to read previous page(s)

            return;
        }

        if (cmd->c2 != 'W')             // Is it nPW?
        {
            yank = true;
        }
    }
    else                                // P, :P, PW
    {
        count = 1;

        if (cmd->c2 == 'W')             // Is it PW?
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
                store_val(FAILURE);
            }

            return;
        }

        if (cmd->c2 == 'W')
        {
            page_flush(ofiles[ostream].fp);
        }

        // If the command isn't m,nP or m,nPW, then we're always writing out
        // the entire buffer, so check the start and end positions each time.

        if (!cmd->m_set)
        {
            start = t->B;
            end   = t->Z;
        }
    }

    if (cmd->colon)
    {
        store_val(SUCCESS);
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

bool next_page(int_t start, int_t end, bool ff, bool yank)
{
    if (!page_forward(ofiles[ostream].fp, start - t->dot, end - t->dot, ff))
    {
        if (yank)                       // Yank next page if we need to
        {
            kill_edit();

            struct ifile *ifile = &ifiles[istream];

            if (ifile->fp == NULL
                || !append((bool)false, (int_t)0, (bool)false))
            {
                return false;           // False if no more data
            }
        }
    }

    return true;
}


///
///  @brief    Scan P command, which may have an optional postfix W.
///
///  @returns  true if command is an operand or operator, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool scan_P(struct cmd *cmd)
{
    assert(cmd != NULL);

    confirm(cmd, NO_NEG_M, NO_M_ONLY, NO_DCOLON, NO_ATSIGN);

    int c = peek_cbuf();

    if (c == 'W' || c == 'w')
    {
        next_cbuf();

        cmd->c2 = 'W';
    }

    return false;
}
