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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "file.h"


// Local functions


static void exit_EI(void);


///
///  @brief    Execute EI command: read TECO command file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EI(struct cmd *cmd)
{
    assert(cmd != NULL);

    const char *buf = cmd->text1.buf;
    uint len = cmd->text1.len;
    uint stream = IFILE_INDIRECT;
    char name[len + 4 + 1];             // Allow room for possible '.tec'

    close_input(stream);                // Close any open file

    if (len == 0)
    {
        return;
    }

    assert(buf != NULL);

    // Here if EIfile`, so try to open file.

    len = (uint)sprintf(name, "%.*s", (int)len, buf);

    // Treat first open as colon-modified to avoid error. This allows
    // us to try a second open with .tec file type.

    struct ifile *ifile = open_input(name, len, stream, (bool)true);
    
    if (ifile == NULL)
    {
        if (strchr(last_file, '.') == NULL)
        {
            len = (uint)sprintf(name, "%s.tec", last_file);

            ifile = open_input(name, len, stream, cmd->colon_set);
        }
        else if (!cmd->colon_set)
        {
            prints_err(E_INP, last_file); // Input file name
        }
    }

    // Note: open_input() only returns NULL for colon-modified command.

    if (ifile == NULL)
    {
        push_expr(TECO_FAILURE, EXPR_VALUE);
    }
    else if (cmd->colon_set)
    {
        push_expr(TECO_SUCCESS, EXPR_VALUE);
    }
}


///
///  @brief    Clean up memory before we exit from TECO.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void exit_EI(void)
{
    reset_indirect();
}


///
///  @brief    Initialization for EI commands.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void init_EI(void)
{
    register_exit(exit_EI);
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

    close_input(IFILE_INDIRECT);

    if (nbytes)
    {
        return 1;                       // Partial command pending
    }
    else if (current->len >= 2 &&
             current->buf[current->len - 2] == ESC &&
             current->buf[current->len - 1] == ESC)
    {
        return -1;                      // Complete command in buffer
    }
    else
    {
        return 0;                       // No command pending
    }
}


///
///  @brief    Reset indirect command file buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_indirect(void)
{
    close_input(IFILE_INDIRECT);
}
