///
///  @file    a_cmd.c
///  @brief   Execute A command.
///
///  @bug     No known bugs.
///
///  @copyright  2019-2020 Franklin P. Johnston
///
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files (the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
///
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"
#include "textbuf.h"


///
///  @brief    Append to edit buffer.
///
///  @returns  true if not already at end of file, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool append(bool n_set, int n_arg, bool colon_set)
{
    struct ifile *ifile = &ifiles[istream];

    if (ifile->fp == NULL)
    {
        print_err(E_NFI);               // No file for input
    }       

    v.ff = false;                       // Assume no form feed

    // Here if we have A, :A, or n:A

    uint olddot = getpos_tbuf();

    if (ifile->eof)                     // Already at EOF?
    {
        return false;
    }
    else if (n_set)                     // n:A -> append n lines
    {
        for (int i = 0; i < n_arg; ++i)
        {
            if (!append_line())         // If we should stop
            {
                break;
            }
        }
    }
    else if (!colon_set)                // A -> append entire page
    {
        for (;;)                        // Append all we can
        {
            if (!append_line())         // If we should stop
            {
                break;
            }
        }
    }
    else
    {
        (void)append_line();            // :A -> append single line
    }

    setpos_tbuf(olddot);

    return true;
}


///
///  @brief    Append line to edit buffer.
///
///  @returns  true if OK to add more, false if buffer full or nearly so.
///
////////////////////////////////////////////////////////////////////////////////

bool append_line(void)
{
    struct ifile *ifile = &ifiles[istream];
    int c;
 
    while ((c = fgetc(ifile->fp)) != EOF)
    {
        if (c == FF && !f.e2.no_ff)     // If form feed, don't store it
        {
            v.ff = true;                // But do flag it

            return true;
        }

        switch (add_tbuf(c))
        {
            default:
            case EDIT_OK:
                if (c == LF || c == VT) // Done if line terminator found
                {
                    return true;
                }

                break;

            case EDIT_WARN:             // Set flag if buffer getting full
                v.warn = true;

                if (c == LF || c == VT)
                {
                    return false;
                }

                break;

            case EDIT_FULL:             // Stop if buffer is full
                v.full = true;

                return false;
        }
    }

    ifile->eof = true;

    return false;
}


///
///  @brief    Parse A command: get value of character in buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_A(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->n_set)
    {
        if (!cmd->colon_set)
        {
            int n = getchar_tbuf(cmd->n_arg);

            push_expr(n, EXPR_VALUE);
        }
    }
    else
    {
        // Here to append to buffer with A, :A, or n:A command.

        int status = append(cmd->n_set, cmd->n_arg, cmd->colon_set);

        if (cmd->colon_set)
        {
            push_expr(status, EXPR_VALUE);
        }
    }
}

