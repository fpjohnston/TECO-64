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

#include "teco.h"
#include "ascii.h"
#include "eflags.h"
#include "estack.h"
#include "exec.h"
#include "file.h"

static struct buffer ei_data;           ///< EI command string

static const char ex_cmd[] = "EX";      ///< EX command

// @var    Size of EX command in bytes (ignoring trailing NUL)

#define EX_SIZE     (sizeof(ex_cmd) - 1)

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

    close_input(stream);                // Close any open file

    ei_data.size = EX_SIZE;             // Add room for possible EX command

    if (open_implicit(buf, len, stream, cmd->colon, &ei_data))
    {
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
    char *endbuf = command->buf + len;
    uint nbytes = 0;                    // No. of bytes terminating string

    if (len >= 2)                       // 2+ chrs. for ESC+ESC
    {
        int c1 = *--endbuf;
        int c2 = *--endbuf;

        if (c1 == ESC && c2 == ESC)
        {
            nbytes = 2;
        }

        if (len >= 3 && nbytes == 0)    // 3+ chrs. for ESC+^[ or ^[+ESC
        {
            int c3 = *--endbuf;

            if ((c1 == ESC && c2 == '[' && c3 == '^') ||
                (c1 == '[' && c2 == '^' && c3 == ESC))
            {
                nbytes = 3;
            }

            if (len >= 4 && nbytes == 0) // 4+ chrs. for ^[ + ^[
            {
                int c4 = *--endbuf;

                if (c1 == '[' && c2 == '^' && c3 == '[' && c4 == '^')
                {
                    nbytes = 4;
                }
            }
        }

        if (nbytes != 0)                // Did we find end of command?
        {
            // If --exit option was used, then insert EX command before the
            // the end of the command string to ensure that we exit TECO.

            if (f.e0.exit)
            {
                memmove(endbuf + nbytes, endbuf, EX_SIZE);
                memcpy(endbuf, ex_cmd, EX_SIZE);

                command->len += EX_SIZE; // Add in length of EX command
            }

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
