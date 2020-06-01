///
///  @file    ei_cmd.c
///  @brief   Execute EI command.
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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "file.h"


// Local functions

static void close_indirect(void);

static bool open_indirect(struct ifile *ifile);


///
///  @brief    Execute EI command: read TECO command file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EI(struct cmd *cmd)
{
    assert(cmd != NULL);

    close_indirect();                   // Close any open file

    // If EIfile`, then try to open file

    if (cmd->text1.len != 0)
    {
        struct ifile *ifile = &ifiles[IFILE_INDIRECT];

        init_filename(&ifile->name, cmd->text1.buf, cmd->text1.len);

        bool success = open_indirect(ifile);

        if (!success)
        {
            if (strchr(ifile->name, '.') == NULL)
            {
                uint len = (uint)strlen(ifile->name);
                const char *ext = ".tec";

                ifile->name = expand_mem(ifile->name, len,
                                         len + (uint)strlen(ext));

                last_file = ifile->name;

                strcat(ifile->name, ext);

                success = open_indirect(ifile);
            }
        }            

        if (!success)
        {
            if (cmd->colon_set)
            {
                push_expr(TECO_FAILURE, EXPR_VALUE);
            }
            else
            {
                prints_err(E_FNF, last_file);
            }
        }
        else if (cmd->colon_set)
        {
            push_expr(TECO_SUCCESS, EXPR_VALUE);
        }
    }
}


///
///  @brief    Close indirect command file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void close_indirect(void)
{
    close_input(IFILE_INDIRECT);

    if (f.e0.exit)                      // Command-line option to exit?
    {
        exit(EXIT_SUCCESS);             // Yes, we're done
    }
}


///
///  @brief    Open indirect file.
///
///  @returns  true if success, false if failure.
///
////////////////////////////////////////////////////////////////////////////////

static bool open_indirect(struct ifile *ifile)
{
    if (open_input(ifile) == EXIT_SUCCESS)
    {
        return true;
    }

    if (errno == ENOENT || errno == ENODEV)
    {
        return false;
    }

    // Here if error was something other than a simple "file not found".

    prints_err(E_FNF, last_file);
}


///
///  @brief    Read input from indirect file if one is open.
///
///  @returns  -1 if we have a complete command, 1 if we have a partial
///            command, 0 if no data is available.
///
////////////////////////////////////////////////////////////////////////////////

int read_indirect(void)
{
    struct ifile *stream = &ifiles[IFILE_INDIRECT];

    if (stream->fp == NULL)
    {
        return 0;
    }

    // If a file is open, then we may have data, so clear the command buffer,
    // since we can only get here after the completion of a command string.

    reset_cbuf();

    int c, last = EOF;                  // Current and previous characters read
    bool nbytes = false;                // true if we've read any data

    while ((c = fgetc(stream->fp)) != EOF)
    {
        store_cbuf(c);

        if (c == ESC && last == ESC)
        {
            return -1;                  // Done if double escape seen
        }

        last = c;
        nbytes = true;
    }

    // File ended without a double escape, so close it.

    close_indirect();

    return nbytes ? 1 : 0;
}


///
///  @brief    Reset indirect command file buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_indirect(void)
{
    close_indirect();
}
