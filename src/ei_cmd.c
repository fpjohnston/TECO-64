///
///  @file    ei_cmd.c
///  @brief   Execute EI command.
///
///  @copyright 2019-2021 Franklin P. Johnston / Nowwith Treble Software
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
#include <string.h>

#include "teco.h"
#include "eflags.h"
#include "estack.h"
#include "exec.h"
#include "file.h"

#include "cbuf.h"


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

static struct ei_block *reset_new(void);


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
    if (ei_new != NULL && cbuf == &ei_new->buf)
    {
        return 1;
    }
    else if (cbuf == &ei_old)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}


///
///  @brief    Execute "EI" command: read TECO command file. This is handled in
///            one of two ways: preset where input is read from after execution
///            of the command string has completed, as DEC TECOs have done, or
///            immediately execute the contents of the file as though it were a
///            as TECO C has done. Which behavior is used depends on the new_ei
///            bit in the E1 extended features flag. By default, this bit is
///            enabled.
///
///            N.B.: the new_ei bit should not be changed after TECO starts up,
///            or while executing EI commands, as this could have unpredictable
///            and undesirable results.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

void exec_EI(struct cmd *cmd)
{
    assert(cmd != NULL);

    const char *buf = cmd->text1.data;
    uint len        = cmd->text1.len;
    uint stream     = IFILE_INDIRECT;

    close_input(stream);                // Close any open file

    if (f.e0.init || f.e1.new_ei)       // New-style EI
    {
        if (len == 0)
        {
            ei_new = reset_new();

            if (cmd->colon)
            {
                push_x(-1, X_OPERAND);
            }

            return;
        }

        struct ei_block *ei_cmd = alloc_mem((uint)sizeof(*ei_cmd));

        ei_cmd->buf.data = alloc_mem((uint)sizeof(ei_cmd->buf));
        ei_cmd->buf.size = 0;
        ei_cmd->buf.len  = 0;
        ei_cmd->buf.pos  = 0;
        ei_cmd->next     = ei_new;

        ei_new = ei_cmd;

        char *temp = init_filename(buf, len, cmd->colon);

        if (temp != NULL)
        {
            char name[strlen(temp) + 1];

            strcpy(name, temp);

            free_mem(&temp);

            if (open_command(name, len, stream, cmd->colon, &ei_new->buf))
            {
                if (cmd->colon)
                {
                    push_x(-1, X_OPERAND);
                }

                exec_macro(&ei_new->buf, cmd);

                return;
            }
        }
    }
    else                                // Old-style EI
    {
        if (len == 0)                   // @EI//?
        {
            longjmp(jump_main, MAIN_CTRLC); // Yes, act like CTRL/C typed
        }

        char *temp = init_filename(buf, len, cmd->colon);

        if (temp != NULL)
        {
            char name[strlen(temp) + 1];

            strcpy(name, temp);

            free_mem(&temp);

            if (open_command(name, len, stream, cmd->colon, &ei_old))
            {
                if (cmd->colon)
                {
                    push_x(-1, X_OPERAND);
                }

                return;
            }
        }
    }

    if (cmd->colon)
    {
        push_x(0, X_OPERAND);
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
///  @returns  true if have command to execute, else false.
///
////////////////////////////////////////////////////////////////////////////////

bool read_EI(void)
{
    if (ei_old.data == NULL || ei_old.pos == ei_old.len)
    {
        return false;
    }
    else
    {
        cbuf = &ei_old;                 // Use our command string

        return true;
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
    ei_old.size = 0;
    ei_old.len  = 0;
    ei_old.pos  = 0;

    free_mem(&ei_old.data);

    while ((ei_new = reset_new()) != NULL)
    {
        ;
    }
}


///
///  @brief    Reset buffer for new-style EI command.
///
///  @returns  Nothing.
///
////////////////////////////////////////////////////////////////////////////////

static struct ei_block *reset_new(void)
{
    if (ei_new == NULL)
    {
        return NULL;
    }

    struct ei_block *ei_cmd = ei_new->next;

    ei_new->buf.size = 0;
    ei_new->buf.len  = 0;
    ei_new->buf.pos  = 0;

    free_mem(&ei_new->buf.data);
    free_mem(&ei_new);

    return ei_cmd;
}
