///
///  @file    ei_cmd.c
///  @brief   Execute EI command.
///
///  @bug     No known bugs.
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
#include <unistd.h>
#include <sys/stat.h>

#include "teco.h"
#include "ascii.h"
#include "errors.h"
#include "estack.h"
#include "exec.h"
#include "file.h"

static struct buffer ei_data;

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

            ifile = open_input(name, len, stream, cmd->colon);
        }
        else if (!cmd->colon)
        {
            throw(E_FNF, last_file);    // Input file name
        }
    }

    // Note: open_input() only returns NULL for colon-modified command.

    if (ifile == NULL)
    {
        push_expr(0, EXPR_VALUE);

        return;
    }
    else if (cmd->colon)
    {
        push_expr(-1, EXPR_VALUE);
    }

    struct stat file_stat;

    if (stat(last_file, &file_stat))
    {
        throw(E_SYS, last_file);        // Unexpected system error
    }

    uint size = (uint)file_stat.st_size;

    free_mem(&ei_data.buf);

    // If there's data in the file, then allocate a buffer for it.

    if (size != 0)
    {
        ei_data.len  = size;
        ei_data.pos  = 0;
        ei_data.size = size;
        ei_data.buf  = alloc_mem(size);

        if (fread(ei_data.buf, 1uL, (ulong)size, ifile->fp) != size)
        {
            throw(E_SYS, ifile->name);  // Unexpected system error
        }

        // Start at end of command string, and back up until we find a
        // non-whitespace character. This allows the use of such things
        // as spaces or CRLFs after a double escape. Note that we don't
        // skip TABs, since those are TECO commands.

        while (ei_data.len > 0)
        {
            int c = ei_data.buf[ei_data.len - 1];

            if (!isspace(c) || c == TAB)
            {
                break;
            }

            --ei_data.len;
        }

        if (ei_data.len == 0)           // Any data left?
        {
            free_mem(&ei_data.buf);     // No, discard allocated memory
        }
    }

    close_input(IFILE_INDIRECT);
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
    if (ei_data.len == 0)
    {
        free_mem(&ei_data.buf);
    }

    if (ei_data.buf == NULL)
    {
        return 0;
    }

    set_cbuf(&ei_data);                 // Use command string we allocated

    // Check to see if buffer ends with a double ESCape, or some combination
    // of an ESCape and its equivalent 2-chr. ASCII construct (i.e., ^[ ).

    uint len = command->len;
    int c1 = EOF, c2 = EOF, c3 = EOF, c4 = EOF;

    if (len >= 2)                       // 2+ chrs. for ESC+ESC
    {
        c1 = command->buf[len - 1];
        c2 = command->buf[len - 2];

        if (len >= 3)                   // 3+ chrs. for ESC+^[ or ^[+ESC
        {
            c3 = command->buf[len - 3];

            if (len >= 4)               // 4+ chrs. for ^[ + ^[
            {
                c4 = command->buf[len - 4];
            }
        }

        if ((c1 == ESC && c2 == ESC) ||
            (c1 == ESC && c2 == '[' && c3 == '^') ||
            (c1 == '[' && c2 == '^' && c3 == ESC) ||
            (c1 == '[' && c2 == '^' && c3 == '[' && c4 == '^'))
        {
            return -1;                  // Say we have a complete command
        }
    }

    return 1;                           // We only have a partial command
}


///
///  @brief    Reset indirect command file buffer.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void reset_indirect(void)
{
    free_mem(&ei_data);

    close_input(IFILE_INDIRECT);
}
