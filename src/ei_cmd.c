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
#include <unistd.h>

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "file.h"


bool ei_active = false;                 ///< true to flag EI in progress

static struct buffer *ei_buf;           ///< Buffer for EI commands

static struct buffer *saved_buf;        ///< Saved command buffer

// Local functions

static void close_indirect(void);

static bool open_indirect(struct ifile *ifile);

static void exit_EI(void);


///
///  @brief    Close indirect command file.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static void close_indirect(void)
{
    close_input(IFILE_INDIRECT);

    ei_active = false;

    if (saved_buf != NULL)
    {
        set_cbuf(saved_buf);
        saved_buf = NULL;
    }

    if (ei_buf != NULL)
    {
        free_mem(&ei_buf->buf);
        free_mem(&ei_buf);
    }

    if (f.e0.exit)                      // Command-line option to exit?
    {
        exit(EXIT_SUCCESS);             // Yes, we're done
    }
}


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
        int len = (int)cmd->text1.len;
        char name1[len + 1];
        char name2[len + 4 + 1];

        len = sprintf(name1, "%.*s", len, cmd->text1.buf);

        init_filename(&ifile->name, name1, (uint)len);

        last_file = ifile->name;

        // Try to open file; if failure, and file name does
        // not have a dot, then add ".tec" and try again.

        if (access(name1, F_OK) != 0 && strchr(name1, '.') == NULL)
        {
            len = sprintf(name2, "%s%s", name1, ".tec");

            if (access(name2, F_OK) == 0)
            {
                init_filename(&ifile->name, name2, (uint)len);
            }
        }

        if (open_indirect(ifile))
        {
            if (cmd->colon_set)
            {
                push_expr(TECO_SUCCESS, EXPR_VALUE);
            }
        }
        else
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

        assert(saved_buf == NULL);
        assert(ei_buf == NULL);

        saved_buf = get_cbuf();
        ei_buf = alloc_mem((uint)sizeof(struct buffer));

        ei_buf->len  = 0;
        ei_buf->pos  = 0;
        ei_buf->size = STR_SIZE_INIT;
        ei_buf->buf  = alloc_mem(ei_buf->size);

        set_cbuf(ei_buf);

        ei_active = true;
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
    close_indirect();
}
