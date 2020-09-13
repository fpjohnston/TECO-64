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

///  @struct ei_list
///  @brief  Linked list structure for EI command strings.

struct ei_list
{
    struct ei_list *next;           ///< Next EI block
    struct buffer buf;              ///< EI command string buffer
};

static struct ei_list *current = NULL;  ///< Current EI command string

// Local functions

static void exit_EI(void);

static uint trim_white(struct buffer *buf);


///
///  @brief    Execute EI command: read TECO command file. This is handled in
///            one of two ways: preset where input is read from after execution
///            of the current command string has completed, as DEC TECOs have
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

    if (len == 0)                       // @EI//?
    {
        if (current != NULL && current->buf.data == cbuf->data)
        {
            current->buf.pos = current->buf.len = 0;
            cbuf->pos = cbuf->len = 0;
        }

        return;
    }

    // The following exists to ensure proper handling of old-style EI commands
    // inside of macros. Since such EI commands just switch where input is read
    // from, then we ensure that we reset any buffers in use before we create
    // a new one.

    if (current != NULL && !f.e1.alt_ei && !cmd->dcolon)
    {
        free_mem(&current->buf.data);
        free_mem(&current);

        cbuf->pos = cbuf->len = 0;
    }

    // Allocate an EI block, to process the macro we are about to read in, and
    // make it the current one. We do it this way in order to avoid any memory
    // leaks in the event an error occurs in one of the functions we call to
    // open the indirect command file.

    struct ei_list *ei_cmd = alloc_mem((uint)sizeof(*ei_cmd));

    ei_cmd->buf.data = alloc_mem((uint)sizeof(ei_cmd->buf));
    ei_cmd->buf.size = 0;
    ei_cmd->buf.len  = 0;
    ei_cmd->buf.pos  = 0;
    ei_cmd->next     = current;

    current = ei_cmd;

    if (open_command(buf, len, stream, cmd->colon, &ei_cmd->buf))
    {
        if (trim_white(&ei_cmd->buf) == 0) // Any file data to process?
        {
            ;
        }
        else if (f.e1.alt_ei || cmd->dcolon)
        {                               // EI command is executed as macro
            exec_macro(&ei_cmd->buf, NULL);
        }
        else                            // Old-style EI command
        {
            return;
        }
    }

    // Here after processing new-style EI command, or
    // if there is no real data to process in EI file.

    current = ei_cmd->next;

    free_mem(&ei_cmd->buf.data);
    free_mem(&ei_cmd);
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
    if (f.e1.alt_ei || current == NULL)
    {
        return 0;
    }

    if (current->buf.len == 0)
    {
        struct ei_list *ei_block = current;

        current = ei_block->next;

        free_mem(&ei_block->buf.data);
        free_mem(&ei_block);
    }

    if (current == NULL || current->buf.data == NULL)
    {
        return 0;
    }

    set_cbuf(&current->buf);              // Use our command string

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
    struct ei_list *ei;

    while ((ei = current) != NULL)
    {
        current = ei->next;

        free_mem(&ei->buf.data);
        free_mem(&ei);
    }
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
