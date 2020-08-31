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

static struct buffer ei_cmd;            ///< EI command string

// Local functions

static void exit_EI(void);

static uint trim_white(struct buffer *buf);


///
///  @brief    Execute EI command: read TECO command file. This is handled in
///            one of two ways: preset where input is read from after execution
///            of the current command string has completed, as DEC TECOs have
///            done, or immediately execute the contents of the file as though
///            it were a macro, as TECO C has done. Which behavior is used
///            depends on the eimacro bit in the E1 extended features flag. By
///            default, this bit is enabled.
///
///            N.B.: the eimacro bit should not be changed after TECO starts
///            up. Changing it in the midst of executing EI commands could have
///            unpredictable and undesirable results.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EI(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    const char *buf     = cmd->text1.data;
    uint len            = cmd->text1.len;
    uint stream         = IFILE_INDIRECT;
    struct buffer macro =
    {
        .data = NULL,
        .size = 0,
        .len  = 0,
        .pos  = 0
    };

    close_input(stream);                // Close any open file

    if (open_command(buf, len, stream, cmd->colon, &macro))
    {
        if (trim_white(&macro) == 0)    // Any file data to process?
        {
            free_mem(&macro.data);      // Discard allocated memory
        }
        else if (f.e1.eimacro)          // EI command is executed as macro
        {
            exec_macro(&macro);         // Execute it like a macro
            free_mem(&macro.data);      // Discard allocated memory
        }
        else                            // EI command presets input
        {
            ei_cmd = macro;             // Copy data for read_indirect()
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
    if (f.e1.eimacro)
    {
        return 0;
    }

    if (ei_cmd.len == 0)
    {
        free_mem(&ei_cmd.data);
    }

    if (ei_cmd.data == NULL)
    {
        return 0;
    }

    set_cbuf(&ei_cmd);                  // Use our command string

    // Check to see if buffer ends with a double ESCape, or some combination
    // of an ESCape and its equivalent 2-chr. ASCII construct (i.e., ^[ ).

    uint len = cbuf->len;
    char *end = cbuf->data + len;
    uint nbytes = 0;                    // No. of bytes terminating string

    if (len >= 2)                       // 2+ chrs. for ESC+ESC
    {
        int c1 = *--end;
        int c2 = *--end;

        if (c1 == ESC && c2 == ESC)
        {
            nbytes = 2;
        }

        if (len >= 3 && nbytes == 0)    // 3+ chrs. for ESC+^[ or ^[+ESC
        {
            int c3 = *--end;

            if ((c1 == ESC && c2 == '[' && c3 == '^') ||
                (c1 == '[' && c2 == '^' && c3 == ESC))
            {
                nbytes = 3;
            }

            if (len >= 4 && nbytes == 0) // 4+ chrs. for ^[ + ^[
            {
                int c4 = *--end;

                if (c1 == '[' && c2 == '^' && c3 == '[' && c4 == '^')
                {
                    nbytes = 4;
                }
            }
        }

        if (nbytes != 0)                // Did we find end of command?
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
    free_mem(&ei_cmd.data);
}


///
///  @brief    Delete any whitespace at end of buffer.
///
///  @returns  No. of bytes left in buffer.
///
////////////////////////////////////////////////////////////////////////////////

static uint trim_white(struct buffer *buf)
{
    assert(buf != NULL);
    assert(buf->data != NULL);

    // Delete any whitespace at end of command string.

    while (buf->len > 0)
    {
        int c = buf->data[buf->len - 1];

        if (!isspace(c) || c == TAB)
        {
            break;
        }

        --buf->len;
    }

    return buf->len;
}
