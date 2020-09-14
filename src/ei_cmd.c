///
///  @file    ei_cmd.c
///  @brief   Execute EI command.
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

///  @var    ei_old
///  @brief  Buffer for old-style EI commands

static struct buffer ei_old;

///  @struct ei_block
///  @brief  Linked list structure for EI command strings.

struct ei_block
{
    struct ei_block *next;          ///< Next EI block
    struct buffer buf;              ///< EI command string buffer
};

///   @var    ei_new
///   @brief  Linked list for new-style EI commands

static struct ei_block *ei_new = NULL;

// Local functions

static void exit_EI(void);

///
///  @brief    Check to see if any EI commands are active.
///
///  @returns  +1 -> New-style EI command is active.
///            -1 -> Old-style EI command is active.
///             0 -> No EI command is active.
///
////////////////////////////////////////////////////////////////////////////////

int check_EI(void)
{
    if (cbuf != NULL)
    {
        if (ei_new != NULL && cbuf->data == ei_new->buf.data)
        {
            return 1;
        }
        else if (cbuf->data == ei_old.data)
        {
            return -1;
        }
    }

    return 0;
}


///
///  @brief    Execute EI command: read TECO command file. This is handled in
///            one of two ways: preset where input is read from after execution
///            of the ehead command string has completed, as DEC TECOs have
///            done, or immediately execute the contents of the file as though
///            it were a macro, as TECO C has done. Which behavior is used
///            depends on the alt_ei bit in the E1 extended features flag. By
///            default, this bit is enabled.
///
///            N.B.: the alt_ei bit should not be changed after TECO starts up,
///            or while executing EI commands, as this could have unpredictable
///            and undesirable results.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EI(struct cmd *cmd)
{
    assert(cmd != NULL);                // Error if no command block

    const char *buf = cmd->text1.data;
    uint len        = cmd->text1.len;
    uint stream     = IFILE_INDIRECT;

    close_input(stream);                // Close any open file

    if (f.e0.init || f.e1.new_ei)
    {
        struct ei_block *ei_cmd;

        if (len != 0)
        {
            ei_cmd           = alloc_mem((uint)sizeof(*ei_cmd));
            ei_cmd->buf.data = alloc_mem((uint)sizeof(ei_cmd->buf));
            ei_cmd->buf.size = 0;
            ei_cmd->buf.len  = 0;
            ei_cmd->buf.pos  = 0;
            ei_cmd->next     = ei_new;

            ei_new = ei_cmd;

            if (open_command(buf, len, stream, cmd->colon, &ei_new->buf))
            {
                exec_macro(&ei_new->buf, NULL);

                return;
            }
        }

        if ((ei_cmd = ei_new) != NULL)
        {
            if (cbuf != NULL && cbuf->data == ei_cmd->buf.data)
            {
                cbuf->len = 0;
                cbuf->pos = 0;
            }

            ei_new = ei_cmd->next;
            
            ei_cmd->buf.size = 0;
            ei_cmd->buf.len  = 0;
            ei_cmd->buf.pos  = 0;

            free_mem(&ei_cmd->buf.data);
            free_mem(&ei_cmd);
        }
    }
    else
    {
        if (len != 0)
        {
            if (open_command(buf, len, stream, cmd->colon, &ei_old))
            {
                return;
            }
        }

        if (cbuf != NULL && cbuf->data == ei_old.data)
        {
            cbuf->len = 0;
            cbuf->pos = 0;
        }

        ei_old.size = 0;
        ei_old.len  = 0;
        ei_old.pos  = 0;

        free_mem(&ei_old);
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
    if (ei_old.data == NULL || ei_old.pos == ei_old.len)
    {
        ei_old.len = 0;
        ei_old.pos = 0;

        free_mem(&ei_old);

        return 0;
    }

    set_cbuf(&ei_old);                  // Use our command string

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
    ei_old.size = 0;
    ei_old.len  = 0;
    ei_old.pos  = 0;

    free_mem(&ei_old);

    struct ei_block *ei_cmd;

    while ((ei_cmd = ei_new) != NULL)
    {
        ei_new = ei_cmd->next;

        ei_cmd->buf.size = 0;
        ei_cmd->buf.len  = 0;
        ei_cmd->buf.pos  = 0;

        free_mem(&ei_cmd->buf);
        free_mem(&ei_cmd);
    }
}
