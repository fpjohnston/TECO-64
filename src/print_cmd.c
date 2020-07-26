///
///  @file    print_cmd.c
///  @brief   Print current command (used in dry runs).
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
#include "eflags.h"
#include "exec.h"
#include "term.h"


// Local functions

static void format_chr(int c);

static void format_str(const char *p, uint nbytes);


///
///  @brief    Format character for dry run print out.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void format_chr(int c)
{
    if (c == NUL)
    {
        return;
    }

    if (c == ESC)                       // Use accent grave instead of ESC
    {
        c = '`';
    }

    if (iscntrl(c))                     // Control character
    {
        print_chr('^');
        print_chr((char)c + 'A' - 1);
    }
    else if (isprint(c) || f.et.eightbit) // Print character as is?
    {
        print_chr(c);
    }
    else                                // No, make it printable
    {
        printf("[%02x]", c);
    }
}


///
///  @brief    Format string for dry run print out.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void format_str(const char *p, uint n)
{
    if (p != NULL)
    {
        while (n-- > 0)
        {
            format_chr(*p++);
        }
    }
}


///
///  @brief    Print the current command to terminal and/or log file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void print_cmd(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->m_set || cmd->n_set)
    {
        printf("(");

        if (cmd->m_set)
        {
            printf("%d,", cmd->m_arg);
        }

        if (cmd->n_set)
        {
            printf("%d", cmd->n_arg);
        }

        printf(")");
    }

    format_chr(cmd->colon ? ':' : NUL);
    format_chr(cmd->dcolon ? ':' : NUL);
    format_chr(cmd->atsign ? '@' : NUL);
    format_chr(cmd->c1);
    format_chr(cmd->c2);
    format_chr(cmd->c3);
    format_chr(cmd->qlocal ? '.' : NUL);
    format_chr(cmd->qname);
    format_chr(cmd->atsign ? cmd->delim : NUL);
    format_str(cmd->text1.buf, cmd->text1.len);
    format_chr(cmd->text1.len != 0 ? cmd->delim : NUL);
    format_str(cmd->text2.buf, cmd->text2.len);
    format_chr(cmd->text2.len != 0 ? cmd->delim : NUL);

    print_chr(CRLF);
}
