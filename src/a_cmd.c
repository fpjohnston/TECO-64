///
///  @file    a_cmd.c
///  @brief   Execute "A" command.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
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
        throw(E_ARG);                   // Invalid arguments
    }

    f.ctrl_e = false;                   // Assume not appending FF

    int_t olddot = t.dot;

    setpos_ebuf(t.Z);                   // Go to end of buffer

    if (feof(ifile->fp))                // Already at EOF?
    {
        return false;
    }
    else if (n_set)                     // n:A -> append n lines
    {
        for (int_t i = 0; i < n_arg; ++i)
        {
            if (!append_line())         // If we should stop
            {
                break;
            }
        }
    }
    else if (!colon)                    // A -> append entire page
    {
        while (append_line())           // Append all we can
        {
            ;
        }
    }
    else
    {
        (void)append_line();            // :A -> append single line
    }

    setpos_ebuf(olddot);

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
    bool first_line = (ftell(ifile->fp) == 0);
    int next = fgetc(ifile->fp);
    int c;

    while ((c = next) != EOF)
    {
        next = fgetc(ifile->fp);

        if (c == NUL && !f.e3.keepnul)  // Discard NUL chrs. if necessary
        {
            continue;
        }
        else if (c == FF && !f.e3.nopage) // Is it FF, and is it a page delimiter?
        {
            f.ctrl_e = true;            // Yes, flag it, but don't store it

            if (next != EOF)
            {
                (void)ungetc(next, ifile->fp);
            }

            return false;               // And say we need to stop
        }
        else if (c == CR)
        {
            // Check for discarding CR if start of CR/LF sequence.

            if (next == LF)
            {
                if (f.e3.smart && first_line)
                {
                    first_line = false;
                    f.e3.icrlf = true;
                    f.e3.ocrlf = true;
                }

                if (!f.e3.icrlf)
                {
                    continue;
                }
            }
        }
        else if (c == LF)
        {
            if (f.e3.smart && first_line)
            {
                first_line = false;
                f.e3.icrlf = false;
                f.e3.ocrlf = false;
            }
        }

        switch (add_ebuf(c))
        {
            default:
            case EDIT_OK:
                if (c == LF || c == VT) // Done if line terminator found
                {
                    if (next != EOF)
                    {
                        (void)ungetc(next, ifile->fp);
                    }

                    return true;
                }

                break;

            case EDIT_WARN:             // Set flag if buffer getting full
                if (c == LF || c == VT)
                {
                    if (next != EOF)
                    {
                        (void)ungetc(next, ifile->fp);
                    }

                    return false;
                }

                break;

            case EDIT_FULL:             // Stop if buffer is full
                if (next != EOF)
                {
                    (void)ungetc(next, ifile->fp);
                }

                return false;

            case EDIT_ERROR:
                break;
        }
    }

    return false;
}


///
///  @brief    Execute "A" command: append lines to buffer.
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
///  @brief    Scan "A" command: get value of character in buffer.
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

    int_t n = getchar_ebuf(cmd->n_arg);

    push_x(n, X_OPERAND);               // Note: n may be EOF (-1)

    return true;
}
