///
///  @file    el_cmd.c
///  @brief   Execute EL command.
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "exec.h"

static char line[256];                  ///< Output line
static uint nbytes;                     ///< Length of output line
static uint sequence = 0;               ///< Command sequence no.

// Local functions

static void log_chr(int c);

static void log_str(const char *p, uint nbytes);


///
///  @brief    Execute EL command (open or close log file). Format is:
///
///            ELfile`         - open log file for write.
///            EL`             - close log file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EL(struct cmd *cmd)
{
    assert(cmd != NULL);

    if (cmd->text1.len == 0)
    {
        close_output(OFILE_LOG);

        return;
    }

    if (open_output(cmd, OFILE_LOG) == EXIT_FAILURE)
    {
        if (!cmd->colon_set)
        {
            prints_err(E_UFO, last_file);
        }

        push_expr(TECO_FAILURE, EXPR_VALUE);
    }
    else if (cmd->colon_set)
    {
        push_expr(TECO_SUCCESS, EXPR_VALUE);
    }
}


///
///  @brief    Format character for log file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void log_chr(int c)
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
        line[nbytes++] = '^';
        line[nbytes++] = (char)c + 'A' - 1;

        assert(nbytes < sizeof(line));
    }
    else if (isprint(c) || f.et.eightbit) // Print character as is?
    {
        line[nbytes++] = (char)c;       // Yes

        assert(nbytes < sizeof(line));
    }
    else                                // No, make it printable
    {
        int n = snprintf(line + nbytes, sizeof(line) - nbytes, "[%02x]", c);

        nbytes += (uint)n;

        assert(nbytes < sizeof(line));
    }
}

           
///
///  @brief    Format string for log file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void log_str(const char *p, uint n)
{
    if (p != NULL)
    {
        while (n-- > 0)
        {
            log_chr(*p++);
        }
    }
}
           

///
///  @brief    Print the current command to terminal and/or log file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void log_cmd(struct cmd *cmd)
{
//    static int cmd_level = 0;

    assert(cmd != NULL);

    FILE *fp = ofiles[OFILE_LOG].fp;

    // If we're not in debug mode, or there is no log file open,
    // or the command is just an ESCape, then skip this.

    if ((fp == NULL && !f.e1.noexec) || cmd->c1 == ESC)
    {
        return;
    }

    return;

    nbytes = 0;

    // Skip any leading whitespace in expression

    while (cmd->expr.len != 0)
    {
        if (isspace(cmd->expr.buf[0]))
        {
            ++cmd->expr.buf;
            --cmd->expr.len;
        }
        else
        {
            break;
        }
    }

    // Skip any trailing whitespace in expression

    while (cmd->expr.len != 0)
    {
        if (isspace(cmd->expr.buf[cmd->expr.len - 1]))
        {
            --cmd->expr.len;
        }
        else
        {
            break;
        }
    }

    // Output the expression

    if (cmd->expr.len != 0)
    {
        log_chr('(');
        log_str(cmd->expr.buf, cmd->expr.len);
        log_chr(')');
        log_chr(SPACE);
    }
   
    log_chr(cmd->colon_set ? ':' : NUL);
    log_chr(cmd->dcolon_set ? ':' : NUL);
    log_chr(cmd->atsign_set ? '@' : NUL);
    log_chr(cmd->c1);
    log_chr(cmd->c2);
    log_chr(cmd->c3);
    log_chr(cmd->qlocal ? '.' : NUL);
    log_chr(cmd->qname);
    log_chr(cmd->atsign_set ? cmd->delim : NUL);
    log_str(cmd->text1.buf, cmd->text1.len);
    log_chr(cmd->text1.buf != NULL ? cmd->delim : NUL);
    log_str(cmd->text2.buf, cmd->text2.len);
    log_chr(cmd->text2.buf != NULL ? cmd->delim : NUL);

    line[nbytes] = NUL;

    if (f.e1.noexec)
    {
        prints("#%05u:  %s\r\n", ++sequence, line);
    }

    if (fp != NULL)                     // Print to log file if it's open
    {
        fprintf(fp, "%s\n", line);
        (void)fflush(fp);
    }
}
