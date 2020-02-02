///
///  @file    p_cmd.c
///  @brief   Execute P command.
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
#include "ascii.h"
#include "textbuf.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"


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
        print_err(E_NFO);               // No file for output
    }

    int start = t.B;
    int end   = t.Z;
    int count  = 1;
    bool ff    = false;
    bool yank  = false;

    // Use of a colon only makes sense for P and nP

    if (f.e1.strict && cmd->colon_set)
    {
        if (cmd->m_set || cmd->h_set || cmd->w_set)
        {
            print_err(E_MOD);           // Invalid modifier
        }
    }

    if (cmd->h_set)                     // HPW, HP
    {
        ;                               // Just accept all presets above
    }
    else if (cmd->m_set)
    {
        assert(cmd->n_set == true);

        // TODO: check whether this is correct

        if (cmd->m_arg < (int)start || cmd->m_arg >= (int)end ||
            cmd->n_arg < (int)start || cmd->n_arg >= (int)end)
        {
            printc_err(E_POP, 'P');     // Pointer off page
        }

        start = cmd->m_arg;
        end   = cmd->n_arg;
    }
    else if (cmd->n_set)                // nP, n:P, nPW
    {
        if ((count = cmd->n_arg) == 0)
        {
            print_err(E_IPA);           // Negative argument to P
        }
        else if (count < 0)             // -nP?
        {
            print_err(E_T32);           // Unimplemented TECO-32 feature
        }

        if (cmd->w_set)                 // Is it nPW?
        {
            ff = true;
        }
        else                            // Must be nP or n:P
        {
            ff = v.ff;
            yank = true;
        }
    }
    else                                // P, :P, PW
    {
        count = 1;

        if (cmd->w_set)                 // Is it PW?
        {
            ff = true;
        }
        else                            // Must be P or :P
        {
            ff = v.ff;
            yank = true;
        }
    }

    for (int i = 0; i < count; ++i)
    {
        if (!next_page(start, end, ff, yank))
        {
            if (cmd->colon_set)
            {
                push_expr(TECO_FAILURE, EXPR_VALUE);
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

    if (cmd->colon_set)
    {
        push_expr(TECO_SUCCESS, EXPR_VALUE);
    }
}


///
///  @brief    Write out current page. If needed, add form feed, clear buffer,
///            and read next page.
///
///  @returns  true if more data, false if not.
///
////////////////////////////////////////////////////////////////////////////////

bool next_page(int start, int end, bool ff, bool yank)
{
    struct ofile *ofile = &ofiles[ostream];
    int m = start - t.dot;
    int n = end - t.dot;

    assert(ofile != NULL && ofile->fp != NULL);

    for (int i = m; i < n; ++i)
    {
        int c = getchar_tbuf(i);

        if (c == LF && f.e2.add_cr)
        {
            fputc(CR, ofile->fp);
        }

        fputc(c, ofile->fp);
    }

    if (ff)                             // Add a form feed if necessary
    {
        fputc(FF, ofile->fp);
    }

    if (yank)                           // Yank next page if we need to
    {
        setpos_tbuf(t.B);

        delete_tbuf(t.Z);               // Kill the whole buffer

        struct ifile *ifile = &ifiles[istream];

        if (ifile->fp == NULL || !append((bool)false, 0, (bool)false))
        {
            return false;               // False if no more data
        }
    }

    return true;
}
